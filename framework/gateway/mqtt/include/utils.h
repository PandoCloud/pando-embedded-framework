#ifndef _UTILS_H_
#define	_UTILS_H_

#include "c_types.h"

uint32_t FUNCTION_ATTRIBUTE UTILS_Atoh(const int8_t *s);
uint8_t FUNCTION_ATTRIBUTE UTILS_StrToIP(const int8_t* str, void *ip);
uint8_t FUNCTION_ATTRIBUTE UTILS_IsIPV4 (int8_t *str);
#endif
