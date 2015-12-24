/*******************************************************
 * File name: pando_net_http.h
 * Author:Chongguang Li
 * Versions:0.0.1
 * Description: the http api
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#ifndef _PANDO_NET_HTTP_H_
#define _PANDO_NET_HTTP_H_

#include "pando_types.h"

#define BUFFER_SIZE_MAX  5000

typedef void (* http_callback)(char* response);

/******************************************************************************
 * FunctionName : net_http_post
 * Description  : the http post api.
 * Parameters   : url: the url.
 *                data: the post data.
 *                http_cb: the specify function called after post successfully.
 * Returns      : none
*******************************************************************************/
void net_http_post(const char* url, const char* data, http_callback http_cb);

/******************************************************************************
 * FunctionName : net_http_get
 * Description  : the http get api.
 * Parameters   : url: the url.
 *                http_cb: the specify function called after post successfully.
 * Returns      : none
*******************************************************************************/
void net_http_get(const char* url, http_callback http_cb);
/*
 * Call this function to skip URL parsing if the arguments are already in separate variables.
 */
void http_raw_request(const char * hostname, int port, const char * path, const char * post_data, http_callback user_callback);

/*
 * Output on the UART.
 */
void http_callback_example(char * response, int http_status, char * full_response);

#endif /* _PANDO_NET_HTTP_H_ */
