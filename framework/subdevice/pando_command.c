
#include "pando_command.h"
#include "../platform/include/pando_sys.h"
#include "../protocol/pando_machine.h"

#define MAX_COMMANDS 16

static pd_command s_pando_command_list[MAX_COMMANDS];
static int s_pando_command_list_idx = 0;

/******************************************************************************
 * FunctionName : register_pando_command.
 * Description  : register a pando command to framework.
 * Parameters   : a pando command.
 * Returns      : none.
*******************************************************************************/
void FUNCTION_ATTRIBUTE
register_pando_command(pd_command cmd)
{
	if(s_pando_command_list_idx > MAX_COMMANDS - 1)
	{
		return;
	}

	s_pando_command_list[s_pando_command_list_idx++] = cmd;
}

/******************************************************************************
 * FunctionName : find_pando_command.
 * Description  : find a pando command by command no.
 * Parameters   : the command no.
 * Returns      : the pando command of specified no, NULL if not found.
*******************************************************************************/
pd_command* FUNCTION_ATTRIBUTE
find_pando_command(uint8_t no)
{
	int i;
	for(i = 0; i < s_pando_command_list_idx; i++)
	{
		if( s_pando_command_list[i].no == no)
	    {
			return &s_pando_command_list[i];
	    }
	}
	return NULL;
}

