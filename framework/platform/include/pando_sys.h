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

#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define pd_malloc       os_malloc
#define pd_free         os_free

#define pd_memcpy       os_memcpy
#define pd_memcmp       os_memcmp
#define pd_memset       os_memset

#define pd_strlen       os_strlen
#define pd_strncpy      os_strncpy
#define pd_strcmp       os_strcmp
#define pd_strncmp      os_strncmp
#define pd_strchr		os_strchr

#define pd_sprintf      os_sprintf

#define pd_printf       os_printf

#endif /* _PANDO_SYS_H_ */
