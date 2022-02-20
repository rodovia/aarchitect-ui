#pragma once

#define MSG_AWAKE           0xbee /* Used to force the connection thread
                                    to create the thread mutex. */
#define MSG_SEND_CHMESSAGE  0x100 /* Send chat message */
#define MSG_RM_USER         0x101 /* Remove user */

typedef struct ASYNCMESSAGEINFO
{
    int type;
    union
    {
        char* contenttbs; /* content to be sent */
    };
} aarAsyncMessageInfo;

void aarSendMessageThr(int type);
