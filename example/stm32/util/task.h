/*******************************************************
 * File name: task.h
 * Author: Xiaoxiao Li
 * Versions:0.0.1
 * Description: the task api
 *********************************************************/

#ifndef TASK_H_
#define TASK_H_

#include "platform/include/pando_types.h"

//define the task scheduler base type
typedef struct task {
	int8_t (*handler)(void *pdata);
	void *pdata;
}task;

/******************************************************************************
 * FunctionName : new_task
 * Description  : new a empty task
 * Parameters   : none
 * Returns      : the created task
*******************************************************************************/
task* new_task(void);

/******************************************************************************
 * FunctionName : add_task
 * Description  : add a new task to the system
 * Parameters   : the task to be added
 * Returns      : the add operation result
*******************************************************************************/
int8_t add_task(task *ptask);

/******************************************************************************
 * FunctionName : pop_task
 * Description  : pop a task from the system
 * Parameters   : none
 * Returns      : the task
*******************************************************************************/
task* pop_task(void);

/******************************************************************************
 * FunctionName : delete_task
 * Description  : delete a task
 * Parameters   : the task to be deleted
 * Returns      : none
*******************************************************************************/
void delete_task(task *ptask);

#endif
