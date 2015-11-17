/*********************************************************
 * File name: pando_sys.h
 * Author: xiaoxiao
 * Versions: 1.0
 * Description: declare common base function lib.
 * History:
 *   1.Date: Oct 28, 2015
 *     Author: xiaoxiao
 *     Modification:
 *********************************************************/

#ifndef _PANDO_SYS_H_
#define _PANDO_SYS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP8266_PLATFORM

#include "c_types.h"
#include "mem.h"
#include "osapi.h"
//#include "../gateway/pando_system_time.h"

// for #pragma pack(ALIGNED_LENGTH), for example, esp8266 should be 1
#define ALIGNED_LENGTH   1


// different platform has its own define of these functions.

#define pd_malloc os_malloc
#define pd_free os_free
#define pd_memcpy os_memcpy
#define pd_printf os_printf
#define pd_memcmp os_memcmp
#define pd_memset os_memset
#define pd_strlen   os_strlen
#define pd_strncpy  os_strncpy
#define pd_strcmp   os_strcmp

#else
#include "../platform/include/pando_types.h"

#define ALIGNED_LENGTH 1


// different platform has its own define of these functions.
#define pd_malloc malloc
#define pd_free free
#define pd_memcpy memcpy
#define pd_printf printf
#define pd_memcmp memcmp
#define pd_memset memset
#define pd_strlen   strlen
#define pd_strncpy  strncpy
#define pd_strcmp   strcmp


#endif

#endif /* _PANDO_SYS_H_ */
