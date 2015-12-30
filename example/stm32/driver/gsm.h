/*******************************************************
 * File name: gsm.h
 * Author: Chongguang Li
 * Versions: 1.0
 * Description:This module is gsm api.
 * History:
 *   1.Date: initial code
 *     Author: Chongguang Li
 *     Modification:
 *********************************************************/
#ifndef __GSM_H
#define __GSM_H

#include "platform/include/pando_types.h"

#define GSM_OFF_LINE -1
#define GSM_START 0
#define GSM_SYNC  1
#define GSM_INIT  2
#define GSM_GET_IP  3
#define GSM_INIT_DONE 4


typedef void (* module_http_callback)(char* response);
typedef void(*gsm_tcp_connected_callback)(int16_t fd, int8_t error_no);
typedef void(* gsm_tcp_sent_callback)(int16_t fd, int8_t errno);
typedef void(* gsm_tcp_recv_callback)(int16_t fd, uint8_t *data, uint16_t length);
typedef void(* gsm_tcp_disconnected_callback)(int16_t fd, int8_t errno);

/******************************************************************************
 * FunctionName : get_gsm_status
 * Description  : get the status of the gsm.
 * Parameters   : none
 * Returns      :
*******************************************************************************/
int8_t  get_gsm_status(void);

/******************************************************************************
 * FunctionName : set_gsm_status
 * Description  : set the status of the gsm.
 * Parameters   : status: the set status.
 * Returns      :
*******************************************************************************/
void set_gsm_status(int8_t status);


uint8_t gsm_system_start(void);

/******************************************************************************
 * FunctionName : add_send_at_command
 * Description  : add the at command to the send buffer.
 * Parameters   : name_buffer: the at command name
 * 				  cmd_buffer: cmd_buffer: the command.
 * Returns      : none.
*******************************************************************************/
void add_send_at_command(char *name_buffer, char *cmd_buffer);

/******************************************************************************
 * FunctionName : gsm_handler
 * Description  : the gsm response data process handler.
 * Parameters   : data : the gsm response data, include data and length. 
 * Returns      : none.
*******************************************************************************/
int8_t gsm_data_handler(void* data);

/******************************************************************************
 * FunctionName : gsm_system_init
 * Description  : initialize the gsm moudle.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
uint8_t gsm_system_init(void);

//void register_gsm_http_callback(GSM_HTTP_CALLBACK http_cb);


void register_gsm_tcp_connect_callback(uint16_t fd, gsm_tcp_disconnected_callback connect_cb);

void register_gsm_tcp_sent_callback(uint16_t fd, gsm_tcp_sent_callback sent_callback);

void register_gsm_tcp_recv_callback(uint16_t fd, gsm_tcp_recv_callback recv_callback);

void register_gsm_tcp_disconnected_callback(uint16_t fd, gsm_tcp_disconnected_callback tcp_disconnected_callback);

void module_http_post(char *url, const char* data, module_http_callback http_cb);

/******************************************************************************
 * FunctionName : gsm_send_data
 * Description  : gsm send data api.
 * Parameters   : fd: the send connect index.
 * 				  buf: the send data.
 * 				  length: the send length.
 * Returns      : none.
*******************************************************************************/
void gsm_send_data(uint16_t fd, uint8_t *buf, uint16_t len);

#endif
