/*******************************************************
 * File name: pando_storage_interface.h
 * Author:
 * Versions:1.0
 * Description:This module is the interface used to store the gateway data.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#include "pando_storage_interface.h"
#include "platform/include/pando_types.h"
#include "platform/include/pando_sys.h"
#include "flash.h"

#define PANDO_CONFIG_ADDRESS 0x08042000
#define PANDO_CONFIG_SEC (0x7E)
#define DATAPAIR_KEY_LEN (32)
#define DATAPAIR_VALUE_LEN (96)
#define PANDO_CONFIG_MAGIC (0x70646777)

struct data_pair
{
    struct data_pair * next;
    char key[DATAPAIR_KEY_LEN];
    char val[DATAPAIR_VALUE_LEN];
};

static struct data_pair * head = NULL;

static void save_data_to_flash()
{
    pd_printf("saving data to flash...\n");
    int32_t magic = PANDO_CONFIG_MAGIC;
    flash_write(PANDO_CONFIG_ADDRESS, (uint16_t*)(&magic), sizeof(int32_t));
    struct data_pair * cur;
    int32_t cnt = 0;
    pd_printf("saving data to flash...\n");
	flash_write(PANDO_CONFIG_ADDRESS, (uint16_t *)(&magic), sizeof(int32_t));
    for(cur=head; cur!=NULL; cur=cur->next)
    {
        flash_write(PANDO_CONFIG_ADDRESS + sizeof(int32_t) + sizeof(int32_t) + (cnt * sizeof(struct data_pair)),
            (uint16_t*)cur, sizeof(struct data_pair));
        cnt ++;
    }
    flash_write(PANDO_CONFIG_ADDRESS + sizeof(int32_t),
        (uint16_t*)&cnt, sizeof(int32_t));
    pd_printf("done...\n");
}

void load_data_from_flash()
{
    pd_printf("loading config data from flash...\n");
    int32_t cnt, i;
    int32_t magic = 0;
    pd_printf("loading config data from flash...\n");
    flash_read(PANDO_CONFIG_ADDRESS, (uint16_t *)&magic, sizeof(int32_t));
    pd_printf("read magic : %x\n", magic);
    if(magic != PANDO_CONFIG_MAGIC)
    {
        pd_printf("flash config data not initialized!\n");
        return;
    }

    flash_read(PANDO_CONFIG_ADDRESS + sizeof(int32_t), (uint16_t *)&cnt, sizeof(int32_t));
    pd_printf("reading config from flash , key count : %d...\n", cnt);
    for(i=0; i<cnt; i++)
    {
        struct data_pair * p = (struct data_pair * )pd_malloc(sizeof(struct data_pair));
        flash_read(PANDO_CONFIG_ADDRESS + sizeof(int32_t) + sizeof(int32_t) + sizeof(struct data_pair)*i,
            (uint16_t*)p, sizeof(struct data_pair));
        p->next = head;
        head = p;
    }
    pd_printf("done...\n");
}

static struct data_pair * find_pair_by_key(char * key){
    struct data_pair * p;
    for( p=head; p!=NULL; p=p->next )
    {
        if(pd_strcmp(key, p->key) == 0){
            return p;
        }
    }
    return NULL;
}

/******************************************************************************
 * FunctionName : pando_data_set
 * Description  : set the vale of the parameter stored, if the parameter is existing.
                  Else creat the parameter, and save it.
 * Parameters   : key -- the parameter;
                  value -- the value of the parameter.
 * Returns      : the save result
*******************************************************************************/
SET_RESULT pando_data_set(char* key, char* value)
{
    struct data_pair * p;
    p = find_pair_by_key(key);
    if(p != NULL)
    {
        // key exist, update value.
        pd_strncpy(p->val, value, DATAPAIR_VALUE_LEN);
        pd_printf("key %s updated...\n", key, p->val);
    } else {
        // key not exist, create a new pair.
        struct data_pair * p = (struct data_pair * )pd_malloc(sizeof(struct data_pair));
        pd_strncpy(p->key, key, DATAPAIR_KEY_LEN);
        pd_strncpy(p->val, value, DATAPAIR_VALUE_LEN);
        p->next = head;
        head = p;
        pd_printf("key %s created..., value %s \n", key, p->val);
    }
    save_data_to_flash();
}

/******************************************************************************
 * FunctionName : pando_data_get
 * Description  : get the value of the key.
 * Parameters   : key -- the parameter
 * Returns      : the pointer of the value. NULL if not exist
*******************************************************************************/
char * pando_data_get(char* key)
{
    struct data_pair * p;
    p = find_pair_by_key(key);
    if(p != NULL)
    {
        return p->val;
    }
    return NULL;
}

/******************************************************************************
 * FunctionName : pando_storage_space_left
 * Description  : get the space left for pando data saved in the storage.
 * Parameters   :
 * Returns      : the space left for pando data saving.
*******************************************************************************/
uint16_t pando_storage_space_left()
{

}

/******************************************************************************
 * FunctionName : pando_storage_clean.
 * Description  : clean the pando configuration message.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pando_storage_clean()
{

}

