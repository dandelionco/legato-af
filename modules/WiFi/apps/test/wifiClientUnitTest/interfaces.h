/**
 * This module contains function prototype for sim and mrc services.
 *
 * Copyright (C) Sierra Wireless Inc.
 *
 */

#include "le_wifiClient_interface.h"
#include "le_cfg_interface.h"
#include "le_secStore_interface.h"

#undef LE_KILL_CLIENT
#define LE_KILL_CLIENT LE_WARN

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message (STUBBED FUNCTION)
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_wifiClient_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference (STUBBED FUNCTION)
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_wifiClient_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Registers a function to be called whenever one of this service's sessions is closed by
 * the client.  (STUBBED FUNCTION)

 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionEventHandlerRef_t MyAddServiceCloseHandler
(
    le_msg_ServiceRef_t             serviceRef, ///< [IN] Reference to the service.
    le_msg_SessionEventHandler_t    handlerFunc,///< [IN] Handler function.
    void*                           contextPtr  ///< [IN] Opaque pointer value to pass to handler.
);
