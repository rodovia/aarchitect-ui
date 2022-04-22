#if defined(_MSC_VER) || defined(__clang__)
#	define _ITERATOR_DEBUG_LEVEL 0
#endif

#include <libwebsockets.h>
#include <stdio.h>
#include "protocol.h"
#include "message.h"
#include <exthost.h>
#include "config.h"

#include <string>
#include <filesystem>
#include <vector>

std::vector<ext::CThreadedPlugin> g_LoadedPlugins;

struct MESSAGE
{
    void* payload;
    size_t len;
    char binary;
    char final;
    char first;
};

static void MinimalFreeMessage(void* _msg)
{
    struct MESSAGE* msg = static_cast<struct MESSAGE*>(_msg);
    free(msg);
}

int DefProtocolProc(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len)
{
    struct pss_data* psd = (struct pss_data*) user;
    struct vhd_server* vhd = (struct vhd_server *)
                        lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                lws_get_protocol(wsi));
    
    const struct MESSAGE* pmsg;
    struct MESSAGE msg;
    int flags, m, n;

    fprintf(stdout, "%i", reason);

    switch(reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            
            vhd = static_cast<struct vhd_server*>(lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), 
                                            lws_get_protocol(wsi),
                                            sizeof(struct vhd_server)));
            if (!vhd)
                return -1;

            vhd->ctx = lws_get_context(wsi);
            vhd->vhost = lws_get_vhost(wsi);

            vhd->interrupted = static_cast<int*>(calloc(1, sizeof(int)));
            vhd->options = static_cast<int*>(calloc(1, sizeof(int)));
            break;
        case LWS_CALLBACK_ESTABLISHED:
            printf("OII\n");
            lws_ll_fwd_insert(psd, pssList, vhd->pssList);
            vhd->ring = lws_ring_create(sizeof(struct MESSAGE), 205, 
                                            MinimalFreeMessage);

            if (!vhd->ring)
                return 1;
            psd->tail = 0;
            break;
        /* LWS_TIMER_CALLBACK is reserved for disconnecting clients */
        case LWS_CALLBACK_TIMER:
            return -1; 
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            if (psd->writeConsumePending)
            {
                lws_ring_consume_single_tail(psd->ring, &psd->tail, 1);
                psd->writeConsumePending = 0;
            }

            pmsg = static_cast<const struct MESSAGE*>(lws_ring_get_element(vhd->ring, NULL));
            if (!pmsg)
            {
                lwsl_user("RING is null");
                break;
            }

            flags = lws_write_ws_flags(pmsg->binary 
                            ? LWS_WRITE_BINARY : LWS_WRITE_TEXT,
                            pmsg->first, pmsg->final);


            m = lws_write(wsi, (uint8_t*) pmsg->payload, pmsg->len,
                                    (enum lws_write_protocol) flags);
            
            if (m < (int)pmsg->len)
            {
                return -1;
            }

            if (psd->flowControlled 
                    && (int) lws_ring_get_count_free_elements(psd->ring) > 205 - 15)
                {
                    lws_rx_flow_control(wsi, 1);
                    psd->flowControlled = 0;
                }

            if ((*vhd->options & 1) && pmsg && pmsg->final)
            {
                /* psd->completed = 1; */
            }

            break;
        case LWS_CALLBACK_RECEIVE:
            lwsl_user("LWS_CALLBACK_RECEIVE: %4d (rpp %5d, first %d, "
                          "last %d, bin %d, msglen %d (+ %d = %d))\n",
                          (int)len, (int)lws_remaining_packet_payload(wsi),
                          lws_is_first_fragment(wsi),
                          lws_is_final_fragment(wsi),
                          lws_frame_is_binary(wsi), psd->msgLen, (int)len,
                          (int)psd->msgLen + (int)len);
            
            ParseBytes(psd, (const char*) in, len);

            msg.binary = (char) lws_frame_is_binary(wsi);
            msg.final = (char) lws_is_final_fragment(wsi);
            msg.first = (char) lws_is_first_fragment(wsi);

            n = (int) lws_ring_get_count_free_elements(vhd->ring);

            if (msg.final)
                psd->msgLen = 0;
            else
                psd->msgLen += (int32_t) len;

            if (n < 3 && !psd->flowControlled)
            {
                lwsl_info("Flow controlled");
                psd->flowControlled = 1;
                lws_rx_flow_control(wsi, 0);
            }
            break;

        case LWS_CALLBACK_CLOSED:
            lwsl_info("Closing");
            lws_ring_destroy(vhd->ring);
            lws_ll_fwd_remove(struct pss_data, pssList, psd, vhd->pssList);

            if (*vhd->options & 1) 
            {
                if (!*vhd->interrupted)
                {
                    *vhd->interrupted = 1 + psd->completed;
                }
                lws_cancel_service(lws_get_context(wsi));
            }
            break;

        default:
            break;  
    }

    return 0;
}
