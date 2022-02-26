#include <aarchitect.h>
#include <stdlib.h>
#include <malloc.h>
#include <mpack.h>
#include <connection_opcodes.h>

#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "connection.h"

void connSendMessage(struct PERVHOSTDATA__MINIMAL host, struct MESSAGE msg)
{
    char* newCnt = (char*) malloc((sizeof(char) * msg.len) + LWS_PRE);
    memset(newCnt, 0, (sizeof(char) * msg.len) + LWS_PRE);
    newCnt[LWS_PRE - 1] = msg.payload;

    struct MESSAGE newMsg = { newCnt, msg.len + LWS_PRE };

    lws_ring_insert(host.ring, &newMsg, 1);
    lws_callback_on_writable(host.client_wsi);
}

void connSendHelloPayload(struct PERVHOSTDATA__MINIMAL* host, 
                        aarUserConfiguration* lpuCfg)
{
    char* data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);

    mpack_build_map(&writer);
    mpack_write_cstr(&writer, "type");
        mpack_write_int(&writer, HELLO_PACKET);

    mpack_write_cstr(&writer, "username");
        mpack_write_cstr(&writer, lpuCfg->username);
    mpack_complete_map(&writer);

    if (mpack_writer_destroy(&writer) != mpack_ok)
    {
        lwsl_warn("Error while building HELLO packet");
        return;
    }

    struct MESSAGE msg = { data, size };
    connSendMessage(*host, msg);
}

void connSendMessageCreatePayload(struct PERVHOSTDATA__MINIMAL* host,
                                    const char* content)
{
    char* data;
    size_t size;
    mpack_writer_t writer;
    mpack_writer_init_growable(&writer, &data, &size);

    mpack_build_map(&writer);
    mpack_write_cstr(&writer, "type");
        mpack_write_int(&writer, EVENT_PACKET);
    
    mpack_write_cstr(&writer, "event");
        mpack_write_cstr(&writer, MESSAGE_CREATE);
    
    mpack_write_cstr(&writer, "content");
        mpack_write_cstr(&writer, content);
    mpack_complete_map(&writer);
    if (mpack_writer_destroy(&writer) != mpack_ok)
    {
        lwsl_warn("Error while building MESSAGE_CREATE packet");
        return;
    }

    struct MESSAGE msg = { data, size };
    connSendMessage(*host, msg);
}
