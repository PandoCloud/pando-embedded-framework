#include "stm32f10x.h"
#include "usart2.h"
#include "usart1.h"
#include <stdio.h>
#include "task.h"
#include "platform/include/pando_types.h"
#include "sim5360.h"
#include <stdlib.h>
#include <string.h>
#include "malloc.h"


#define USART_RECEIVED_MAX_LEN 1000

uint8_t g_usart2_received_buf[USART_RECEIVED_MAX_LEN];

struct uart_buffer{
	uint8_t * buf;
	uint16_t length;
};

void usart2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef dma_structure;

	//Enable clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);

	USART_DeInit(USART2);

	//config IO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	             //USART2 TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;			 //GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //
	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 //USART2 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USARTx */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable USARTx Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel6);
	dma_structure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
	dma_structure.DMA_MemoryBaseAddr = (uint32_t)g_usart2_received_buf;
	dma_structure.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma_structure.DMA_BufferSize = USART_RECEIVED_MAX_LEN;
	dma_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_structure.DMA_Mode = DMA_Mode_Circular;
	dma_structure.DMA_Priority = DMA_Priority_VeryHigh;
	dma_structure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &dma_structure);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

	/* Enable the USARTx */
	USART_Cmd(USART2, ENABLE);	
}


void usart2_putchar(unsigned char ch)
{
	USART2->SR;
	USART_SendData(USART2, ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)!= SET);
}

uint16_t usart2_send(uint8_t *buffer, uint16_t length)
{
	printf("%s11\n", __func__);
    uint16_t tx_index;
    for(tx_index = 0; tx_index < length; tx_index++)
    {
        usart2_putchar(buffer[tx_index]);
    }
    printf("%s12\n",  __func__);
    return tx_index;
}



/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval : None
  */

void USART2_IRQHandler(void)      //����2 �жϷ������
{
	uint8_t tmp;
	task * uart2_task;
	struct uart_buffer *uart2_buf;
	uint16_t length = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		tmp = USART_ReceiveData(USART2);
		DMA_Cmd(DMA1_Channel6, DISABLE); // close DMA
		length = USART_RECEIVED_MAX_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
		//printf("uart2 dma2 length:%d\n", length);
		uart2_buf = (struct uart_buffer*)malloc(sizeof(struct uart_buffer));
		uart2_buf->buf = (uint8_t*)malloc(length + 1);
		uart2_buf->length = length + 1;
		memcpy(uart2_buf->buf, g_usart2_received_buf, length);
		uart2_buf->buf[length] = 0;

		uart2_task = new_task();
		uart2_task->handler = module_data_handler;
		uart2_task->pdata = uart2_buf;
		add_task(uart2_task);

		// set data length of transmission
		DMA_SetCurrDataCounter(DMA1_Channel6, USART_RECEIVED_MAX_LEN);
		// open DMA
		DMA_Cmd(DMA1_Channel6,ENABLE);
	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG
	{
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     //��ֹ�����������жϣ�
	}

}
