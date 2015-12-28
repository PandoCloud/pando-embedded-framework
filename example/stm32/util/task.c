#include "task.h"
#include <stdio.h>

//define the task node structure in the system task scheduler
typedef struct task_node {
	task *ptask;
	struct task_node *pre;
	struct task_node *next;
}task_node;

//declare the task head in the task queue
task_node *g_task_head = NULL;

/******************************************************************************
 * FunctionName : new_task
 * Description  : new a empty task
 * Parameters   : none
 * Returns      : the created task
*******************************************************************************/
task * new_task(void)
{
	return (task *)malloc(sizeof(task));
}

/******************************************************************************
 * FunctionName : add_task
 * Description  : add a new task to the system
 * Parameters   : the task to be added
 * Returns      : the add operation result
*******************************************************************************/
int8_t add_task(task *ptask)
{
	task_node *cur_task_tail;
	task_node *new_task_node;
	new_task_node = (task_node *)malloc(sizeof(task_node));
	if(new_task_node == NULL)
	{
		return -1;
	}
	
	new_task_node->pre = NULL;
	new_task_node->next = NULL;
	new_task_node->ptask = ptask;
	if(g_task_head == NULL)
	{
		g_task_head = new_task_node;
		return 0;
	}
	
	if(g_task_head->next == NULL)
	{
		cur_task_tail = g_task_head;
	}
	else
	{
		cur_task_tail = g_task_head->next;
	}
	
	while(cur_task_tail->next != NULL)
	{
		cur_task_tail = cur_task_tail->next;
	}
	
	cur_task_tail->next = new_task_node;
	new_task_node->pre = cur_task_tail;
	new_task_node->next = NULL;
	return 0;
}

/******************************************************************************
 * FunctionName : pop_task
 * Description  : pop a task from the system
 * Parameters   : none
 * Returns      : the task
*******************************************************************************/
task* pop_task(void)
{
	task_node *cur_task_head = g_task_head;
	task_node *next_task_node;
	task *ptask;
	if(cur_task_head == NULL)
	{
		return NULL;
	}
	
	next_task_node = cur_task_head->next;
	if(next_task_node == NULL)
	{
		ptask = cur_task_head->ptask;
		g_task_head = NULL;
		free(cur_task_head);
		return ptask;
	}
	
	ptask = cur_task_head->ptask;
	g_task_head = next_task_node;
	g_task_head->pre = NULL;
	cur_task_head->next = NULL;
	free(cur_task_head);
	return ptask;
}

/******************************************************************************
 * FunctionName : delete_task
 * Description  : delete a task
 * Parameters   : the task to be deleted
 * Returns      : none
*******************************************************************************/
void delete_task(task *ptask)
{
	if(ptask == NULL)
	{
		return;
	}
	
	if(ptask->pdata != NULL)
	{
		free(ptask->pdata);
	}
	
	free(ptask);
}
