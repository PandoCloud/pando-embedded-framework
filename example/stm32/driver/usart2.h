#ifndef _USART2_H
#define _USART2_H

#include "platform/include/pando_types.h"

typedef void (* uart2_recv_callback)(uint8_t *buffer, uint16_t length);

void usart2_init(void);
void usart2_reset_tx_rx(void);
void usart2_putchar(unsigned char ch);
uint16_t usart2_send(uint8_t *buffer, uint16_t length);
/******************************************************************************
* FunctionName : usart2_register_receive_cb
* Description  : register the callback function when uart2 receive data.
* Parameters   : recv_cb: the callback function specify when receive data.
* Returns      : none.
*******************************************************************************/
void usart2_register_receive_cb(uart2_recv_callback  recv);


#endif
