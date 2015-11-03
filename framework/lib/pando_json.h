/*******************************************************
 * File name: pando_json.h
 * Author: ruizeng
 * Versions: 1.0
 * Description:This module has some json handle utils.
 * History:
 *   1.Date: initial code
 *     Author: ruizeng
 *     Modification:    
 *********************************************************/

#ifndef __PANDO_JSON_H
#define __PANDO_JSON_H

#include "json/jsontree.h"

 /******************************************************************************
 * FunctionName : pando_json_print
 * Description  : print json value to a "char *" string.
 * Parameters   : json_value: the json_value struct ptr.
 *                dst: the string buffer to print to.
 *                len: the length of the output buffer.
 * Returns      : return the length printed, -1 if error
*******************************************************************************/

int pando_json_print(struct jsontree_value * json_value, char * dst, int len);

#endif
