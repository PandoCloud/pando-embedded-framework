#include "pando_cloud_access.h"
#include "pando_storage_interface.h"
#include "../platform/include/pando_types.h"
#include "gateway_defs.h"
#include "pando_channel.h"
//#include "pando_system_time.h"
#include "mqtt/mqtt.h"
#include "../protocol/sub_device_protocol.h"
#include "../protocol/pando_protocol.h"
#include "../platform/include/pando_sys.h"

#define PORT_STR_LEN 8
#define DEVICE_TOKEN_LEN 16

gateway_callback error_callback = NULL;
uint8_t pando_device_token[DEVICE_TOKEN_LEN];

MQTT_Client mqtt_client;
static uint8_t str_device_id_hex[17];

static int FUNCTION_ATTRIBUTE
conv_addr_str(const char * ip_str, uint8_t * str_ip_addr, int * port)
{
    char * colon = NULL;
    if ((colon = strchr(ip_str, ':')) == NULL)
    {
        return -1;
    }
    // ip address
    size_t ip_len = colon - ip_str;
    pd_strncpy(str_ip_addr, ip_str, ip_len);
    str_ip_addr[ip_len] = '\0';

    // port
    char str_port_addr[PORT_STR_LEN];
    pd_strncpy(str_port_addr, colon+1, PORT_STR_LEN);
    *port = atoi(str_port_addr);
    return 0;
}

static void FUNCTION_ATTRIBUTE
init_gateway_info()
{
    // initialize gateway package.
    struct protocol_base gateway_info;
    pd_memset(&gateway_info, 0, sizeof(gateway_info));
    gateway_info.device_id = atol(pando_data_get(DATANAME_DEVICE_ID));
    pd_memcpy(gateway_info.token, pando_device_token, DEVICE_TOKEN_LEN);
    pd_printf("token:\n");
    show_package(gateway_info.token, DEVICE_TOKEN_LEN);
    pando_protocol_init(gateway_info);
}

static void FUNCTION_ATTRIBUTE
pando_publish_data_channel1(uint8_t* buffer, uint16_t length)
{
	pd_printf("pando_publish_data_channel1");
    struct pando_buffer *gateway_data_buffer = NULL;
    uint16_t buf_len = 0;
    uint16_t payload_type = 0;
    buf_len = GATE_HEADER_LEN + length - sizeof(struct device_header);
    gateway_data_buffer = pando_buffer_create(buf_len, GATE_HEADER_LEN - sizeof(struct device_header));

    if (gateway_data_buffer->buffer == NULL)
    {
        pd_printf("%s:malloc failed.\n", __func__);
        return;
    }

    pd_memcpy(gateway_data_buffer->buffer + gateway_data_buffer->offset, buffer, length);
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
            pd_memcpy(topic, "c", 2);
            break;
        case PAYLOAD_TYPE_EVENT:
            pd_memcpy(topic, "e", 2);
            break;
        case PAYLOAD_TYPE_DATA:
            pd_memcpy(topic, "d", 2);
            break;
        default:
            pd_printf("error payload type\n");
            pando_buffer_delete(gateway_data_buffer);
            return;
    }
    MQTT_Publish(&mqtt_client, topic, gateway_data_buffer->buffer, gateway_data_buffer->buff_len, 1, 0);
    pando_buffer_delete(gateway_data_buffer);
}

static void FUNCTION_ATTRIBUTE
mqtt_data_cb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	pd_printf("mqtt_data_cb\n");
    pd_printf("mqtt topic length: %d\n", topic_len);
    pd_printf("mqtt data length: %d\n", data_len);

    uint16_t sub_device_id = 0;

    if((topic == NULL) || (data == NULL))
    {
        pd_printf("no needed mqtt package!");
        return;
    }
    char *topic_buf = (char*)pd_malloc(topic_len+1);
	pd_memset(topic_buf, 0 , topic_len + 1);
    pd_memcpy(topic_buf, topic, topic_len);
    topic_buf[topic_len] = 0;
    pd_printf("the topic is: %s\n", topic_buf);

    uint8_t payload_type = 0;
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
    pd_buffer = (struct pando_buffer *)pd_malloc(sizeof(struct pando_buffer));
    if(pd_buffer == NULL)
    {
        pd_printf("malloc error!\n");
        return;
    }
    pd_buffer->buff_len = data_len;
    pd_buffer->buffer = (uint8_t*)pd_malloc(data_len);
    if(pd_buffer->buffer == NULL)
    {
        pd_printf("malloc error!\n");
        return;
    }
    pd_memcpy(pd_buffer->buffer, data, data_len);
    pd_buffer->offset = 0;
    pando_protocol_get_sub_device_id(pd_buffer, &sub_device_id);

    pd_printf("package from server, get rid of mqtt head:\n");
    show_package(pd_buffer->buffer, pd_buffer->buff_len);
    if(pando_protocol_decode(pd_buffer, payload_type) != 0)
    {
        pd_printf("the data from server is wrong!\n");
        return;
    }

    struct sub_device_buffer *device_buffer = (struct sub_device_buffer *)pd_malloc(sizeof(struct sub_device_buffer));
    device_buffer->buffer_length = pd_buffer->buff_len - pd_buffer->offset;
    device_buffer->buffer = (uint8_t*)pd_malloc(device_buffer->buffer_length);
    pd_memcpy(device_buffer->buffer, pd_buffer->buffer + pd_buffer->offset, device_buffer->buffer_length);
    pando_buffer_delete(pd_buffer);

    if(sub_device_id == 1 || sub_device_id == 65535) //65535 is broadcast id.
    {
        pd_printf("transfer data to sub device: %d\n", sub_device_id);
        channel_send_to_subdevice(PANDO_CHANNEL_PORT_1, device_buffer->buffer,device_buffer->buffer_length);
    }

    else if(sub_device_id == 0)
    {
        channel_send_to_subdevice(PANDO_CHANNEL_PORT_0, device_buffer->buffer,device_buffer->buffer_length);
    }
    delete_device_package(device_buffer);
}

static void FUNCTION_ATTRIBUTE
mqtt_published_cb(uint32_t *arg)
{
    pd_printf("MQTT: Published\r\n");
    MQTT_Client* client = (MQTT_Client*)arg;

}

static void FUNCTION_ATTRIBUTE
mqtt_connect_cb(uint32_t* arg)
{
    pd_printf("MQTT: Connected\r\n");
    MQTT_Client* client = (MQTT_Client*)arg;
    on_device_channel_recv(PANDO_CHANNEL_PORT_1, pando_publish_data_channel1);
}

static void FUNCTION_ATTRIBUTE
mqtt_disconnect_cb(uint32_t* arg)
{
    pd_printf("MQTT: Disconnected\r\n");
    MQTT_Client* client = (MQTT_Client*)arg;
}
static void FUNCTION_ATTRIBUTE
mqtt_error_cb(uint32_t* arg)
{
    pd_printf("MQTT: connecting error\r\n");
    if(error_callback != NULL)
    {
        error_callback(PANDO_ACCESS_ERR);
    }
}

/******************************************************************************
* FunctionName : pando_cloud_access
* Description  : pando cloud device access api.
* Parameters   : callback: the specify access callback function.
* Returns      :
*******************************************************************************/
void FUNCTION_ATTRIBUTE
pando_cloud_access(gateway_callback callback)
{
    pd_printf("PANDO: begin access cloud...\n");

    pd_printf("before access:\n");
    if(callback != NULL)
    {
        error_callback = callback;
    }

    char* access_addr = pando_data_get(DATANAME_ACCESS_ADDR);
    if( NULL == access_addr )
    {
        pd_printf("no access server address found...\n");
        error_callback(PANDO_ACCESS_ERR);
		return;
    }

    int port;
    uint8_t ip_string[16];
    if(0 != (conv_addr_str(access_addr, ip_string, &port)))
    {
        pd_printf("wrong access server address...\n");
        error_callback(PANDO_ACCESS_ERR);
		return;
    }

    char* str_device_id = pando_data_get(DATANAME_DEVICE_ID);

    int device_id = atol(str_device_id); // TODO: device id is 64bit, atol not support.
    os_sprintf(str_device_id_hex, "%x", device_id);
    init_gateway_info();

    MQTT_InitConnection(&mqtt_client, ip_string, port, 0);

   // MQTT_Connect(&mqtt_client);

    char access_token_str[64];
    char* token_str = pando_data_get(DATANAME_ACCESS_TOKEN);
    pd_memcpy(access_token_str, token_str, pd_strlen(token_str));
    MQTT_InitClient(&mqtt_client, str_device_id_hex, "", access_token_str, 30, 1);
    pd_printf("access str_device_id_hex:%s\n",&str_device_id_hex);
    MQTT_OnConnected(&mqtt_client, mqtt_connect_cb);
    MQTT_OnDisconnected(&mqtt_client, mqtt_disconnect_cb);
    MQTT_OnPublished(&mqtt_client, mqtt_published_cb);
    MQTT_OnData(&mqtt_client, mqtt_data_cb);
    MQTT_OnConnect_Error(&mqtt_client, mqtt_error_cb);
    MQTT_Connect(&mqtt_client);
    // to consider: reconnect.
}
