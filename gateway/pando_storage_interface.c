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
#include "spi_flash.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

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

static void ICACHE_FLASH_ATTR
save_data_to_flash()
{
    PRINTF("saving data to flash...\n");
    spi_flash_erase_sector(PANDO_CONFIG_SEC);
    int32 magic = PANDO_CONFIG_MAGIC;
    spi_flash_write((PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE),
        (uint32 *)&magic, sizeof(int32));
    struct data_pair * cur;
    int32 cnt = 0;
    for(cur=head; cur!=NULL; cur=cur->next) 
    {
        spi_flash_write((PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE)
            + sizeof(int32) + sizeof(int32)
            + (cnt * sizeof(struct data_pair)),
            (uint32 *)cur, sizeof(struct data_pair));
        cnt ++;
    }
    spi_flash_write((PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE) + sizeof(int32),
        (uint32 *)&cnt, sizeof(int32));
    PRINTF("done...\n");
}

void ICACHE_FLASH_ATTR
load_data_from_flash()
{
    PRINTF("loading config data from flash...\n");
    int32 cnt, i;
    int32 magic = 0;
    spi_flash_read(PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE,
        (uint32 *)&magic, sizeof(int32));
    PRINTF("read magic : %x\n", magic);
    if(magic != PANDO_CONFIG_MAGIC)
    {
        PRINTF("flash config data not initialized!\n");
        return;
    }
    spi_flash_read(PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE + sizeof(int32),
        (uint32 *)&cnt, sizeof(int32));
    PRINTF("reading config from flash , key count : %d...\n", cnt);
    for(i=0; i<cnt; i++) 
    {
        struct data_pair * p = (struct data_pair * )os_malloc(sizeof(struct data_pair));
        spi_flash_read(PANDO_CONFIG_SEC * SPI_FLASH_SEC_SIZE
            + sizeof(int32) + sizeof(int32) + sizeof(struct data_pair)*i,
            (uint32 *)p, sizeof(struct data_pair));
        p->next = head;
        head = p;
    }
    PRINTF("done...\n");
}

static struct data_pair * ICACHE_FLASH_ATTR
find_pair_by_key(char * key){
    struct data_pair * p;
    for( p=head; p!=NULL; p=p->next ) 
    {
        if(os_strcmp(key, p->key) == 0){
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

SET_RESULT ICACHE_FLASH_ATTR
pando_data_set(char* key, char* value)
{
    struct data_pair * p;
    p = find_pair_by_key(key);
    if(p != NULL)
    {
        // key exist, update value.
        os_strncpy(p->val, value, DATAPAIR_VALUE_LEN);
        PRINTF("key %s updated...\n", key, p->val);
    } else {
        // key not exist, create a new pair.
        struct data_pair * p = (struct data_pair * )os_malloc(sizeof(struct data_pair));
        os_strncpy(p->key, key, DATAPAIR_KEY_LEN);
        os_strncpy(p->val, value, DATAPAIR_VALUE_LEN);
        p->next = head;
        head = p;
        PRINTF("key %s created..., value %s \n", key, p->val);
    }
    save_data_to_flash();
}

/******************************************************************************
 * FunctionName : pando_data_get
 * Description  : get the value of the key.
 * Parameters   : key -- the parameter
 * Returns      : the pointer of the value. NULL if not exist
*******************************************************************************/

char * ICACHE_FLASH_ATTR
pando_data_get(char* key)
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

uint16 ICACHE_FLASH_ATTR
pando_storage_space_left()
{

}

/******************************************************************************
 * FunctionName : pando_storage_clean.
 * Description  : clean the pando configuration message.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
pando_storage_clean()
{
	spi_flash_erase_sector(PANDO_CONFIG_SEC);
}

