/*******************************************************
 * File name: pando_object.h
 * Author: razr
 * Versions: 1.0
 * Description: pando object interfaces
 * History:
 *   1.Date: Sep 11, 2015
 *     Author: razr
 *     Modification:    
 *********************************************************/

#ifndef PANDO_OBJECTS_H_
#define PANDO_OBJECTS_H_

#include "../platform/include/pando_types.h"
//#include "../protocol/sub_device_protocol.h"

typedef struct TLVs PARAMS;

typedef struct {
    uint8 no;
    void (*pack)(PARAMS*);
    void (*unpack)(PARAMS*);
}pando_object;

typedef struct {
    uint8 cur;
}pando_objects_iterator;

/******************************************************************************
 * FunctionName : register_pando_object.
 * Description  : register a pando object to framework.
 * Parameters   : a pando object.
 * Returns      : none.
*******************************************************************************/
void register_pando_object(pando_object object);

/******************************************************************************
 * FunctionName : find_pando_object.
 * Description  : find a pando object by object no.
 * Parameters   : the object no.
 * Returns      : the pando object of specified no, NULL if not found.
*******************************************************************************/
pando_object* find_pando_object(int8 no);

/******************************************************************************
 * FunctionName : create_pando_objects_iterator, delete_pando_objects_iterator.
 * Description  : iterator for pando object list.
 * Parameters   : .
 * Returns      : .
*******************************************************************************/
pando_objects_iterator* create_pando_objects_iterator(void);
void delete_pando_objects_iterator(pando_objects_iterator*);
pando_object* pando_objects_iterator_next(pando_objects_iterator*);

#endif /* PANDO_OBJECTS_H_ */
