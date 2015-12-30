/*********************************************************
 * File name: pando_types.h
 * Author: xiaoxiao
 * Versions: 1.0
 * Description: declare the types.
 * History:
 *   1.Date: Oct 28, 2015
 *     Author: xiaoxiao
 *     Modification:
 *********************************************************/

#ifndef _PANDO_TYPES_H_
#define _PANDO_TYPES_H_



#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

#define BIT(nr)                 (1UL << (nr))

#ifdef ESP8266_PLATFORM
#define FUNCTION_ATTRIBUTE ICACHE_FLASH_ATTR
#include "c_types.h"
#else
#define FUNCTION_ATTRIBUTE
#include <stdint.h>
#endif

#ifndef __cplusplus
typedef unsigned char   bool;
#define BOOL            bool
#define true            (1)
#define false           (0)
#define TRUE            true
#define FALSE           false

#endif /* !__cplusplus */

//define data handler callback
typedef void (* data_handler_callback)(uint8_t *buffer, uint16_t length);

#endif /* _PANDO_TYPES_H_ */

