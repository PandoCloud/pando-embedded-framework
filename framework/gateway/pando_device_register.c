#include "pando_device_register.h"
#include "pando_storage_interface.h"
#include "../lib/json/jsontree.h"
#include "../lib/json/jsonparse.h"
#include "../platform/include/pando_net_tcp.h"
#include "../platform/include/pando_net_http.h"
#include "../platform/include/pando_types.h"
#include "../platform/include/pando_sys.h"
#include "gateway_defs.h"

#include "../../../user/device_config.h"

#define MAX_BUF_LEN 256
#define DEVICE_SERIAL_BUF_LEN 16
#define BIG_INT_BUF_LEN 21
#define KEY_BUF_LEN 96
#define MSG_BUF_LEN 32

static gateway_callback device_register_callback = NULL;
static char* request = NULL;

static void FUNCTION_ATTRIBUTE
http_callback_register(char * response)
{

	if(request != NULL)
	{
	    os_free(request);
	    request = NULL;
	}

    if(NULL == response)
    {
        PRINTF("http request failed\n");
        if(device_register_callback != NULL)
        {
        	return device_register_callback(PANDO_REGISTER_FAIL);
        }
    }

    PRINTF("response=%s\n(end)\n", response);

    struct jsonparse_state json_state;
    jsonparse_setup(&json_state, response, os_strlen(response));
    uint8 code;
    char message[MSG_BUF_LEN];
    long device_id;
    char device_secret[KEY_BUF_LEN];
    char device_key[KEY_BUF_LEN];

    int type;
    while ((type = jsonparse_next(&json_state)) != 0)
    {
        if (type == JSON_TYPE_PAIR_NAME) 
        {
            if(jsonparse_strcmp_value(&json_state, "code") == 0) 
            {
                jsonparse_next(&json_state);
                jsonparse_next(&json_state);
                code = jsonparse_get_value_as_int(&json_state);
            }
            else if(jsonparse_strcmp_value(&json_state, "message") == 0)
            {
                jsonparse_next(&json_state);
                jsonparse_next(&json_state);
                jsonparse_copy_value(&json_state, message, MSG_BUF_LEN);
            }
            else if(jsonparse_strcmp_value(&json_state, "data") == 0)
            {
                while((type = jsonparse_next(&json_state)) != 0 && json_state.depth > 1)
                {
                    if(type == JSON_TYPE_PAIR_NAME)
                    {
                        if(jsonparse_strcmp_value(&json_state, "device_id") == 0) 
                        {
                            jsonparse_next(&json_state);
                            jsonparse_next(&json_state);
                            device_id = jsonparse_get_value_as_long(&json_state);
                        }
                        else if(jsonparse_strcmp_value(&json_state, "device_secret") == 0)
                        {
                            jsonparse_next(&json_state);
                            jsonparse_next(&json_state);
                            jsonparse_copy_value(&json_state, device_secret, KEY_BUF_LEN);
                        }
                        else if(jsonparse_strcmp_value(&json_state, "device_key") == 0)
                        {
                            jsonparse_next(&json_state);
                            jsonparse_next(&json_state);
                            jsonparse_copy_value(&json_state, device_key, KEY_BUF_LEN);
                        }
                    }
                }
            }
        }
    }

    if(code != 0)
    {
        PRINTF("device register failed: %s\n", message);
        if(device_register_callback != NULL)
        {
          return device_register_callback(PANDO_REGISTER_FAIL);
        }
    }

    PRINTF("device register success, id: %d, secret : %s, key : %s\n",
        device_id, device_secret, device_key);
    char str_device_id[BIG_INT_BUF_LEN];
    os_sprintf(str_device_id, "%d", device_id);
    PRINTF("saving device info to storage...\n");
    pando_data_set(DATANAME_DEVICE_ID, str_device_id);
    pando_data_set(DATANAME_DEVICE_SECRET, device_secret);
    pando_data_set(DATANAME_DEVICE_KEY, device_key);
    PRINTF("done...\n");
    if(device_register_callback != NULL)
    {
        device_register_callback(PANDO_REGISTER_OK);
    }
}

void FUNCTION_ATTRIBUTE
pando_device_register(gateway_callback callback)
{
    PRINTF("PANDO begin register device...\n");

    if(callback != NULL)
    {
    	device_register_callback = callback;
    }

    char * str_device_id = NULL;
    char * str_device_secret = NULL;
    char * str_device_key = NULL;
    str_device_id = pando_data_get(DATANAME_DEVICE_ID);
    str_device_secret = pando_data_get(DATANAME_DEVICE_SECRET);
    str_device_key = pando_data_get(DATANAME_DEVICE_KEY);

    char str_device_serial[DEVICE_SERIAL_BUF_LEN];
    char device_sta_mac[6];
    wifi_get_macaddr(STATION_IF,device_sta_mac);
    os_sprintf(str_device_serial, "%02x%02x%02x%02x%02x%02x", device_sta_mac[0], device_sta_mac[1], device_sta_mac[2], device_sta_mac[3] \
    		, device_sta_mac[4], device_sta_mac[5]);
    PRINTF("device_serial:%s\n", str_device_serial);
    // try register device via HTTP
    struct jsontree_string json_product_key = JSONTREE_STRING(PANDO_PRODUCT_KEY);
    struct jsontree_string json_device_code = JSONTREE_STRING(str_device_serial);
    struct jsontree_int json_device_type = JSONTREE_INT(1);
    struct jsontree_string json_device_module = JSONTREE_STRING(PANDO_DEVICE_MODULE);
    struct jsontree_string json_version = JSONTREE_STRING(PANDO_SDK_VERSION);
    
    JSONTREE_OBJECT_EXT(device_info, 
        JSONTREE_PAIR("product_key", &json_product_key),
        JSONTREE_PAIR("device_code", &json_device_code),
        JSONTREE_PAIR("device_type", &json_device_type),
        JSONTREE_PAIR("device_module", &json_device_module),
        JSONTREE_PAIR("version", &json_version));
    request = (char *)os_malloc(MAX_BUF_LEN);
    int ret = pando_json_print(&device_info, request, MAX_BUF_LEN);

    PRINTF("device register request:::\n%s\n(end)\n", request);

    net_http_post(PANDO_API_URL
        "/v1/devices/registration",
        request,
        http_callback_register);    

    if(request != NULL)
    {
    	os_free(request);
        request = NULL;
    }

}
