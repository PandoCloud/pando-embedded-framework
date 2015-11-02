#ifndef _USART1_H_
#define _USART1_H_

#include "pando_types.h"

/******************************************************************************
 * FunctionName : usart1_send
 * Description  : send data via usart1
 * Parameters   : buffer: the data buffer to send
 *                length: the data length to send
 * Returns      : the data length that be sent successfully
*******************************************************************************/
uint16_t usart1_send(uint8_t *buffer, uint16_t length);

/******************************************************************************
 * FunctionName : on_usart1_recv_data_callback
 * Description  : register the handler callback when usart1 receiving data
 * Parameters   : data_recv_cb: the callback handler
 * Returns      : 
*******************************************************************************/
void on_usart1_recv_data_callback(data_handler_callback data_recv_cb);

#endif /* _USART1_H_ */
