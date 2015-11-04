/*******************************************************
 * File name: pando_endian.h
 * Author:    Zhao Wenwu
 * Versions:  0.1
 * Description: APIs to change endian between net and host machine.
     Net is big endian, host endian depends machine. 
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/


#ifndef PANDO_ENDIAN_H
#define PANDO_ENDIAN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common_functions.h"

uint16_t net16_to_host(uint16_t A);
uint32_t net32_to_host(uint32_t A);
uint64_t net64_to_host(uint64_t A);
float    net32f_to_host(float A);
double   net64f_to_host(double A);

#define host16_to_net  net16_to_host
#define host32_to_net  net32_to_host
#define host64_to_net  net64_to_host
#define host32f_to_net net32f_to_host
#define host64f_to_net net64f_to_host

#ifdef __cplusplus
}
#endif
#endif



