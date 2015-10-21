/**********************************************************************************
 * File name: pando_lan_bind.h
 * Author:Chongguang Li
 * Versions:1.0
 * Description:This module is to used to send device bind message to mobile phone through lan.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 **********************************************************************************/

#include "json/json.h"
#include "json/jsontree.h"
#include "json/jsonparse.h"
#include "c_types.h"
#include "gateway_defs.h"
#include "../protocol/pando_endian.h"
#include "pando_storage_interface.h"
#include "espconn.h"
#include "user_interface.h"

#define TCP_SERVER_PORT 8890
#define JSON_HEAD_MAGIC 0x7064
#define JSON_PACK_TYPE  0x0001
#define JSON_BUF_LEN 100

struct JSON_PACK_HEAD
{
    uint16 magic;
    uint16 type;
    uint32 length;
};

/******************************************************************************
 * FunctionName : send_device_key.
 * Description  : send the device_key to the mobile phone.
 * Parameters   : arg -- additional argument to pass to the callback function.
 * Returns      : none
*******************************************************************************/

static void ICACHE_FLASH_ATTR
send_device_key(void* arg)
{
     struct espconn* duplicate_conn = arg;
     char* str_device_key = NULL;
     str_device_key = pando_data_get(DATANAME_DEVICE_KEY);
     
     uint32 package_length = 0;
     struct JSON_PACK_HEAD json_pack_head;
     char* json_pack = (char *)os_malloc(JSON_BUF_LEN);
     json_pack_head.magic = host16_to_net(JSON_HEAD_MAGIC);
     json_pack_head.type = host16_to_net(JSON_HEAD_MAGIC);
     
     if(str_device_key != NULL)
     {
    	 // the json package head initialize, hton* is used for transferring the data to big endian.
    	 struct jsontree_string json_token = JSONTREE_STRING(str_device_key);
    	 JSONTREE_OBJECT_EXT(token, JSONTREE_PAIR("token", &json_token));
    	 int ret = pando_json_print(&token, json_pack + sizeof(json_pack_head), JSON_BUF_LEN);
    	 package_length = os_strlen(json_pack + sizeof(json_pack_head));
    	 json_pack_head.length = host32_to_net(package_length);


     }
     else
     {
    	PRINTF("no device_key, please check whether the device has registered!\n");
        json_pack_head.length = 0;
        package_length = 0;
     }
     os_memcpy(json_pack, &json_pack_head, sizeof(struct JSON_PACK_HEAD));
     espconn_sent(duplicate_conn, json_pack, package_length + sizeof(json_pack_head));

     if(json_pack != NULL)
     {
    	 os_free(json_pack);
     	 json_pack = NULL;
     }
}

/******************************************************************************
 * FunctionName : send_token.
 * Description  : espconn send callback function.
 * Parameters   : arg -- additional argument to pass to the callback function.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
tcp_send_cb(void* arg)
{
	PRINTF("send ok!\n");
}

/******************************************************************************
 * FunctionName : tcp_server_recv
 * Description  : Processing the received data from the client.
 * Parameters   : arg -- argument pass to the callback function.
 *                pusrdata -- The received data (or NULL when the connection has been closed!).
 *                length -- The length of received data.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
tcp_server_recv(void *arg, char *pusrdata, unsigned short length)
{
	PRINTF("TCP server receive data..\n");

	struct espconn* duplicate_conn = arg;
    struct jsonparse_state json_state;
    struct JSON_PACK_HEAD json_pack_head;
    int  type;
    char action[20];
    
    uint8_t* recv_buffer = (uint8_t*)os_malloc(length);
    os_memcpy(recv_buffer, pusrdata, length);\
    os_memcpy(&json_pack_head, recv_buffer, sizeof(struct JSON_PACK_HEAD));

    if(length < sizeof(struct JSON_PACK_HEAD))
    {
        PRINTF("not enough length!\n");
        return;
    }
    
    if(net16_to_host(json_pack_head.magic) != JSON_HEAD_MAGIC)
    {
        PRINTF("not a json package\n");
        return;
    }

    // prase json.
    jsonparse_setup(&json_state, (recv_buffer + sizeof(json_pack_head)), length);
    while ((type = jsonparse_next(&json_state)) != 0)
    {
    	if (type == JSON_TYPE_PAIR_NAME)
    	{
    		if(jsonparse_strcmp_value(&json_state, "action") == 0)
    		{
    			jsonparse_next(&json_state);
    			jsonparse_next(&json_state);
    			jsonparse_copy_value(&json_state, action, sizeof(action));

    			if(os_strncmp(action, "token", 5) == 0)
    			{
    				send_device_key(duplicate_conn);
    				break;
    			}
    		}
        }
    }

    if(recv_buffer != NULL)
    {
	   os_free(recv_buffer);
       recv_buffer = NULL;
    }
}



/******************************************************************************
 * FunctionName : tcp_server_recon.
 * Description  : .process the tcp server reconnect.
 * Parameters   : arg:additional argument to pass to the callback function.
 * 				  err: reconnect error.
 * Returns      : none
*******************************************************************************/

static void
 ICACHE_FLASH_ATTR
tcp_server_recon(void *arg, sint8 err)
{
    PRINTF("tcp server reconnect...error:%d\n", err); 
}


/******************************************************************************
 * FunctionName : tcp_server_discon
 * Description  : process the tcp server disonnect.
 * Parameters   : arg: additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/

static void ICACHE_FLASH_ATTR
tcp_server_discon(void *arg)
{
    PRINTF("tcp server disconnect...\n"); 
}

/******************************************************************************
 * FunctionName : tcp_server_listen
 * Description  : server listened a connection successfully, call back this function.
 * Parameters   : arg -- additional argument to pass to the callback function.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg)
{
	PRINTF("tcp server listened...\n");

    struct espconn* duplicate_conn = arg;
    espconn_regist_sentcb(arg, tcp_send_cb);
    espconn_regist_recvcb(duplicate_conn, tcp_server_recv);
    espconn_regist_reconcb(duplicate_conn, tcp_server_recon);
    espconn_regist_disconcb(duplicate_conn, tcp_server_discon);
}

/******************************************************************************
 * FunctionName : pando_lan_bind_init
 * Description  : initialize the lan tcp server.
 * Parameters   : nonde
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
pando_lan_bind_init()
{
	PRINTF("tcp server init...\n");

	struct espconn* bind_service_conn = (struct espconn*)os_malloc(sizeof(struct espconn));
	bind_service_conn->type = ESPCONN_TCP;
	bind_service_conn->state = ESPCONN_NONE;
	bind_service_conn->proto.tcp = (esp_tcp *)os_malloc(sizeof(esp_tcp));
	bind_service_conn->proto.tcp->local_port = TCP_SERVER_PORT; // listen port.

	espconn_regist_connectcb(bind_service_conn, tcp_server_listen);
	espconn_accept(bind_service_conn);
	espconn_regist_time(bind_service_conn, 600, 0); // tcp server connecting time.
}
