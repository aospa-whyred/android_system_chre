/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CHRE_PAL_WIFI_H
#define CHRE_PAL_WIFI_H

/**
 * @file
 * Defines the interface between the common CHRE core system and the
 * platform-specific WiFi module.
 */

#include "chre_api/chre/common.h"
#include "chre_api/chre/wifi.h"
#include "chre/pal/version.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initial version of the CHRE WiFi PAL, tied to CHRE API v1.1.
 */
#define CHRE_PAL_WIFI_API_V1_0  CHRE_PAL_CREATE_API_VERSION(1, 0)

/**
 * The version of the WiFi GNSS PAL defined in this header file.
 */
#define CHRE_PAL_WIFI_API_CURRENT_VERSION  CHRE_PAL_WIFI_API_V1_0

struct chrePalWifiCallbacks {
    /**
     * Callback invoked to inform the CHRE of the result of changes to the scan
     * monitor registration status requested via configureScanMonitor in struct
     * chrePalWifiApi.
     *
     * Unsolicited calls to this function must not be made. In other words,
     * this callback should only be invoked as the direct result of an earlier
     * call to configureScanMonitor. If the scan monitor registration is lost,
     * for example due to a reset of the WiFi subsystem, then the PAL
     * implementation is required to silently re-register the scan monitor when
     * it recovers, as needed.
     *
     * @param enabled true if the scan monitor is currently active and
     *        scanEventCallback will receive unsolicited scan results, false
     *        otherwise
     * @param errorCode An error code from enum chreError
     *
     * @see #configureScanMonitor
     * @see #chreError
     */
    void (*scanMonitorStatusChangeCallback)(bool enabled, uint8_t errorCode);

    /**
     * Callback invoked to inform the CHRE of the result of a request for a
     * scan requested via requestScan in struct chrePalWifiApi.
     *
     * Unsolicited calls to this function must not be made. See
     * scanMonitorStatusChangeCallback() for more information.
     *
     * @param pending true if the request was successful and the scan is
     *        scheduled (or cached results are pending delivery), false
     *        otherwise
     * @param errorCode An error code from enum chreError
     */
    void (*scanResponseCallback)(bool pending, uint8_t errorCode);

    /**
     * Callback used to pass scan results from the WiFi module to the core CHRE
     * system, which distributes it to clients (nanoapps).
     *
     * This function call passes ownership of the event memory to the core CHRE
     * system, i.e. the PAL module must not modify the referenced data until the
     * associated API function is called to release the memory.
     *
     * @param event Event data to distribute to clients. The WiFi module
     *        must ensure that this memory remains accessible until it is passed
     *        to the releaseScanEvent() function in struct chrePalWifiApi.
     */
    void (*scanEventCallback)(struct chreWifiScanEvent *event);
};

struct chrePalWifiApi {
    /**
     * Version of the module providing this API. This value should be
     * constructed from CHRE_PAL_CREATE_MODULE_VERSION using the supported
     * API version constant (CHRE_PAL_WIFI_API_*) and the module-specific patch
     * version.
     */
    uint32_t moduleVersion;

    /**
     * Initializes the WiFi module. Initialization must complete synchronously.
     *
     * @param callbacks Structure containing entry points to the core CHRE
     *        system. The module does not need to deep-copy this structure; its
     *        memory remains accessible at least until after close() is called.
     *
     * @return true if initialization was successful, false otherwise
     */
    bool (*open)(const struct chrePalWifiCallbacks *callbacks);

    /**
     * Performs clean shutdown of the WiFi module, usually done in preparation
     * for stopping the CHRE. The WiFi module must ensure that it will not
     * invoke any callbacks past this point, and complete any relevant teardown
     * activities before returning from this function.
     */
    void (*close)(void);

    //! @see chreWifiGetCapabilities()
    uint32_t (*getCapabilities)(void);

    /**
     * Configures whether the scanEventCallback receives unsolicited scan
     * results, i.e. the results of scans not performed at the request of CHRE.
     *
     * @param enable true to enable listening for all available scan results
     *
     * @return true if the request was accepted for processing, in which case a
     *         subsequent call to scanMonitorStatusChangeCallback will be used
     *         to communicate the result of the operation
     *
     * @see chreWifiConfigureScanMonitorAsync()
     */
    bool (*configureScanMonitor)(bool enable);

    /**
     * Request that the WiFi chipset perform a scan, or deliver results from its
     * cache if the parameters allow for it. If this function returns true, then
     * the scanResponseCallback will be invoked to provide the result of
     * requesting the scan. If that indicates a successful result (the scan is
     * pending), then scanEventCallback() will be invoked one more more times to
     * deliver the results of the scan. The results for the requested scan are
     * delivered in scanEventCallback() regardless of the most recent setting
     * passed to configureScanMonitor().
     *
     * @param params See chreWifiRequestScanAsync()
     *
     * @return true if the request was accepted for further processing, in which
     *         case its result will be indicated via a call to the scan monitor
     *         status change callback.
     *
     * @see #chreWifiScanParams
     * @see chreWifiRequestScanAsync()
     */
    bool (*requestScan)(const struct chreWifiScanParams *params);

    /**
     * Invoked when the core CHRE system no longer needs a WiFi scan event
     * structure that was provided to it via scanEventCallback()
     *
     * @param event Event data to release
     */
    void (*releaseScanEvent)(struct chreWifiScanEvent *event);
};

/**
 * Retrieve a handle for the CHRE WiFi PAL.
 *
 * @param requestedApiVersion The implementation of this function must return a
 *        pointer to a structure with the same major version as requested.
 *
 * @return Pointer to API handle, or NULL if a compatible API version is not
 *         supported by the module, or the API as a whole is not implemented. If
 *         non-NULL, the returned API handle must be valid as long as this
 *         module is loaded.
 */
const struct chrePalWifiApi *chrePalWifiGetApi(uint32_t requestedApiVersion);

#ifdef __cplusplus
}
#endif

#endif  // CHRE_PAL_WIFI_H
