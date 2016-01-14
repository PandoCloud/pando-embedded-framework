
#include "stm32f10x.h"
#include "usart1.h"
#include <stdio.h>
#include "task.h"
#include "sim5360.h"
#include <stdlib.h>
#include <string.h>

#include "uart3.h"

#define UART3_BAUDRATE 9600
#define XLT_LPR_485_DAT_LENGTH      38
uint8_t g_usart3_received_buf[40];    //store data from xlt-lpr: 485<->232 protocol (v0.4)
struct uart_lpr_buffer{
	uint8_t * buf;	
    uint16_t length;
};

void uart3_init(int baud)
{
	GPIO_InitTypeDef gpio_structure;
	USART_InitTypeDef usart_structure;
	NVIC_InitTypeDef nvic_structure;
	
	// enable uart3, GPIOC clock.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// config uart3 gpio(PB.10)
	gpio_structure.GPIO_Pin = GPIO_Pin_10;
	gpio_structure.GPIO_Mode = GPIO_Mode_AF_PP; // alternate function push-pull
	gpio_structure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_structure);
	gpio_structure.GPIO_Pin = GPIO_Pin_11;
	gpio_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // input floating
	GPIO_Init(GPIOB, &gpio_structure); // init PB.11

	// config uart3 NVIC
	nvic_structure.NVIC_IRQChannel = USART3_IRQn;
	nvic_structure.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_structure.NVIC_IRQChannelSubPriority = 2;
	nvic_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_structure);

	// uart config
	usart_structure.USART_BaudRate = baud;
	usart_structure.USART_WordLength = USART_WordLength_8b;
	usart_structure.USART_StopBits = USART_StopBits_1;
	usart_structure.USART_Parity = USART_Parity_No;
	usart_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &usart_structure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
}

void usart3_putchar(unsigned char ch)
{
	USART_SendData(USART3, ch);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC)!= SET);
}

uint16_t usart3_send(uint8_t *buffer, uint16_t length)
{
    uint16_t tx_index;
    for(tx_index = 0; tx_index < length; tx_index++)
    {
        usart3_putchar(buffer[tx_index]);
    }
    return tx_index;
}

/**
  * @brief  This function handles UART4 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART3_IRQHandler(void)
{
	uint8_t res;
    task * uart3_task;
    static uint8_t index = 0;
    struct uart_lpr_buffer *lpr_buf;
    uint8_t length = 0;
    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		res = USART_ReceiveData(USART3); //(USART3->DR);	//读取接收到的数据
        
        if(index == 0)
        {
            if(res == 0xBB)
            {
                g_usart3_received_buf[index] = res;
                index++;
            }
        }
        else if(index == 1) 
        {
            if(res == 0xAA)
            {
                g_usart3_received_buf[index] = res;
                index++;
            }
            else
            {
                memset(g_usart3_received_buf, 0, 40);
                index = 0;
            }
        }
        else
        {
            if(index < XLT_LPR_485_DAT_LENGTH)
            {
                g_usart3_received_buf[index++] = res;
            }
            if(index == XLT_LPR_485_DAT_LENGTH)   //receive ok, will create task.
            {
                lpr_buf = (struct uart_lpr_buffer*)malloc(sizeof(struct uart_lpr_buffer));
                lpr_buf->buf = (uint8_t*)malloc(index + 1);
                lpr_buf->length = index + 1;
                memcpy(lpr_buf->buf, g_usart3_received_buf, length);
                lpr_buf->buf[length] = 0;
                
                uart3_task = new_task();
                uart3_task->handler = module_data_handler;
                uart3_task->pdata = g_usart3_received_buf;
                add_task(uart3_task);
                
                memset(g_usart3_received_buf, 0, 40);
                index = 0;
            }
        }
		
		usart1_putchar(res);
	} 
		
	//这段是为了避免STM32 USART 第一个字节发不出去的BUG 
	if(USART_GetFlagStatus(USART3, USART_IT_TXE) != RESET)
	{
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);    //禁止发缓冲器空中断， 
	}
} 

