#include "stm32f10x.h"
#include "usart1.h"
//#include "sys/sched.h"
//#include "business/gateway_handler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define USART1_BAUDRATE 115200


void uart_put(u8 dat)
{
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    USART_SendData(USART1,dat);
}

int fputc(int ch, FILE *f)
{
    uart_put((u8)ch);
    return ch;
}

int fgetc(FILE *f)
{
    /* Loop until received a char */
    while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));
    /* Read a character from the USART and RETURN */
    return (USART_ReceiveData(USART1));
}

void usart1_init(void)
{
	GPIO_InitTypeDef gpio_structure;
	USART_InitTypeDef usart_structure;
	NVIC_InitTypeDef nvic_structure;
	DMA_InitTypeDef dma_structure;
	
	// enable usart1, GPIOA clock.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART1);	// reset usart1
	
	// config usart1 gpio. USART1 Tx(PA.09)
	gpio_structure.GPIO_Pin = GPIO_Pin_9;
	gpio_structure.GPIO_Mode = GPIO_Mode_AF_PP; // alternate function push-pull
	gpio_structure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_structure); // init PA.09
	gpio_structure.GPIO_Pin = GPIO_Pin_10;
	gpio_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // input floating
	GPIO_Init(GPIOA, &gpio_structure); // init PA.10

	// config usart1 NVIC
	nvic_structure.NVIC_IRQChannel = USART1_IRQn;
	nvic_structure.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_structure.NVIC_IRQChannelSubPriority = 3;
	nvic_structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_structure);

	// usart config
	usart_structure.USART_BaudRate = USART1_BAUDRATE;
	usart_structure.USART_WordLength = USART_WordLength_8b;
	usart_structure.USART_StopBits = USART_StopBits_1;
	usart_structure.USART_Parity = USART_Parity_No;
	usart_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart_structure);

//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // enable idle irq

//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	DMA_DeInit(DMA1_Channel5);
//	dma_structure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
//	dma_structure.DMA_MemoryBaseAddr = (uint32_t)g_usart1_received_buf;
//	dma_structure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	dma_structure.DMA_BufferSize = USART_RECEIVED_MAX_LEN;
//	dma_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	dma_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	dma_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	dma_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	dma_structure.DMA_Mode = DMA_Mode_Circular;
//	dma_structure.DMA_Priority = DMA_Priority_VeryHigh;
//	dma_structure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel5, &dma_structure);
//	DMA_Cmd(DMA1_Channel5, ENABLE);

	// receive data with dma
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void usart1_putchar(unsigned char ch)
{
	while((USART1->SR  & 0x40) == 0); //绛夊緟鍙戦�佸畬姣�
	USART_SendData(USART1, ch);
}

/*void USART1_IRQHandler(void)
{
	uint8_t tmp;
	task_struct *task;
	uint8_t *data;
	uint16_t length = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		tmp = USART_ReceiveData(USART1);
		DMA_Cmd(DMA1_Channel5, DISABLE); // close DMA
		length = USART_RECEIVED_MAX_LEN - DMA_GetCurrDataCounter(DMA1_Channel5);
		data = (uint8_t*)malloc(length + 2); // assign two more to store the length
		data[0] = (uint8_t)(length & 0xFF);
		data[1] = (uint8_t)((length >> 8) & 0xFF);
		memcpy(&data[2], g_usart1_received_buf, length);

		task = task_create();
		task->task_handler = gateway_data_handler;
		task->pdata = data;
		append_task(task);
		
		// set data length of transmission
		DMA_SetCurrDataCounter(DMA1_Channel5, USART_RECEIVED_MAX_LEN);
		// open DMA
		DMA_Cmd(DMA1_Channel5,ENABLE);
	}
	
	if(USART_GetFlagStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);    //绂佹鍙戠紦鍐插櫒绌轰腑鏂�
	}
}*/

