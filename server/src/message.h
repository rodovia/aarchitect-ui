#pragma once

#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "protocol.h"

struct aaruserdata
{
    const char* username;
    uint8_t* avatar;
};

void aarSendMessage(struct pss_data* wsi, uint8_t* bytes);
void ParseBytes(struct pss_data* wsi, uint8_t *buffer, size_t len);

struct pss_data* aarGetConnectedClients();
struct aaruserdata* aarGetAllUsers();
struct aaruserdata aarRemoveUser(struct pss_data user);
struct aaruserdata aarGetConnectedUser(struct pss_data wsi);
void aarPutUser(struct pss_data wsi, struct aaruserdata user);
