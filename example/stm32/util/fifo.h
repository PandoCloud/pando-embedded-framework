/*******************************************************
 * File name: fifo.h
 * Author: Xiaoxiao Li
 * Versions:0.0.1
 * Description: the fifo api
 *********************************************************/

#ifndef FIFO_H_
#define FIFO_H_

#include "stdint.h"

#define COMMAND_NUM  10
#define NAME_WIDTH 48
#define COMMAND_WIDTH 96

#define FIFO_OK 1
#define FIFO_ERROR -1

struct fifo_data{
	uint16_t length;
	uint8_t* buf;
};

typedef struct FIFO {
	char name_buffer[COMMAND_NUM][NAME_WIDTH];
	char command_buffer[COMMAND_NUM][COMMAND_WIDTH];
	struct fifo_data* data[COMMAND_NUM];
	unsigned int head;
	unsigned int tail;
	unsigned int size;
}FIFO;

int FIFO_isEmpty (struct FIFO *Fifo);
void FIFO_Init (struct FIFO *Fifo);
void FIFO_Empty (struct FIFO *Fifo);
int FIFO_Put (struct FIFO *Fifo, char* name, char* command);
int FIFO_Get (struct FIFO *Fifo, char* name, char* command);
void fifo_put_data(struct FIFO *Fifo, struct fifo_data* data);
struct fifo_data* fifo_get_data(struct FIFO *Fifo);

#endif
