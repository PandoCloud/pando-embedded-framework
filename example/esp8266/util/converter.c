/*******************************************************
 * File name: converter.c
 * Author:Chongguang Li
 * Versions:1.0
 * Description:This module is to used to covert the data coding.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#include "converter.h"
#include "user_interface.h"

/******************************************************************************
 * FunctionName : char2bin.
 * Description  : convert char to bin.
 * Parameters   : dst --
 * 				  hex_str --
 * Returns      : none.
*******************************************************************************/
static uint8_t ICACHE_FLASH_ATTR
char2bin(char a)
{
    uint8_t n;
    if(a>='0'&&a<='9')
    {
        n=a-'0';
    }
    else
    {
        n=a-'a'+10;
    }
    return n;
}

/******************************************************************************
 * FunctionName : hex2bin.
 * Description  : convert string to bin.
 * Parameters   : dst --
 * 				  hex_str --
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
hex2bin(uint8 * dst, char * hex_str)
{
    char * p = NULL;
    int i=0;
    for(p=hex_str; *p; p=p+2) 
    {
        dst[i++] = ((char2bin(*p)) << 4) | (char2bin(*(p+1)));
    }
}

const char* ICACHE_FLASH_ATTR
u32_to_str(unsigned int val)
{
        /* Maximum number of decimal digits in u32 is 10 */
        static char num_str[11];
        int  pos = 10;
        num_str[10] = 0;

        if (val == 0) {
                /* If already zero then just return zero */
                return "0";
        }

        while ((val != 0) && (pos > 0)) {
                num_str[--pos] = (val % 10) + '0';
                val /= 10;
        }

        return &num_str[pos];
}
