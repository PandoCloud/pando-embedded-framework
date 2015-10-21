#include "pando_device_login.h"
#include "../../util/httpclient.h"
#include "pando_storage_interface.h"
#include "user_interface.h"
#include "c_types.h"
#include "osapi.h"
#include "mem.h"
#include "json/jsontree.h"
#include "json/jsonparse.h"
#include "../../util/converter.h"
#include "../../user/device_config.h"
#include "gateway_defs.h"

#define MAX_BUF_LEN 256
#define KEY_BUF_LEN 64
#define MSG_BUF_LEN 32
#define ACCESS_TOKEN_LEN 16

static login_callback device_login_callback = NULL;
static char * request = NULL;

extern uint8 pando_device_token[ACCESS_TOKEN_LEN];

static void ICACHE_FLASH_ATTR
http_callback_login(char * response, int http_status, char * full_response)
{
    if(request != NULL)
    {
    	os_free(request);
    	request = NULL;
    }
    
    if(response == NULL)
    {
        device_login_callback(ERR_LOGIN_FAIL);
        return;
    }
    
    PRINTF("response=%s\n(end)\n", response);
    
    struct jsonparse_state json_state;
    jsonparse_setup(&json_state, response, os_strlen(response));
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
        PRINTF("device login failed: %s\n", message);
        if(device_login_callback != NULL) 
        {
            device_login_callback(ERR_NOT_REGISTERED);
        }
        return;
    }

    hex2bin(pando_device_token, access_token);


    PRINTF("device login success, access_addr : %s\n", access_addr);

    pando_data_set(DATANAME_ACCESS_ADDR, access_addr);
    pando_data_set(DATANAME_ACCESS_TOKEN, access_token);
    if(device_login_callback != NULL) 
    {
        device_login_callback(LOGIN_OK);
    }
}

/******************************************************************************
 * FunctionName : pando_device_login
 * Description  : try login device using pando cloud device register api.
 * Parameters   : login callback function
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
pando_device_login(login_callback callback)
{
    PRINTF("begin login device...\n");

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
        PRINTF("login failed ! device has not been registerd...\n");
        device_login_callback(ERR_NOT_REGISTERED);
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

    request = (char *)os_malloc(MAX_BUF_LEN);
    int ret = pando_json_print(&device_info, request, MAX_BUF_LEN);

    PRINTF("device login request:::\n%s\n(end)\n", request);

    http_post(PANDO_API_URL
        "/v1/devices/authentication",
        request,
        http_callback_login);    
    if(request != NULL)
    {
    	os_free(request);
    	request = NULL;
    }
}
