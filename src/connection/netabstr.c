#include <aarchitect.h>
#include <aarchitect/messagepass.h>
#include <apiutils.h>
#include "connection.h"
#include <string.h>

void aarSendChatMessage(const char* content)
{
    struct lws_context* ctx = GetWebsocketContext();
    struct lws_vhost* vhs = lws_get_vhost_by_name(ctx, "default");     
    struct lws_protocols* aarproto = lws_vhost_name_to_protocol(vhs, "aarchitect-chat");

    struct PERVHOSTDATA__MINIMAL* vhd = (struct PERVHOSTDATA__MINIMAL*) 
                                    lws_protocol_vh_priv_get(vhs, aarproto);
    
    connSendMessageCreatePayload(vhd, content);
}

DEPRECATE(
void aarSendMessageThr(int type)
)
{
    struct ASYNCMESSAGEINFO* mInfo = malloc(sizeof(struct ASYNCMESSAGEINFO));
    memset(mInfo, 0, sizeof(struct ASYNCMESSAGEINFO));
    mInfo->type = type;

    GetAsyncHandle()->data = mInfo;

    uv_mutex_t* mut = GetAsyncMutex();
    uv_async_send(GetAsyncHandle());
}
