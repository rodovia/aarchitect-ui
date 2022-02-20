#include <aarchitect.h>
#include <aarchitect/messagepass.h>
#include "connection.h"
#include <string.h>

void aarSendChatMessage(const char* content)
{
    struct ASYNCMESSAGEINFO* mInfo = malloc(sizeof(struct ASYNCMESSAGEINFO));
    memset(mInfo, 0, sizeof(struct ASYNCMESSAGEINFO));
    mInfo->type = MSG_SEND_CHMESSAGE;
    mInfo->contenttbs = content;

    /* BUG: We need to create a race condition here. */
    GetAsyncHandle()->data = mInfo;

    uv_mutex_t* mut = GetAsyncMutex();
    uv_mutex_lock(mut);
    int status = uv_async_send(GetAsyncHandle());
    printf("\n\n*********** status: %i\n\n", status);
    uv_mutex_unlock(mut);
}

void aarSendMessageThr(int type)
{
    struct ASYNCMESSAGEINFO* mInfo = malloc(sizeof(struct ASYNCMESSAGEINFO));
    memset(mInfo, 0, sizeof(struct ASYNCMESSAGEINFO));
    mInfo->type = type;

    GetAsyncHandle()->data = mInfo;

    uv_mutex_t* mut = GetAsyncMutex();
    uv_async_send(GetAsyncHandle());
}
