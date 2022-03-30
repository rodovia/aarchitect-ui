#pragma once

#ifndef _AAR_API
#   include "api.h"
#endif
#include "bits/aarbase.h"

/** 
 * @file aarchitect_connection.h
 * @brief Funnctions related to connection
 * 
 * This file should not be included directly, 
 * as it requires structs only available in `aarchitect.h`.
 */

#include <uv.h>

/**
 * @brief Creates a thread and connects to the server.
 * 
 * The created thread will be returned.
 * 
*/
uv_thread_t aarConnectClient(
    aarUserConfiguration* lpuCfg
);

/**
 * @brief Creates a connection to the server in the current thread
 * 
 * The created connection will block the current thread and only returns when
 * the connection ends
 **/
int aarConnectClientBlocking(
    aarUserConfiguration* lpuCfg /** < [in] the configuration */
);

