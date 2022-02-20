#include <aarchitect.h>
#include <stdlib.h>
#include <malloc.h>
#include <cn-cbor/cn-cbor.h>
#include <connection_opcodes.h>

#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "connection.h"

static size_t MessageEncodeCBOR(const cn_cbor* cn, uint8_t** outBuffer)
{
    size_t sz = cn_cbor_encoder_write(NULL, 0, 0, cn);
    aarPrettyLog("%i", sz);
    uint8_t* buffer = malloc(sz + LWS_PRE + 10);
    
    size_t lpSz = cn_cbor_encoder_write(buffer, LWS_PRE, sz, cn);
    aarPrettyLog("%i", lpSz);

    (*outBuffer) = buffer;

    return sz;
}

void connSendMessage(struct PERVHOSTDATA__MINIMAL host, struct MESSAGE msg)
{
    lws_ring_insert(host.ring, &msg, 1);
    lws_callback_on_writable(host.client_wsi);
}

void connSendMessageCreatePayload(struct PERVHOSTDATA__MINIMAL* host, const char* content)
{
    cn_cbor_errback err;
    cn_cbor* map = cn_cbor_map_create(&err);
    cn_cbor_mapput_string(map, "type", 
                            cn_cbor_int_create(EVENT_PACKET, &err), &err);
    cn_cbor_mapput_string(map, "event",
                         cn_cbor_string_create(MESSAGE_CREATE, &err), &err);
    cn_cbor_mapput_string(map, "content",
                        cn_cbor_string_create(content, &err), &err);

    ssize_t sz = cn_cbor_encoder_write(NULL, 0, 0, map);

    uint8_t* buffer = malloc(sz + LWS_PRE);
    ssize_t bufSz = cn_cbor_encoder_write(buffer, 0, sz, map);
    struct MESSAGE nMess = { buffer, bufSz };
    connSendMessage(*host, nMess);
}

void connSendHelloPayload(struct PERVHOSTDATA__MINIMAL* host, 
                            aarUserConfiguration* lpuCfg)
{
    cn_cbor_errback err;
    cn_cbor* cb = cn_cbor_map_create(&err);
    cn_cbor_mapput_string(cb, "username", 
                cn_cbor_string_create(lpuCfg->username, &err), &err);
        
    if (err.err != CN_CBOR_NO_ERROR)
    {
        aarPrettyLog("WARNING: Error while creating HELLO payload: %i", err);
    }

    cn_cbor* avData = cn_cbor_data_create(lpuCfg->avatar, 
                                    sizeof(lpuCfg->avatar) / sizeof(*lpuCfg->avatar), 
                                    &err);

    if (err.err != CN_CBOR_NO_ERROR)
    {
        aarPrettyLog("WARNING: Error while creating HELLO payload (avData): %i", err);
    }
    cn_cbor_mapput_string(cb, "type",
                cn_cbor_int_create(HELLO_PACKET, &err), &err);
    
    if (err.err != CN_CBOR_NO_ERROR)
    {
        aarPrettyLog("WARNING: Error while creating HELLO payload: %i", err);
    }
    
    size_t sz = cn_cbor_encoder_write(NULL, 0, 0, cb);

    uint8_t* buffer = malloc(sz + LWS_PRE);
    size_t bufSz = cn_cbor_encoder_write(buffer, 0, sz, cb);
    struct MESSAGE nMess = { buffer, bufSz };

    connSendMessage(*host, nMess);
}

void connFreeMessage(void* msg)
{
    struct MESSAGE* _msg = msg;
    free(_msg);
}
