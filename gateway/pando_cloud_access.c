#include "pando_cloud_access.h"
#include "pando_storage_interface.h"
#include "user_interface.h"
#include "c_types.h"
#include "osapi.h"
#include "mem.h"
#include "espconn.h"
#include "gateway_defs.h"
#include "pando_channel.h"
#include "pando_system_time.h"
#include "mqtt/mqtt.h"
#include "../protocol/sub_device_protocol.h"
#include "../protocol/pando_protocol.h"

#define PORT_STR_LEN 8
#define DEVICE_TOKEN_LEN 16

access_error_callback error_callback = NULL;
uint8 pando_device_token[DEVICE_TOKEN_LEN];

MQTT_Client mqtt_client;
static uint8 str_device_id_hex[17];

static int ICACHE_FLASH_ATTR
conv_addr_str(const char * ip_str, uint8 * str_ip_addr, int * port)
{
    ip_addr_t ip_addr;
    char * colon = NULL;
    if ((colon = strchr(ip_str, ':')) == NULL)
    {
        return -1;
    }
    // ip address
    size_t ip_len = colon - ip_str;
    os_strncpy(str_ip_addr, ip_str, ip_len);
    str_ip_addr[ip_len] = '\0';

    // port
    char str_port_addr[PORT_STR_LEN];
    os_strncpy(str_port_addr, colon+1, PORT_STR_LEN);
    *port = atoi(str_port_addr);
    return 0;
}

static void ICACHE_FLASH_ATTR
init_gateway_info()
{
	// initialize gateway package.
	struct protocol_base gateway_info;
	os_memset(&gateway_info, 0, sizeof(gateway_info));
	gateway_info.device_id = atol(pando_data_get(DATANAME_DEVICE_ID));
	os_memcpy(gateway_info.token, pando_device_token, DEVICE_TOKEN_LEN);
	PRINTF("token:\n");
	show_package(gateway_info.token, DEVICE_TOKEN_LEN);
	pando_protocol_init(gateway_info);
}

static void ICACHE_FLASH_ATTR
pando_publish_data_channel1(uint8* buffer, uint16 length)
{
	struct pando_buffer *gateway_data_buffer = NULL;
	uint16_t buf_len = 0;
	uint16_t payload_type = 0;
	buf_len = GATE_HEADER_LEN + length - sizeof(struct device_header);
	gateway_data_buffer = pando_buffer_create(buf_len, GATE_HEADER_LEN - sizeof(struct device_header));

	if (gateway_data_buffer->buffer == NULL)
	{
		PRINTF("%s:malloc failed.\n", __func__);
		return;
	}

	os_memcpy(gateway_data_buffer->buffer + gateway_data_buffer->offset, buffer, length);
	if (pando_protocol_encode(gateway_data_buffer, &payload_type))
	{
		pd_printf("pando_protocol_encode error.\n");
		return;
	}

	pando_protocol_set_sub_device_id(gateway_data_buffer, 1);
	show_package(gateway_data_buffer->buffer, gateway_data_buffer->buff_len);
	char topic[2];
	switch(payload_type)
	{
		case PAYLOAD_TYPE_COMMAND:
		    os_memcpy(topic, "c", 2);
		    break;
		case PAYLOAD_TYPE_EVENT:
			os_memcpy(topic, "e", 2);
			break;
		case PAYLOAD_TYPE_DATA:
			os_memcpy(topic, "d", 2);
			break;
		default:
			PRINTF("error payload type\n");
			pando_buffer_delete(gateway_data_buffer);
			return;
	}
	MQTT_Publish(&mqtt_client, topic, gateway_data_buffer->buffer, gateway_data_buffer->buff_len, 1, 0);
	pando_buffer_delete(gateway_data_buffer);
}

static void ICACHE_FLASH_ATTR
mqtt_data_cb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	PRINTF("mqtt topic length: %d\n", topic_len);
	PRINTF("mqtt data length: %d\n", data_len);
	uint16 sub_device_id = 0;

	if((topic == NULL) || (data == NULL))
	{
		PRINTF("no needed mqtt package!");
		return;
	}
	char *topic_buf = (char*)os_zalloc(topic_len+1);
	os_memcpy(topic_buf, topic, topic_len);
	topic_buf[topic_len] = 0;
	PRINTF("the topic is: %s\n", topic_buf);

	uint8 payload_type = 0;
	switch(*topic_buf)
	{
		case 'd':
			payload_type = PAYLOAD_TYPE_DATA;
	    	break;
	    case 'c':
	    	payload_type = PAYLOAD_TYPE_COMMAND;
	    	break;
	    case 'e':
	    	payload_type = PAYLOAD_TYPE_EVENT;
	    	break;
	    default:
	    	return;
	}

	struct pando_buffer* pd_buffer;
	pd_buffer = (struct pando_buffer *)os_malloc(sizeof(struct pando_buffer));
	if(pd_buffer == NULL)
	{
		PRINTF("malloc error!\n");
		return;
	}
	pd_buffer->buff_len = data_len;
	pd_buffer->buffer = (uint8*)os_malloc(data_len);
	if(pd_buffer->buffer == NULL)
	{
		PRINTF("malloc error!\n");
		return;
	}
	os_memcpy(pd_buffer->buffer, data, data_len);
	pd_buffer->offset = 0;
	pando_protocol_get_sub_device_id(pd_buffer, &sub_device_id);

	PRINTF("package from server, get rid of mqtt head:\n");
	show_package(pd_buffer->buffer, pd_buffer->buff_len);
	if(pando_protocol_decode(pd_buffer, payload_type) != 0)
	{
		PRINTF("the data from server is wrong!\n");
		return;
	}

	struct sub_device_buffer *device_buffer = (struct sub_device_buffer *)os_malloc(sizeof(struct sub_device_buffer));
	device_buffer->buffer_length = pd_buffer->buff_len - pd_buffer->offset;
	device_buffer->buffer = (uint8*)os_malloc(device_buffer->buffer_length);
	os_memcpy(device_buffer->buffer, pd_buffer->buffer + pd_buffer->offset, device_buffer->buffer_length);
	pando_buffer_delete(pd_buffer);

	if(sub_device_id == 1 || sub_device_id == 65535) //65535 is broadcast id.
	{
		PRINTF("transfer data to sub device: %d\n", sub_device_id);
		channel_send_to_subdevice(PANDO_CHANNEL_PORT_1, device_buffer->buffer,device_buffer->buffer_length);
	}

	else if(sub_device_id == 0)
	{
		channel_send_to_subdevice(PANDO_CHANNEL_PORT_0, device_buffer->buffer,device_buffer->buffer_length);
	}
	delete_device_package(device_buffer);
}

static void ICACHE_FLASH_ATTR
mqtt_published_cb(uint32_t *arg)
{
	PRINTF("MQTT: Published\r\n");
	MQTT_Client* client = (MQTT_Client*)arg;

}

static void ICACHE_FLASH_ATTR mqtt_connect_cb(uint32_t* arg)
{
	PRINTF("MQTT: Connected\r\n");
    MQTT_Client* client = (MQTT_Client*)arg;
    on_device_channel_recv(PANDO_CHANNEL_PORT_1, pando_publish_data_channel1);
}

static void ICACHE_FLASH_ATTR mqtt_disconnect_cb(uint32_t* arg)
{
	PRINTF("MQTT: Disconnected\r\n");
	MQTT_Client* client = (MQTT_Client*)arg;
}
static void ICACHE_FLASH_ATTR mqtt_error_cb(uint32_t* arg)
{
	PRINTF("MQTT: connecting error\r\n");
	if(error_callback != NULL)
	{
		error_callback(ERR_CONNECT);
	}
}



/******************************************************************************
* FunctionName : pando_cloud_access
* Description  : pando cloud device access api.
* Parameters   : access_error_callback callback:access callback function
* Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
pando_cloud_access(access_error_callback callback)
{
    PRINTF("PANDO: begin access cloud...\n");

    PRINTF("before access:\n");
    PRINTF("available heap size:%d\n", system_get_free_heap_size());
    if(callback != NULL)
    {
        error_callback = callback;
    }

    char* access_addr = pando_data_get(DATANAME_ACCESS_ADDR);
    if( NULL == access_addr )
    {
        PRINTF("no access server address found...\n");
        return error_callback(ERR_OTHER);
    }

    int port;
    uint8 ip_string[16];
    if(0 != (conv_addr_str(access_addr, ip_string, &port)))
    {
        PRINTF("wrong access server address...\n");
        return error_callback(ERR_OTHER);
    }

    char* str_device_id = pando_data_get(DATANAME_DEVICE_ID);

    int device_id = atol(str_device_id); // TODO: device id is 64bit, atol not support.
    os_sprintf(str_device_id_hex, "%x", device_id);

    init_gateway_info();

    MQTT_InitConnection(&mqtt_client, ip_string, port, 0);

    MQTT_Connect(&mqtt_client);

    char access_token_str[64];
    char* token_str = pando_data_get(DATANAME_ACCESS_TOKEN);
    os_memcpy(access_token_str, token_str, os_strlen(token_str));
    MQTT_InitClient(&mqtt_client, str_device_id_hex, "", access_token_str, 30, 1);
    MQTT_OnConnected(&mqtt_client, mqtt_connect_cb);
    MQTT_OnDisconnected(&mqtt_client, mqtt_disconnect_cb);
    MQTT_OnPublished(&mqtt_client, mqtt_published_cb);
    MQTT_OnData(&mqtt_client, mqtt_data_cb);
    MQTT_OnConnect_Error(&mqtt_client, mqtt_error_cb);
    PRINTF("before mqtt connect:\n");
    PRINTF("available heap size:%d\n", system_get_free_heap_size());
    MQTT_Connect(&mqtt_client);
    // to consider: reconnect.
}
