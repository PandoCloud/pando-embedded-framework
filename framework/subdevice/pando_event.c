
#include "pando_event.h"
#include "../platform/include/pando_sys.h"
#include "../protocol/pando_machine.h"

#define MAX_EVENTS 16

static pd_event s_pando_event_list[MAX_EVENTS];
static int s_pando_event_list_idx = 0;

/******************************************************************************
 * FunctionName : register_pando_event.
 * Description  : register a pando event to framework.
 * Parameters   : a pando event.
 * Returns      : none.
*******************************************************************************/
void FUNCTION_ATTRIBUTE
register_pando_event(pd_event event)
{
	if(s_pando_event_list_idx > MAX_EVENTS - 1)
	{
		return;
	}

	s_pando_event_list[s_pando_event_list_idx++] = event;
}

/******************************************************************************
 * FunctionName : find_pando_event.
 * Description  : find a pando event by event no.
 * Parameters   : the event no.
 * Returns      : the pando event of specified no, NULL if not found.
*******************************************************************************/
pd_event* FUNCTION_ATTRIBUTE
find_pando_event(uint8_t no)
{
    int i;
    for(i = 0; i < s_pando_event_list_idx; i++)
    {
        if( s_pando_event_list[i].no == no)
        {
            return &s_pando_event_list[i];
        }
    }

    return NULL;
}
