/*******************************************************
 * File name: pando_net_http.c
 * Author:Chongguang Li
 * Versions:0.0.1
 * Description: the http api
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#include "pando_net_http.h"
#include "sim5360.h"

/******************************************************************************
 * FunctionName : net_http_post
 * Description  : the http post api.
 * Parameters   : url: the url.
 *                data: the post data.
 *                http_cb: the specify function called after post successfully.
 * Returns      : none
*******************************************************************************/
void net_http_post(const char* url, const char* data, http_callback http_cb)
{	
	module_http_post(url, data, (module_http_callback)http_cb);
}

/******************************************************************************
 * FunctionName : net_http_get
 * Description  : the http get api.
 * Parameters   : url: the url.
 *                http_cb: the specify function called after post successfully.
 * Returns      : none
*******************************************************************************/
void net_http_get(const char* url, http_callback http_cb)
{

}
