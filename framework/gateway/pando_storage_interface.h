/*******************************************************
 * File name: pando_storage_interface.h
 * Author:
 * Versions:1.0
 * Description:This module is the interface used to store the gateway data.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
#ifndef __PANDO_STORAGE_INTERFACE_H__
#define __PANDO_STORAGE_INTERFACE_H__

#include "platform/include/pando_types.h"

typedef enum {
    CHANGE_SAVE_OK = 0,
    CREAT_SAVE_OK,
    FULL_ERROR,
    SAVE_ERROR,
} SET_RESULT;
 
/******************************************************************************
 * FunctionName : pando_data_set
 * Description  : set the vale of the parameter stored, if the parameter is existing.
                  Else creat the parameter, and save it.
 * Parameters   : key -- the parameter;
                  value -- the value of the parameter. 
 * Returns      : the save result
*******************************************************************************/
SET_RESULT pando_data_set(char* key, char* value);

/******************************************************************************
 * FunctionName : pando_data_get
 * Description  : get the value of the key.
 * Parameters   : key -- the parameter
 * Returns      : the pointer of the value. NULL if not exist
*******************************************************************************/
char * pando_data_get(char* key);

/******************************************************************************
 * FunctionName : pando_storage_space_left
 * Description  : get the space left for pando data saved in the storage.
 * Parameters   : 
 * Returns      : the space left for pando data saving.
*******************************************************************************/
uint16 pando_storage_space_left(void);

/******************************************************************************
 * FunctionName : pando_storage_clean.
 * Description  : clean the pando configuration message.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pando_storage_clean(void);

#endif
