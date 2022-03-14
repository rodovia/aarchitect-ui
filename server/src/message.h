#pragma once

#define LWS_WITH_NETWORK
#include <libwebsockets.h>
#include "protocol.h"

struct aaruserdata
{
    const char* username;
    uint8_t* avatar;
};

#ifdef __cplusplus
extern "C" {
#endif

void aarSendMessage(struct pss_data* wsi, const char* bytes);
void ParseBytes(struct pss_data* wsi, const char *buffer, size_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif

struct pss_data* aarGetConnectedClients(size_t* lpSize);
struct aaruserdata* aarGetAllUsers();
struct aaruserdata aarRemoveUser(struct pss_data user);
struct aaruserdata aarGetConnectedUser(struct pss_data wsi);
void aarPutUser(struct pss_data wsi, struct aaruserdata user);
