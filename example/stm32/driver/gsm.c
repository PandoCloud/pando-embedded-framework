/*******************************************************
 * File name: gsm.c
 * Author: Chongguang Li
 * Versions: 1.0
 * Description:This module is gsm api.
 * History:
 *   1.Date: initial code
 *     Author: Chongguang Li
 *     Modification:
 *********************************************************/

#include "usart2.h"
#include "gsm.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "timer4.h"
#include "fifo.h"
#include "stdlib.h"
#include "malloc.h"
#include "pando_net_tcp.h"
#include "common_functions.h"

#define ATC_RSP_FINISH 1
#define ATC_RSP_WAIT 0
#define AT_MAX_RESPONSE_TIME 300

extern uint8_t g_imei_buf[16];

struct gsm_buffer{
	uint8_t *buffer;
	uint16_t length;
};

static struct gsm_buffer *gsm_send_data_buffer = NULL;
static gsm_tcp_connected_callback tcp_connect_cb = NULL;
static gsm_tcp_sent_callback tcp_sent_cb = NULL;
static gsm_tcp_recv_callback tcp_recv_cb = NULL;
static gsm_tcp_disconnected_callback tcp_disconnected_cb = NULL;

typedef int8_t (*AT_cmdHandle)(bool *urc, char* buf);
typedef struct
{
    char *name;
    AT_cmdHandle at_cmd_handle;
}AtcHandleType;

static int8_t at_handle( bool *urc, char* buf);
static int8_t sapbr_open_hanle( bool *urc, char* buf);
static int8_t gsn_handle( bool *urc, char* buf);
static int8_t http_data_para_handle(bool *urc, char* buf);
static int8_t http_action_handle(bool *urc, char*buf);
static int8_t http_read_hanle(bool *urc, char*buf);
static int8_t cipstart_handle(bool *urc, char*buf);
static int8_t ipsend_handle(bool *urc, char*buf);
static int8_t urc_handle(bool *urc, char *buf);
static int8_t tcp_closed_handle(bool *urc, char *buf);

static void at_fifo_check(void *arg);

//AT command table.
AtcHandleType  atCmdTable[ ] =
{
        {"ATE",  at_handle},
        {"AT+IFC",  at_handle},
        {"AT+CPIN",   at_handle},
        {"AT+CSQ",  at_handle},
		{"AT+GSN", gsn_handle},
		{"AT+HTTPINIT", at_handle},
		{"AT+HTTPPARA_CID", at_handle},
		{"AT+HTTPPARA_URL", at_handle},
		{"AT+HTTPPARA_HEAD", at_handle},
		{"AT+HTTPSSL", at_handle},
		{"AT+HTTPDATA_PARA", http_data_para_handle},
		{"AT+HTTPDATA", at_handle},
		{"AT+HTTPACTION", http_action_handle},
		{"AT+SAPBR_TYPE", at_handle},
		{"AT+SAPBR_APN", at_handle},
		{"AT+SAPBR_OPEN", sapbr_open_hanle},
		{"AT+HTTPREAD", http_read_hanle},
		{"AT+HTTPTERM", at_handle},
		{"AT+CSTT", at_handle},
		{"AT+CIPSTART", cipstart_handle},
        {"AT+CIPSEND", ipsend_handle},
        {"AT+CIPCLOSE", at_handle}
};

// urc table,
AtcHandleType urc_table[] =
{
	{"Call Ready",  urc_handle},
	{"SMS Ready", urc_handle},
	{"CLOSED", tcp_closed_handle},
	{"+CPIN: READY", urc_handle},
	{"RDY", urc_handle}
};


struct gsm_buf{
	uint8_t* buf;
	uint16_t length;
};

struct at_cmd_entity{
	uint8_t at_name[20];
	uint8_t at_cmd[128];
};

static int8_t s_gsm_status = GSM_OFF_LINE;
static struct FIFO  s_at_fifo;
static uint8_t s_at_status = ATC_RSP_FINISH;
static struct at_cmd_entity *s_current_at_command = NULL;
static GSM_HTTP_CALLBACK http_callback = NULL;

static void send_at_command(const char* cmd)
{
	uint32_t i;
	//TODO: replace with delay api.
	for(i = 0; i<500; i++);
	printf("send cmd: %s, length:%d\r\n", cmd, strlen(cmd));
	usart2_send((uint8_t*)cmd, strlen(cmd));
	
}

static int8_t urc_handle(bool *urc, char *buf)
{
	printf("urc handle\n");
}

static int8_t at_handle( bool *urc, char* buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }

    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

	switch (res)
	{
    	case 0:  // OK
    	{
           s_at_status  = ATC_RSP_FINISH;
    	}
    	break;

        case 1: // ERROR
        {
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        default:
        break;
    }
   	return s_at_status;
}

static int8_t gsn_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
	static uint8_t flag = 0;
    p= (char *)buf;
    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }
	
	if(flag == 0)
	{
		memcpy(g_imei_buf, p, 15);
		g_imei_buf[15] = 0;
		printf("the imei is %s\n", g_imei_buf);
		flag = 1;
	}
	
    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

	switch (res)
    {
    	case 0:  // OK
    	{
           s_at_status  = ATC_RSP_FINISH;
    	}
    	break;

        case 1: // ERROR
        {
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        default:
        break;
    }
   	return s_at_status;
}

static int8_t sapbr_open_hanle( bool *urc, char* buf)
{
	/// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }

    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

    switch (res)
    {
    	case 0:  // OK
    	{
		   s_gsm_status = GSM_GET_IP;
           s_at_status  = ATC_RSP_FINISH;
    	}
    	break;

        case 1: // ERROR
        {
			//TODO: error process.
		   s_gsm_status = GSM_GET_IP;
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        default:
        break;
    }

   	return s_at_status;
}

static int8_t http_data_para_handle(bool *urc, char* buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"DOWNLOAD","ERROR"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }

    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

    switch (res)
    {
    	case 0:  // OK
    	{
           s_at_status  = ATC_RSP_FINISH;
    	}
    	break;

        case 1: // ERROR
        {
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        default:
        break;
    }

   	return s_at_status;
}

static int8_t http_action_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR","+HTTPACTION"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }

    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

   switch (res)
    {
    	case 0:  // OK
    	{
           //s_at_status  = ATC_RSP_FINISH;
    	}
    	break;

        case 1: /* ERROR */
        {
		    s_at_status = ATC_RSP_FINISH;
        }
        break;

        case 2:
        {
			//TODO: consider status is not 200;
			s_at_status = ATC_RSP_FINISH;

        }
        break;

        default:
        break;
    }
   	return s_at_status;
}

static int8_t http_read_hanle(bool *urc, char*buf)
{
	// TODO: deal with urc
    char *rep_str[ ] = {"OK","ERROR","+HTTPREAD: "};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }

    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }
   //*urc = FALSE;
    switch (res)
    {
		case 0:  /* OK */
    	{
			char len_str[4];
        	char* http_response_buffer = NULL;
        	uint16_t rsp_len;
			p = strstr(p,"+HTTPREAD: ");
			p = p + strlen("+HTTPREAD: ");
			memcpy(len_str, p, 3);
        	len_str[3] = 0;
        	rsp_len = atoi(len_str);
        	http_response_buffer = (char*)mymalloc(rsp_len) + 1;
        	p = strstr(p, "{");
        	memcpy(http_response_buffer, p, rsp_len);
        	http_response_buffer[rsp_len] = 0;
        	printf("http_respon_buffer:%s\n", http_response_buffer);
        	s_at_status = ATC_RSP_FINISH;
        	if(http_callback != NULL)
        	{
        		http_callback(http_response_buffer);
        		if(http_response_buffer != NULL)
        		{
        			myfree(http_response_buffer);
        			http_response_buffer = NULL;
        		}
        	}
    	}
    	break;
        case 1: /* ERROR */
        {
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        case 2:
        {
        	
        }
        break;
        default:
        break;
    }
   	return s_at_status;
}

static int8_t cipstart_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"CONNECT OK", "ERROR", "OK"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;
    while ( '\r' == *p || '\n' == *p)
    {
       p++;
    }
    for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
    {
    	if (strstr( p,rep_str[i]))
       {
           res = i;
           break;
       }
    }

    switch (res)
    {
        case 0:  // CONNECT OK.
    	{
    		if(tcp_connect_cb != NULL)
    		{
    			tcp_connect_cb(0, 0);
    		}
    		s_at_status = ATC_RSP_FINISH;
    	}
    	break;
        case 1: // ERROR
        {
        	//tcp_connect_cb(conn, 1);
        	s_at_status = ATC_RSP_FINISH;
        }
        break;
        case 2:
        {

        }
        break;
        default:
        //*urc = TRUE;
        break;
    }

   	return s_at_status;
}

int8_t ipsend_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"SEND OK","ERROR", ">", "OK", "CLOSED"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
    p= (char *)buf;

	while ( '\r' == *p || '\n' == *p)
	{
		p++;
	}

	for (i = 0; i < sizeof(rep_str) / sizeof(rep_str[0]); i++)
	{
		if (strstr( p,rep_str[i]))
	    {
			res = i;
			break;
	    }
	}

	switch (res)
	{
		case 0:  //SEND OK
	    {
			//TODO: add respose send ok!
	    	if(gsm_send_data_buffer != NULL)
	    	{
	    		if(gsm_send_data_buffer->buffer != NULL)
	    		{
	    			myfree(gsm_send_data_buffer->buffer);
	    			gsm_send_data_buffer->buffer = NULL;
	    		}
	    	}
	    	myfree(gsm_send_data_buffer);
	    	gsm_send_data_buffer = NULL;
	    	s_at_status = ATC_RSP_FINISH;
	    	tcp_sent_cb(0,0);
	    }
	    break;

	    case 1: //ERROR
	    {
	    	if(gsm_send_data_buffer != NULL)
	    	{
	    		if(gsm_send_data_buffer->buffer != NULL)
	    		{
	    			myfree(gsm_send_data_buffer->buffer);
	    			gsm_send_data_buffer->buffer = NULL;
	    		}
	    	}
	    	myfree(gsm_send_data_buffer);
	    	gsm_send_data_buffer = NULL;
	    	s_at_status = ATC_RSP_FINISH;
	    	tcp_sent_cb(0,-1);
	    	s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    case 2:
	    {
	    	if(gsm_send_data_buffer != NULL)
	    	{
	    		printf("sending.....\n");
	    		usart2_send(gsm_send_data_buffer->buffer, gsm_send_data_buffer->length);
	    		//TODO:need to send ctrl+z;
	    	}
	    }
	    break;

	    case 3:
	    {
	    	
	    }
	    break;

	    case 4:
	    {
	    	// TODO: free send buf and set the sending state to finish?
	    }
	    break;

	    default:
	    break;
	 }
	   	return s_at_status;
}

static int8_t tcp_closed_handle(bool *urc, char *buf)
{
	tcp_disconnected_cb(0, -1);
	return -1;
}

uint8_t gsm_system_start()
{
	s_gsm_status = GSM_START;
	timer4_init(1000, 1, at_fifo_check);
	timer4_start();
	return s_gsm_status;
}

void register_gsm_http_callback(GSM_HTTP_CALLBACK http_cb)
{
	http_callback = http_cb;
}

void register_gsm_tcp_connect_callback(uint16_t fd, gsm_tcp_connected_callback connect_cb)
{
	tcp_connect_cb = connect_cb;
}

void register_gsm_tcp_sent_callback(uint16_t fd, gsm_tcp_sent_callback sent_callback)
{
	tcp_sent_cb = sent_callback;
}

void register_gsm_tcp_recv_callback(uint16_t fd, gsm_tcp_recv_callback recv_callback)
{
	tcp_recv_cb = recv_callback;
}

void register_gsm_tcp_disconnected_callback(uint16_t fd, gsm_tcp_disconnected_callback tcp_disconnected_callback)
{
	tcp_disconnected_cb = tcp_disconnected_callback;
}

uint8_t gsm_system_init()
{
	FIFO_Init (&s_at_fifo);
	char at_cmd_Buff[30] = {0};
	char at_name_Buff[20] = {0};

    //ATE0, set no echo.
    strcpy(at_name_Buff, "ATE");
    strcpy(at_cmd_Buff, "ATE0\r\n");
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);

    //AT+IFC=0, set no flow control.
    memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+IFC");
    strcpy(at_cmd_Buff, "AT+IFC=0\r\n");
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);

    //AT+CPIN, query .....
    memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+CPIN");
    strcpy(at_cmd_Buff, "AT+CPIN?\r\n");
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);

    //AT+CSQ
    memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+CSQ");
    strcpy(at_cmd_Buff, "AT+CSQ\r\n");     
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
	
	//AT+GSN  request for the IMEI of the gsm.
	memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+GSN");
    strcpy(at_cmd_Buff, "AT+GSN\r\n");     
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
	
	//AT+SAPBR, config type of internet connection.
	memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+SAPBR_TYPE");
    strcpy(at_cmd_Buff, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");     
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
	
	//AT+SAPBR, config access point.
	memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+SAPBR_APN");
    strcpy(at_cmd_Buff, "AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n");     
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
	
	//AT+SAPBR, open bearer.
	memset(at_cmd_Buff, 0x0, sizeof(at_cmd_Buff));
    memset(at_name_Buff, 0x0, sizeof(at_name_Buff));
    strcpy(at_name_Buff,"AT+SAPBR_OPEN");
    strcpy(at_cmd_Buff, "AT+SAPBR=1,1\r\n");     
    FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
    
	s_gsm_status = GSM_INIT;
	return s_gsm_status;
}


static void at_fifo_check(void *arg)
{
	static uint8_t wait_response_tick = 0;

	if(s_gsm_status == GSM_START)
	{
		send_at_command("AT\r\n");
	}
	else if(GSM_INIT ==s_gsm_status|| GSM_INIT_DONE == s_gsm_status)
	{
		if ((NULL == s_current_at_command))
		{
			if ((!FIFO_isEmpty(&s_at_fifo)) &&(s_at_status == ATC_RSP_FINISH))
			{
				s_current_at_command = (struct at_cmd_entity*)malloc(sizeof(struct at_cmd_entity));
	            if (FIFO_Get(&s_at_fifo, s_current_at_command->at_name,  s_current_at_command->at_cmd))
	            {
	            	s_at_status = ATC_RSP_WAIT;
	                send_at_command(s_current_at_command->at_cmd);
	            }
			}
			else
			{
				printf("fifo is empty or the previous at command not complete!\n");
			}
		}
		else
		{
			printf("the at command is in sending!\n");
			if(wait_response_tick > AT_MAX_RESPONSE_TIME)
			{
				printf("the at command sending is timeout!\n");
			}
			wait_response_tick++;

		}
	}
}



/******************************************************************************
 * FunctionName : add_send_at_command
 * Description  : add the at command to the send buffer.
 * Parameters   : name_buffer: the at command name
 * 				  cmd_buffer: cmd_buffer: the command.
 * Returns      : none.
*******************************************************************************/
void add_send_at_command(uint8_t* name_buffer, uint8_t* cmd_buffer)
{
	char at_cmd_Buff[256] = {0};
	char at_name_Buff[20] = {0};

	strcpy(at_name_Buff, name_buffer);
	strcpy(at_cmd_Buff, cmd_buffer);

	FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff);
}


/******************************************************************************
 * FunctionName : get_gsm_status
 * Description  : get the status of the gsm.
 * Parameters   : none
 * Returns      :
*******************************************************************************/
int8_t  get_gsm_status()
{
	return s_gsm_status;
}

/******************************************************************************
 * FunctionName : set_gsm_status
 * Description  : set the status of the gsm.
 * Parameters   : status: the set status.
 * Returns      :
*******************************************************************************/
void set_gsm_status(int8_t status)
{
	s_gsm_status = status;
}

/******************************************************************************
 * FunctionName : gsm_send_data
 * Description  : gsm send data api.
 * Parameters   : fd: the send connect index.
 * 				  buf: the send data.
 * 				  len: the send length.
 * Returns      : none.
*******************************************************************************/
void gsm_send_data(uint16_t fd, uint8_t *buf, uint16_t len)
{
	if(gsm_send_data_buffer == NULL)
	{
		gsm_send_data_buffer = (struct gsm_buffer *)mymalloc(sizeof(struct gsm_buffer));
		gsm_send_data_buffer->length = len;
		gsm_send_data_buffer->buffer = (uint8_t*)mymalloc(len);
		memcpy(gsm_send_data_buffer->buffer, buf, len);
	}
	char command_buffer[48];
	sprintf(command_buffer, "AT+CIPSEND=%d\r\n", len);
	add_send_at_command("AT+CIPSEND", command_buffer);
}

static int8_t urc_process(uint8_t* data)
{
	// is urc.
	uint8_t i= 0;
	uint8_t urc = 0;
	for (i = 0; i <sizeof(urc_table)/sizeof(urc_table[0]); i++)
	{
		if(strstr(data, urc_table[i].name))
		{
			urc_table[i].at_cmd_handle(&urc, data);
			return 0;
		}
	}

	return -1;
}

int8_t gsm_data_handler(void* data)
{

	uint8_t urc = 0;
	struct gsm_buf* gsm_data = (struct gsm_buf*)data;
	printf("gsm response:%s\n", gsm_data->buf);
	show_package(gsm_data->buf, gsm_data->length);
	if(gsm_data->length < 3)
	{
		printf("gsm response not enough length!");
		if(gsm_data->buf != NULL)
		{
			free(gsm_data->buf);
			gsm_data->buf = NULL;
		}
		return -1;
	}
	if(GSM_START == s_gsm_status)
	{
		if((strstr(gsm_data->buf, "OK")))
		{
			printf("the \"at\" cmd response \"OK\"\n");
			s_gsm_status = GSM_SYNC;
		}
	}
	if (GSM_INIT ==s_gsm_status|| GSM_INIT_DONE == s_gsm_status)
	{
		int i = 0;
		if (NULL != s_current_at_command)
	    {
			for (i = 0; i <sizeof(atCmdTable)/sizeof(atCmdTable[0]); i++)
	        {
				if(!strcmp(s_current_at_command->at_name, atCmdTable[i].name))
	            {
					s_at_status =atCmdTable[i].at_cmd_handle(&urc, gsm_data->buf);

	                if (ATC_RSP_FINISH ==s_at_status)
	                {
	                	free(s_current_at_command);
	                    s_current_at_command = NULL;
	                }

	                break;
	            }
	        }
		}
		else
		{
			if(urc_process(gsm_data->buf) == -1) // not urc.
			{
				printf("%s\n, __func__");
				show_package(gsm_data->buf, gsm_data->length);
				tcp_recv_cb(0, gsm_data->buf, (gsm_data->length)-1); // -1 for identifier of string end.
			}

		}
	}

	if(gsm_data != NULL)
	{
		if(gsm_data->buf != NULL)
		{
			free(gsm_data->buf);
			gsm_data->buf = NULL;
		}
		free(gsm_data);
		gsm_data = NULL;
	}

	return 0;
}
