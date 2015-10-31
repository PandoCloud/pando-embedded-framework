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

#include "platform/include/c_types.h"

#define MAX_CHAN_LEN 8

//define the communication medium between device and subdevice.
typedef enum {
    CHANNEL_MEMORY = 0,
    CHANNEL_UART,
} PANDO_CHANNEL_TYPE;

/*
 * "channel_recv_callback" is a callback function invoked when recving buffer from some channel.
 */
typedef void (* channel_recv_callback)(uint8_t * buffer, uint16_t length);

/******************************************************************************
 * FunctionName : channel_init
 * Description  : init different channel settings with channel_type, device_receive_cb, subdevice_receive_cb.
 * Parameters   : channel_count: the channel count to set. should be less than MAX_CHAN_LEN
 *                ch1_type: channel1 channel type.
 *                ch1_dev_re_cb: channel1 device receive callback.
 *                ch1_subdev_re_cb: channel1 subdevice receive callback.
 *                :"...": other channel settings, should be the same order with channel1.
 * Returns      : 
*******************************************************************************/
void channel_init(uint8_t channel_count, PANDO_CHANNEL_TYPE ch1_type, channel_recv_callback ch1_dev_re_cb, channel_recv_callback ch1_subdev_re_cb, ...);

 /******************************************************************************
 * FunctionName : channel_send_to_subdevice
 * Description  : send data to subdevice.
 * Parameters   : channel_no: channel number
 * Returns      : 
*******************************************************************************/
void channel_send_to_subdevice(uint8_t channel_no, uint8_t * buffer, uint16_t length);

 /******************************************************************************
 * FunctionName : channel_send_to_device
 * Description  : send data to device.
 * Parameters   : channel_no: channel number
 * Returns      : 
*******************************************************************************/
void channel_send_to_device(uint8_t channel_no, uint8_t * buffer, uint16_t length);

#endif
