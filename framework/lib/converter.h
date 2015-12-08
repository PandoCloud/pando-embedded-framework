#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include "../platform/include/pando_types.h"

/******************************************************************************
 * FunctionName : hex2bin.
 * Description  : convert. string to bin.
 * Parameters   : dst --
 *                hex_str --
 * Returns      : none.
*******************************************************************************/

void hex2bin(uint8 * dst, char * hex_str);

const char *u32_to_str(unsigned int val);

#endif
