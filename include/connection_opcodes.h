#pragma once

/* packet operation codes */

/* Sent by client */
#define HELLO_PACKET     0x1a00
#define USER_UPDT_PACKET 0x1a01 /* User update packet */
#define REFTCH_ALL_MEMB  0x1a02 /* Refetch all members */

/* Sent by server */
#define EVENT_PACKET     0x3bf3 /* EVENT packet (sent when a event happens) */
#define DISCNT_PACKET    0x3bf4 /* Sent when the session got invalidated */

/* Events */
#define USER_ADD        "USER_ADD"
#define USER_REMOVE     "USER_REMOVE"
#define MESSAGE_CREATE  "MESSAGE_CREATE"
