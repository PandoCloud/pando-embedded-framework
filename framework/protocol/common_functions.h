/*******************************************************
 * File name: common_functions.h
 * Author:    Zhao Wenwu
 * Versions:  0.1
 * Description: Common APIs for all platforms.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/



#ifndef PLATFORM_FUNCTIONS_H
#define PLATFORM_FUNCTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ESP8266_PLANTFORM

#else

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../platform/include/pando_types.h"

#endif

#include "pando_machine.h"

void show_package(uint8_t *buffer, uint16_t length);

#ifdef __cplusplus
}
#endif
#endif
