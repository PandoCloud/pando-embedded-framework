#include "pando_device_register.h"
#include "platform/include/pando_storage_interface.h"
#include "platform/include/pando_sys.h"
#include "platform/include/pando_types.h"
#include "lib/json/jsonparse.h"
#include "lib/json/jsontree.h"
#include "lib/pando_json.h"
#include "platform/include/pando_net_http.h"

#define MAX_BUF_LEN 256
#define DEVICE_SERIAL_BUF_LEN 16
#define BIG_INT_BUF_LEN 21
#define KEY_BUF_LEN 96
#define MSG_BUF_LEN 32

static gateway_callback device_register_callback = NULL;
static char* request = NULL;

static void http_callback_register(char * response)
{
    if(request != NULL)
    {
        pd_free(request);
        request = NULL;
    }

    if(NULL == response)
    {
        pd_printf("http request failed\n");
        if(device_register_callback != NULL)
        {
            device_register_callback(PANDO_REGISTER_FAIL);
			return;
        }
    }

    pd_printf("response=%s\n(end)\n", response);

    struct jsonparse_state json_state;
    jsonparse_setup(&json_state, response, pd_strlen(response));
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
        pd_printf("device register failed: %s\n", message);
        if(device_register_callback != NULL) 
        {
			device_register_callback(PANDO_REGISTER_FAIL);
			return;
        }
    }

    pd_printf("device register success, id: %d, secret : %s, key : %s\n",
        device_id, device_secret, device_key);
    char str_device_id[BIG_INT_BUF_LEN];
    pd_sprintf(str_device_id, "%d", device_id);
    pd_printf("saving device info to storage...\n");
    pando_data_set(DATANAME_DEVICE_ID, str_device_id);
    pando_data_set(DATANAME_DEVICE_SECRET, device_secret);
    pando_data_set(DATANAME_DEVICE_KEY, device_key);
    pd_printf("done...\n");
    if(device_register_callback != NULL) 
    {
        device_register_callback(PANDO_REGISTER_OK);
    }
}

void pando_device_register(gateway_callback callback)
{
    pd_printf("PANDO begin register device...\n");

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
    get_device_serial(str_device_serial);
    str_device_serial[DEVICE_SERIAL_BUF_LEN - 1] = 0;
    pd_printf("device_serial:%s\n", str_device_serial);

    // try register device via HTTP
    struct jsontree_string json_product_key = JSONTREE_STRING("");
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
    request = (char *)pd_malloc(MAX_BUF_LEN);
    int ret = pando_json_print((struct jsontree_value*)(&device_info), request, MAX_BUF_LEN);

    pd_printf("device register request:::\n%s\n(end)\n", request);

    net_http_post(PANDO_API_URL
        "/v1/devices/registration",
        request,
        http_callback_register);    

    if(request != NULL)
    {
        pd_free(request);
        request = NULL;
    }
}
