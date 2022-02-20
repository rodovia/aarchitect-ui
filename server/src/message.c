#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "protocol.h"
#include "message.h"
#include "conversions.h"

#define TIMER_TIMEOUT 500000 /* half second so LWS can have time to send the 
                                disconnect payload*/

#include <connection_opcodes.h>
#include <cn-cbor/cn-cbor.h>

void aarSendMessage(struct pss_data* wsi, uint8_t* bytes)
{
    lws_ring_insert(wsi->ring, bytes, ARRAYLEN(bytes));
    lws_callback_on_writable(wsi->wsi);
}

static uint8_t* PrepareSend(cn_cbor* map, size_t* bwr)
{
    size_t bufferSize = cn_cbor_encoder_write(NULL, 0, 0, map);
    uint8_t* buffer = (uint8_t*) malloc(sizeof(uint8_t) * bufferSize + LWS_PRE);
    *bwr = cn_cbor_encoder_write(buffer, LWS_PRE, bufferSize, map);

    return buffer;
}

static void SendDisconnectPayload(struct pss_data* wsi)
{
    cn_cbor* map = cn_cbor_map_create(NULL);
    size_t* bytesWrt = malloc(sizeof(size_t));
    cn_cbor_mapput_string(map, "type", 
            cn_cbor_int_create((int64_t) DISCNT_PACKET, NULL), NULL);

    // TODO: warn if bytes written is less than bufferSize
    uint8_t* buffer = PrepareSend(map, bytesWrt);
    aarSendMessage(wsi, buffer);
    free(bytesWrt);
    aarRemoveUser(*wsi);

    /* actual disconnection */
    lws_set_timer_usecs(wsi->wsi, TIMER_TIMEOUT);
}

static cn_cbor* aarBuildUserAddPacket(const struct aaruserdata* user)
{
    cn_cbor* map = cn_cbor_map_create(NULL);
    cn_cbor_mapput_string(map, "type", 
                        cn_cbor_int_create(EVENT_PACKET, NULL), NULL);
    
    cn_cbor_mapput_string(map, "event", 
                        cn_cbor_string_create(USER_ADD, NULL), NULL);

    cn_cbor_mapput_string(map, "user", ConvertUDCBOR(user), NULL);
    return map;
}

static void aarSendUserAddPacket(struct aaruserdata aud)
{
    struct pss_data* conns = aarGetConnectedClients();
    cn_cbor* map = aarBuildUserAddPacket(&aud);

    ssize_t maps = cn_cbor_encoder_write(NULL, 0, 0, map);
    uint8_t* buffer = malloc(maps + LWS_PRE);
    cn_cbor_encoder_write(buffer, 0, maps, map);

    for (int i = 0; i < ARRAYLEN(conns); i++)
    {
        struct pss_data conn = conns[i];
        aarSendMessage(&conn, buffer);
    }
}

void ParseBytes(struct pss_data* wsi, uint8_t *buffer, size_t len)
{
    cn_cbor* cb = cn_cbor_decode(buffer, len, NULL);
    if (cb->type != CN_CBOR_MAP)
    {
        SendDisconnectPayload(wsi);
        return;
    }

    if (cn_cbor_mapget_string(cb, "type")->v.uint == HELLO_PACKET)
    {
        cn_cbor* avVal;
        const uint8_t* avatar;
        const char* uName = cn_cbor_mapget_string(cb, "username")->v.str;
        
        avVal = cn_cbor_mapget_string(cb, "avatar");

        if (avVal == NULL || avVal->v.sint == 0)
            avatar = NULL;
        else
            avatar = cn_cbor_mapget_string(cb, "avatar")->v.bytes;
        
        struct aaruserdata aud = { uName, avatar };
        aarPutUser(*wsi, aud);
        aarSendUserAddPacket(aud);
    }
}
