#ifndef TINET_H
#define TINET_H

#include <stdbool.h>
#include <srldrvce.h>
#include <string.h>
#include <fileioc.h>
#include <ti/info.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <tice.h>

#ifdef __cplusplus
extern "C" {
#endif

enum TINETInitStatus
{
    TINET_INIT_SUCCESS,
    TINET_INIT_FAIL
};

enum TINETLoginStatus
{
    TINET_LOGIN_SUCCESS,
    TINET_LOGIN_FAILURE,
    TINET_LOGIN_NO_KEYFILE,
    TINET_LOGIN_NO_BRIDGE,
    TINET_LOGIN_SERIAL_FAIL
};

/* FUNCTIONS */

usb_error_t tinet_handle_usb_event(usb_event_t event, void *event_data, usb_callback_data_t *callback_data);

/**
 * Inits the TINET serial functions
 * This return true if initialization was successful
 * @param srl SRL device structure.
 * @return enum, INIT_SUCCESS or INIT_FAIL
 */
enum TINETInitStatus tinet_init();

size_t tinet_write_serial(const char *message);

/**
 * Handle TINET serial data
 * Returns the input buffer
 * @param srl SRL device structure.
 * @return boolean, true on success and false on fail.
 */
char tinet_handle_srl();

enum TINETLoginStatus tinet_login();

bool tinetIsBridgeConnected();
bool tinetIsInternetConnected();
char tinetGetUsername();

#ifdef __cplusplus
}
#endif

#endif