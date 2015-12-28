#include "FIFO.h"
#include "stdio.h"
#include "string.h"

void FIFO_Init (struct FIFO *Fifo){
 	Fifo->head = 0;
 	Fifo->tail = 0;
 	Fifo->size = 0;
}

void FIFO_Empty (struct FIFO *Fifo){
	Fifo->head = Fifo->tail;
	Fifo->size = 0;
}

int FIFO_isEmpty(struct FIFO *Fifo){
	return Fifo->size == 0;
}

int FIFO_isFull(struct FIFO *Fifo){
	printf("fifo used:%d/%d\n", Fifo->size, COMMAND_NUM);
	return Fifo->size == COMMAND_NUM;
}

int FIFO_Put (struct FIFO *Fifo, char* name, char* command)
{
	if (FIFO_isFull(Fifo)){
		return FIFO_ERROR;
	}

	strcpy((Fifo->name_buffer)[Fifo->head], name);
	strcpy((Fifo->command_buffer)[Fifo->head], command);
	Fifo->head = (Fifo->head + 1) % (COMMAND_NUM);
	Fifo->size++;
	return FIFO_OK;
}

void fifo_put_data(struct FIFO *Fifo, struct fifo_data* data)
{
	(Fifo->data)[Fifo->head] = data;
}

int FIFO_Get(struct FIFO *Fifo, char *name, char *command)
{
	if (FIFO_isEmpty(Fifo))
	{
		return FIFO_ERROR;
	}
	strcpy(name,(Fifo->name_buffer)[Fifo->tail]);
	strcpy(command, (Fifo->command_buffer)[Fifo->tail]);
	Fifo->tail = (Fifo->tail + 1) % (COMMAND_NUM);
	Fifo->size--;
	return FIFO_OK;
}

struct fifo_data* fifo_get_data(struct FIFO *Fifo)
{
	 return (Fifo->data)[Fifo->tail];
}
