/*******************************************************
 * File name: pando_subdevice.h
 * Author: razr
 * Versions: 1.0
 * Description:This module is to used to handle subdevice logic.
 * History:
 *   1.Date: Sep 11, 2015
 *     Author: razr
 *     Modification: initial code
 *********************************************************/
#ifndef PANDO_SUBDEVICE_H_
#define PANDO_SUBDEVICE_H_

#include "../platform/include/pando_types.h"

/******************************************************************************
 * FunctionName : pando_subdevice_recv.
 * Description  : process buffer receive from channel.
 * Parameters   : the buffer and length from channel.
 * Returns      : none.
*******************************************************************************/
void pando_subdevice_recv(uint8_t * buffer, uint16_t length);

#endif /* PANDO_SUBDEVICE_H_ */
