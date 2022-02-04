#include <aarchitect.h>
#include <stdlib.h>
#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "connection.h"

void connSendMessage(struct PERVHOSTDATA__MINIMAL host, struct MESSAGE msg)
{
    struct MESSAGE nMessage = calloc(1, sizeof(struct MESSAGE));
    unsigned char* rawBuffer = calloc(1, sizeof(msg.payload) + LWS_PRE);
    rawBuffer[LWS_PRE] = rawBuffer;

    nMessage.payload = rawBuffer;
    nMessage.len = sizeof(rawBuffer);

    lws_ring_insert(host.ring, msg, 1);
    lws_callback_on_writable(host.clientwsi);
    connFreeMessage(nMessage);
}

void connFreeMessage(void* msg)
{
    struct MESSAGE* _msg = msg;
    free(_msg->payload);
    _msg->payload = NULL;
    _msg->len = 0;
}
