#include "pando_device_login.h"
#include "pando_storage_interface.h"
#include "gateway_defs.h"
#include "../platform/include/pando_sys.h"
#include "../platform/include/pando_types.h"
#include "../lib/converter.h"
#include "../lib/json/jsonparse.h"
#include "../lib/json/jsontree.h"
#include "../lib/pando_json.h"
#include "../platform/include/pando_net_http.h"

#include "pando_gateway.h"

#define MAX_BUF_LEN 256
#define KEY_BUF_LEN 64
#define MSG_BUF_LEN 32
#define ACCESS_TOKEN_LEN 16

static gateway_callback device_login_callback = NULL;
static char * request = NULL;

extern uint8 pando_device_token[ACCESS_TOKEN_LEN];

static void FUNCTION_ATTRIBUTE
http_callback_login(char * response)
{
    if(request != NULL)
    {
        pd_free(request);
        request = NULL;
    }
    
    if(response == NULL)
    {
        device_login_callback(PANDO_LOGIN_FAIL);
        return;
    }
    
    pd_printf("response=%s\n(end)\n", response);
    
    struct jsonparse_state json_state;
    jsonparse_setup(&json_state, response, pd_strlen(response));
    int code;
    char message[MSG_BUF_LEN];
    char access_token[ACCESS_TOKEN_LEN*2 + 16];
    char access_addr[KEY_BUF_LEN];

    access_token[ACCESS_TOKEN_LEN*2] = '\0';
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
                        if(jsonparse_strcmp_value(&json_state, "access_token") == 0) 
                        {
                            jsonparse_next(&json_state);
                            jsonparse_next(&json_state);
                            jsonparse_copy_value(&json_state, access_token, ACCESS_TOKEN_LEN*2 + 16);
                        }
                        else if(jsonparse_strcmp_value(&json_state, "access_addr") == 0)
                        {
                            jsonparse_next(&json_state);
                            jsonparse_next(&json_state);
                            jsonparse_copy_value(&json_state, access_addr, KEY_BUF_LEN);
                        }
                    }
                }
            }
        }
    }

    if(code != 0)
    {
        pd_printf("device login failed: %s\n", message);
        if(device_login_callback != NULL) 
        {
            device_login_callback(PANDO_LOGIN_FAIL);
        }
        return;
    }

    hex2bin(pando_device_token, access_token);


    pd_printf("device login success, access_addr : %s\n", access_addr);

    pando_data_set(DATANAME_ACCESS_ADDR, access_addr);
    pando_data_set(DATANAME_ACCESS_TOKEN, access_token);
    if(device_login_callback != NULL) 
    {
        device_login_callback(PANDO_LOGIN_OK);
    }
}

/******************************************************************************
 * FunctionName : pando_device_login
 * Description  : try login device using pando cloud device register api.
 * Parameters   : the specify login callback function
 * Returns      :
*******************************************************************************/
void FUNCTION_ATTRIBUTE
pando_device_login(gateway_callback callback)
{
    pd_printf("begin login device...\n");
    if(callback != NULL)
    {
        device_login_callback = callback;
    }

    char * str_device_id = NULL;
    char * str_device_secret = NULL;
    
    str_device_id = pando_data_get(DATANAME_DEVICE_ID);
    str_device_secret = pando_data_get(DATANAME_DEVICE_SECRET);
    if(str_device_id == NULL || str_device_secret == NULL) 
    {
        // has not registered
        pd_printf("login failed ! device has not been registerd...\n");
        device_login_callback(PANDO_NOT_REGISTERED);
        return;
    }

    int device_id = atol(str_device_id);

    // try login via HTTP
    struct jsontree_int json_device_id = JSONTREE_INT(device_id);
    struct jsontree_string json_device_secret = JSONTREE_STRING(str_device_secret);
    struct jsontree_string json_protocol = JSONTREE_STRING("mqtt");

    JSONTREE_OBJECT_EXT(device_info, 
        JSONTREE_PAIR("device_id", &json_device_id),
        JSONTREE_PAIR("device_secret", &json_device_secret),
        JSONTREE_PAIR("protocol", &json_protocol));

    request = (char *)pd_malloc(MAX_BUF_LEN);
    int ret = pando_json_print((struct jsontree_value*)(&device_info), request, MAX_BUF_LEN);

    pd_printf("device login request:::\n%s\n(end)\n", request);

    net_http_post(PANDO_API_URL
        "/v1/devices/authentication",
        request,
        http_callback_login);    
    if(request != NULL)
    {
        pd_free(request);
        request = NULL;
    }
}
