#include <aarchitect.h>
#include <stdlib.h>
#include <malloc.h>
#include <mpack.h>
#include <connection_opcodes.h>

#include <libwebsockets.h>
#include "connection.h"

void _ZeroMemoryPadded(char* ptr, ptrdiff_t dfStart, ptrdiff_t dfEnd)
{
    for (int i = dfStart; i < dfEnd - 1; i++)
    {
        ptr[i] = 0;
    }
}

void connSendMessage(struct PERVHOSTDATA__MINIMAL host, struct MESSAGE msg)
{
    char* newCnt = (char*) calloc(strlen(msg.payload) + LWS_PRE, sizeof(char));
    (newCnt) = msg.payload;
    _ZeroMemoryPadded(newCnt, strlen(msg.payload), strlen(newCnt));

    struct MESSAGE newMsg = { newCnt, msg.len + LWS_PRE };

    for (ext::CThreadedPlugin& plugin : aarGetAllPlugins())
    {
        plugin.TriggerHook("RawMessageSend");
    }

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
