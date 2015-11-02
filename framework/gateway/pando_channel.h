/*******************************************************
 * File name: pando_channel.h
 * Author: ruizeng
 * Versions: 1.0
 * Description:This module is to used to simulate connection channel (eg. serial port) between device and subdevice.
 * History:
 *   1.Date: initial code
 *     Author: ruizeng
 *     Modification:    
 *********************************************************/
 
#ifndef __PANDO_CHANNEL_H__
#define __PANDO_CHANNEL_H__

#include "pando_types.h"

typedef enum {
    PANDO_CHANNEL_PORT_0 = 0,
    PANDO_CHANNEL_PORT_1,
    PANDO_CHANNEL_PORT_2,
    PANDO_CHANNEL_PORT_3,
    PANDO_CHANNEL_PORT_4,
    PANDO_CHANNEL_PORT_5,
    PANDO_CHANNEL_PORT_6,
    PANDO_CHANNEL_PORT_7
} PANDO_CHANNEL_NAME;

/*
 * "channel_recv_callback" is a callback function invoked when recving buffer from some channel.
 */
typedef void (* channel_recv_callback)(uint8_t * buffer, uint16_t length);

 /******************************************************************************
 * FunctionName : on_subdevice_channel_recv
 * Description  : regiseter the callback function when subdevice received buffer from some channel.
 * Parameters   : name: channel name
 *                cb: callback
 * Returns      : 
*******************************************************************************/
void on_subdevice_channel_recv(PANDO_CHANNEL_NAME name, channel_recv_callback cb);

 /******************************************************************************
 * FunctionName : on_device_channel_recv
 * Description  : regiseter the callback function when device received buffer from some channel.
 * Parameters   : name: channel name
 *                cb: callback
 * Returns      : 
*******************************************************************************/
void on_device_channel_recv(PANDO_CHANNEL_NAME name, channel_recv_callback cb);

 /******************************************************************************
 * FunctionName : subdevice_channel_send
 * Description  : send data to subdevice.
 * Parameters   : name: channel name
 * Returns      : 
*******************************************************************************/
void channel_send_to_subdevice(PANDO_CHANNEL_NAME name, uint8_t * buffer, uint16_t length);

 /******************************************************************************
 * FunctionName : device_channel_send
 * Description  : send data to device.
 * Parameters   : name: channel name
 * Returns      : 
*******************************************************************************/
void channel_send_to_device(PANDO_CHANNEL_NAME name, uint8_t * buffer, uint16_t length);

#endif
