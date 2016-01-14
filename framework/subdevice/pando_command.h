/*******************************************************
 * File name: pando_command.h
 * Author: wfq
 * Versions: 1.0
 * Description: pando object interfaces
 * History:
 *   1.Date: Jan 14, 2016
 *     Author: wfq
 *     create:    
 *********************************************************/

#ifndef PANDO_COMMAND_H_
#define PANDO_COMMAND_H_

#include "../platform/include/pando_types.h"
//#include "../protocol/sub_device_protocol.h"

typedef struct TLVs PARAMS;

typedef struct {
    uint8_t no;
    uint8_t priority;
    void (*unpack)(PARAMS*);
}pando_command;

typedef struct {
	uint8_t cur;
}pando_commands_iterator;

/******************************************************************************
 * FunctionName : register_pando_command.
 * Description  : register a pando command to framework.
 * Parameters   : a pando command.
 * Returns      : none.
*******************************************************************************/
void register_pando_command(pando_command cmd);

/******************************************************************************
 * FunctionName : find_pando_command.
 * Description  : find a pando command by command no.
 * Parameters   : the command no.
 * Returns      : the pando command of specified no, NULL if not found.
*******************************************************************************/
pando_command* find_pando_command(uint8_t no);

/******************************************************************************
 * FunctionName : create_pando_objects_iterator, delete_pando_objects_iterator.
 * Description  : iterator for pando object list.
 * Parameters   : .
 * Returns      : .
*******************************************************************************/
//pando_commands_iterator* create_pando_commands_iterator(void);
//void delete_pando_commands_iterator(pando_commands_iterator*);
//pando_command* pando_commands_iterator_next(pando_commands_iterator*);

#endif /* PANDO_OBJECTS_H_ */




