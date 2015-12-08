/*******************************************************
 * File name: common_functions.c
 * Author:    Zhao Wenwu
 * Versions:  0.1
 * Description: Common APIs for all platforms.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#include "common_functions.h"
#include "../platform/include/pando_sys.h"

void FUNCTION_ATTRIBUTE show_package(uint8_t *buffer, uint16_t length)
{
	int i = 0;
	pd_printf("Package length: %d\ncontent is: \n", length);

	for (i = 0; i < length; i++)
	{
		pd_printf("%02x ",(uint8_t)buffer[i]);
	}

	pd_printf("\n");
}

