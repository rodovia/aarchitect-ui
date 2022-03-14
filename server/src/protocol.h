#pragma once
#define LWS_WITH_NETWORK
#include <libwebsockets.h>

struct pss_data
{
    struct pss_data* pssList;
    struct lws* wsi;
    struct lws_ring* ring;

    uint32_t tail;
    int32_t msgLen;
    int32_t completed;
    
    char writeConsumePending;
    char flowControlled;
};

struct vhd_server
{
    struct lws_context* ctx;
    struct lws_vhost* vhost;
    struct pss_data *pssList;
    struct lws_ring* ring;
    int* interrupted;
    int* options;
};

#ifdef __cplusplus
extern "C" {
#endif

int DefProtocolProc(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#define LWS_PLUGIN_PROTOCOL_AARCHITECT_SERVER \
    {                           \
    "aarchitect-chat",           \
    DefProtocolProc,              \
    sizeof(struct pss_data),     \
    1024,                           \
    0, NULL, 0                       \
    }
