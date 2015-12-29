/*******************************************************
 * File name: module.h
 * Author: Chongguang Li
 * Versions: 1.0
 * Description:This module is SIM5360 api.
 * History:
 *   1.Date: initial code
 *     Author: Chongguang Li
 *     Modification:
 *********************************************************/

#ifndef EXAMPLE_STM32_DRIVER_SIM5360_H_
#define EXAMPLE_STM32_DRIVER_SIM5360_H_
#include "platform/include/pando_types.h"
#include "pando_net_tcp.h"

#define MODULE_OFF_LINE -1
#define MODULE_START 0
#define MODULE_SYNC  1
#define MODULE_INIT  2
#define MODULE_GET_IP  3
#define MODULE_INIT_DONE 4

typedef void(*module_tcp_connected_callback)(int16_t fd, int8_t error_no);
typedef void(* module_tcp_sent_callback)(int16_t fd, int8_t errno);
typedef void(* module_tcp_recv_callback)(int16_t fd, uint8_t *data, uint16_t length);
typedef void(* module_tcp_disconnected_callback)(int16_t fd, int8_t errno);
typedef void(* module_http_callback)(char *buf);

void module_tcp_connect(uint16_t fd, uint32_t ip, uint16_t port);

void register_module_tcp_connect_callback(uint16_t fd, module_tcp_disconnected_callback connect_cb);

void register_module_tcp_sent_callback(uint16_t fd, module_tcp_sent_callback sent_callback);

void register_module_tcp_recv_callback(uint16_t fd, module_tcp_recv_callback recv_callback);

void register_module_tcp_disconnected_callback(uint16_t fd, module_tcp_disconnected_callback tcp_disconnected_callback);


void module_http_post(const char *url, const char* data, module_http_callback http_cb);

/******************************************************************************
 * FunctionName : module_send_data
 * Description  : module send data api.
 * Parameters   : fd: the send connect index.
 * 				  buf: the send data.
 * 				  length: the send length.
 * Returns      : none.
*******************************************************************************/
void module_send_data(uint16_t fd, uint8_t *buf, uint16_t len);

/******************************************************************************
 * FunctionName : module_system_init
 * Description  : initialize the module.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
uint8_t module_system_init(void);

/******************************************************************************
 * FunctionName : get_module_status
 * Description  : get the status of the module.
 * Parameters   : none
 * Returns      :
*******************************************************************************/
int8_t  get_module_status(void);

/******************************************************************************
 * FunctionName : set_module_status
 * Description  : set the status of the module.
 * Parameters   : status: the set status.
 * Returns      :
*******************************************************************************/
void set_module_status(int8_t status);

/******************************************************************************
 * FunctionName : inquire_signal_quality
 * Description  : inquire the signal quality.
 * Parameters   : none.
 * Returns      : the signal quality.
 // TODO: the acquired signal quality is not the current value, but the last inquired value.
*******************************************************************************/
uint8_t inquire_signal_quality(void);

uint8_t module_system_start(void);

/******************************************************************************
 * FunctionName : add_send_at_command
 * Description  : add the at command to the send buffer.
 * Parameters   : name_buffer: the at command name
 * 				  cmd_buffer: cmd_buffer: the command.
 * Returns      : none.
*******************************************************************************/
void add_send_at_command(char* name_buffer, char* cmd_buffer);

/******************************************************************************
 * FunctionName : module_handler
 * Description  : the module response data process handler.
 * Parameters   : data : the module response data, include data and length.
 * Returns      : none.
*******************************************************************************/
int8_t module_data_handler(void* data);

#endif /* EXAMPLE_STM32_DRIVER_module_H_ */
