/*******************************************************
 * File name: pando_event.h
 * Author: wfq
 * Versions: 1.0
 * Description: pando object interfaces
 * History:
 *   1.Date: Jan 14, 2016
 *     Author: wfq
 *     Create:    
 *********************************************************/

#ifndef PANDO_EVENT_H_
#define PANDO_EVENT_H_

#include "../platform/include/pando_types.h"
//#include "../protocol/sub_device_protocol.h"

typedef struct TLVs PARAMS;

typedef struct {
    uint8_t no;
    uint8_t priority;
    void (*unpack)(PARAMS*);
}pando_event;

typedef struct {
	uint8_t cur;
}pando_events_iterator;

/******************************************************************************
 * FunctionName : register_pando_event.
 * Description  : register a pando event to framework.
 * Parameters   : a pando event.
 * Returns      : none.
*******************************************************************************/
void register_pando_event(pando_event event);

/******************************************************************************
 * FunctionName : find_pando_event.
 * Description  : find a pando event by event no.
 * Parameters   : the event no.
 * Returns      : the pando event of specified no, NULL if not found.
*******************************************************************************/
pando_event* find_pando_event(uint8_t no);

/******************************************************************************
 * FunctionName : create_pando_events_iterator, delete_pando_events_iterator.
 * Description  : iterator for pando event list.
 * Parameters   : .
 * Returns      : .
*******************************************************************************/
//pando_events_iterator* create_pando_events_iterator(void);
//void delete_pando_events_iterator(pando_events_iterator*);
//pando_event* pando_events_iterator_next(pando_events_iterator*);

#endif /* PANDO_OBJECTS_H_ */
