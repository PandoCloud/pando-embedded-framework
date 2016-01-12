/*******************************************************
 * File name: sim5360.c
 * Author: Chongguang Li
 * Versions: 1.0
 * Description:This module is sim5360 api.
 * History:
 *   1.Date: initial code
 *     Author: Chongguang Li
 *     Modification:
 *********************************************************/

#include "usart2.h"
#include "sim5360.h"
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

// Suport different Content-Type header
#define HTTP_HEADER_CONTENT_TYPE "application/json"
#define MAX_HTTP_SIZE 1000

uint8_t g_imei_buf[16];

struct module_buffer{
	uint8_t *buffer;
	uint16_t length;
};

static struct module_buffer *module_send_data_buffer = NULL;
static module_tcp_connected_callback tcp_connect_cb = NULL;
static module_tcp_sent_callback tcp_sent_cb = NULL;
static module_tcp_recv_callback tcp_recv_cb = NULL;
static module_tcp_disconnected_callback tcp_disconnected_cb = NULL;
static module_http_callback s_http_cb = NULL;

static uint8_t s_csq_value = 0;
static char s_http_buffer[MAX_HTTP_SIZE];

typedef int8_t (*AT_cmdHandle)(bool *urc, char* buf);
typedef struct
{
    char *name;
    AT_cmdHandle at_cmd_handle;
}AtcHandleType;

// general handle.
static int8_t at_handle( bool *urc, char* buf);
static int8_t ate_handle(bool *urc, char* buf);
static int8_t csq_handle(bool *urc, char*buf);

// net open handle.
static int8_t net_open_hanle( bool *urc, char* buf);
static int8_t gsn_handle( bool *urc, char* buf);

// http handle.
/*static int8_t http_send_handle(bool *urc, char*buf);
static int8_t http_read_handle(bool *urc, char*buf);*/
static int8_t cch_open_handle(bool *urc, char*buf);
static int8_t cch_send_handle(bool *urc, char*buf);

// tcp handle.
static int8_t tcp_connect_handle(bool *urc, char*buf);
static int8_t ipsend_handle(bool *urc, char*buf);
static int8_t urc_handle(bool *urc, char *buf);
static int8_t tcp_closed_handle(bool *urc, char *buf);
static int8_t tcp_disconnect_handle(bool *urc, char *buf);

static void at_fifo_check(void *arg);

//AT command table.
AtcHandleType  atCmdTable[ ] =
{
        {"ATE",  ate_handle},
        {"AT+IFC",  at_handle},
        {"AT+CPIN",   at_handle},
        {"AT+CSQ",  csq_handle},
		{"AT+GSN", gsn_handle},
		{"AT+CGSOCKCONT", at_handle},
		{"AT+CIPMODE", at_handle},
		{"AT+NETOPEN", net_open_hanle},

		// creat connect.
		{"AT+SAPBR_TYPE", at_handle},
		{"AT+SAPBR_APN", at_handle},
		{"AT+SAPBR_OPEN", net_open_hanle},

		// https
		/*{"AT+CCHSTART", at_handle},
		{"AT+CHTTPSOPSE", at_handle},
		{"AT+CHTTPSSEND", http_send_handle},
		{"AT+CHTTPSRECV", http_read_handle},
		{"AT+CHTTPSCLSE", at_handle},
		{"AT+CHTTPSSTOP", at_handle},*/

		// https use CCH.
		{"AT+CCHSET", at_handle},
		{"AT+CCHSTART", at_handle},
		{"AT+CCHOPEN", cch_open_handle},
		{"AT+CCHSEND", cch_send_handle},
		{"AT+CCHCLOSE", at_handle}, // TODO: consider response after OK.
		{"AT+CCHSTOP", at_handle},

		// tcp
		{"AT+CIPHEAD", at_handle},
		{"AT+CIPOPEN", tcp_connect_handle},
        {"AT+CIPSEND", ipsend_handle},
        {"AT+CIPCLOSE", at_handle},
		{"AT+CIPSRIP", at_handle}

};

// urc table,
AtcHandleType urc_table[] =
{
	{"START", urc_handle},
	{"CALL READY",  urc_handle},
	{"SMS Ready", urc_handle},
	//{"CLOSED", tcp_closed_handle},
	{"+CPIN: READY", urc_handle},
	{"+STIN:", urc_handle},
	{"RDY", urc_handle},
	{"OPL UPDATING", urc_handle},
	{"PNN UPDATING", urc_handle},
	{"SMS DONE", urc_handle},
	{"PB DONE", urc_handle},
	{"+CHTTPS:RECV EVENT", urc_handle},
	{"+CHTTPSNOTIFY: PEER CLOSED", urc_handle},
	{"+STIN:", urc_handle},
	{"+CCH_PEER_CLOSED:", urc_handle},
	{"+IPCLOSE:", tcp_disconnect_handle}
};


struct module_buf{
	uint8_t* buf;
	uint16_t length;
};

struct at_cmd_entity{
	char at_name[20];
	char at_cmd[128];
};

static int8_t s_module_status = MODULE_OFF_LINE;
static struct FIFO  s_at_fifo;
static uint8_t s_at_status = ATC_RSP_FINISH;
static struct at_cmd_entity *s_current_at_command = NULL;

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
	return 0;
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

static int8_t ate_handle(bool *urc, char* buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR", "ATE"};
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

	    case 2: // ATE
	    {

	    }
	    break;

	    default:
	    break;
	}
	return s_at_status;
}

static int8_t csq_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR", "+CSQ: "};
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
			printf("csq:%s\n", p);
			p = strstr(p, "+CSQ: ");
	    	s_csq_value = atol(p + strlen("+CSQ: "));
			printf("%d\n", s_csq_value);
	    	s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    case 1: // ERROR
	    {
	    	//TODO: error process.
	        s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    case 2:
	    {
			printf("csq:%s", p);
	    	s_csq_value = atol(p + sizeof("+CSQ: "));
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

static int8_t net_open_hanle( bool *urc, char* buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"OK","ERROR", /*"+NETOPEN: 0"*/ "+NETOPEN:"};
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

    	}
    	break;

        case 1: // ERROR
        {
			//TODO: error process.
		   s_module_status = MODULE_GET_IP;
           s_at_status = ATC_RSP_FINISH;
        }
        break;

        case 2:
        {
            s_module_status = MODULE_GET_IP;
            s_at_status  = ATC_RSP_FINISH;
        }
        break;

        default:
        break;
    }

   	return s_at_status;
}

static int8_t cch_open_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"+CCHOPEN", "OK", "ERROR"};
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
		case 0:  // +CCHOPEN
	    {
	    	s_at_status = ATC_RSP_FINISH;
	    }
	    break;
	    case 1: // OK
	    {

	    }
	    break;

	    case 2:
	    {
	    	s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    default:
	    break;
	}
	return s_at_status;
}

static int8_t cch_send_handle(bool *urc, char*buf)
{
	char *rep_str[ ] = {"+CCHRECV", ">", "OK", "ERROR", "+CCHSEND:", "+CCH_PEER_CLOSED:"};
	int8_t res = -1;
	char *p;
	uint8_t  i = 0;
	static  uint16_t s_http_data_len = 0;
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
		case 0:  // +HTTPSRECV:
	    {
	    	// check response length.
	    	p = strstr(p, "+CCHRECV: DATA,1,");
	    	p = p + strlen("+CCHRECV: DATA,1,");
	    	int response_len = atoi(p);
	    	if(response_len>1000)
	    	{
	    		printf("response size is illegal:%d!\n", response_len);
	    		if(s_http_cb != NULL)
	    		{
	    			s_http_cb(NULL);
	    		}
	    		s_at_status = ATC_RSP_FINISH;
	    		return -1;
	    	}
			p = strstr(p, "\r\n");
			p = p + strlen("\r\n");
			memcpy(s_http_buffer + s_http_data_len, p, response_len);
			s_http_data_len += response_len;		
	    }
	    break;
	    case 1: // OK
	    {
			struct fifo_data* http_post_data;
	        http_post_data = fifo_get_data(&s_at_fifo);
	        if(http_post_data != NULL)
	        {
	        	printf("sending.....\n");
	        	usart2_send(http_post_data->buf, http_post_data->length);
	        	if(http_post_data->buf != NULL)
	        	{
	        		myfree(http_post_data->buf);
	        		http_post_data->buf = NULL;
	        	}
	        	myfree(http_post_data);
	        	http_post_data = NULL;
	        }
	    }
	    break;

	    case 2:
	    {

	    }
	    break;

	    case 3:
	    {
			//TODO: consider error situation.
	        s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    case 4:
	    {

	    }
	    break;
			
		case 5:
		{
			// check http protocol.
			char* http_response_buffer = NULL;
			const char * version = "HTTP/1.1 ";
			printf("receive:%s\n", s_http_buffer);
			p = strstr(s_http_buffer, version);
	        if (p == NULL)
	        {
	        	printf("Invalid version in %s\n", p);
	        	if(s_http_cb != NULL)
	        	{
	        		s_http_cb(NULL);
	        	}
	        	s_at_status = ATC_RSP_FINISH;
	        	return -1;
	        }

	        // check http status.
	        int http_status = atoi(p + strlen(version));
	        if(http_status != 200)
	        {
	        	printf("Invalid version in %s\n", p);
	        	if(s_http_cb != NULL)
	        	{
	        		s_http_cb(NULL);
	        	}
	        	s_at_status = ATC_RSP_FINISH;
	        	return -1;
	        }

	        http_response_buffer = (char *)strstr(p, "\r\n\r\n") + 4;
			s_http_data_len = 0;
	        s_at_status = ATC_RSP_FINISH;
	        if(s_http_cb != NULL)
	        {
	        	s_http_cb(http_response_buffer);
	        }
		}
    
		default:
	    break;
	}
	return s_at_status;
}

static int8_t tcp_connect_handle(bool *urc, char*buf)
{
	// TODO: deal with urc
	char *rep_str[ ] = {"+CIPOPEN", "ERROR", "OK"};
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
        break;
    }

   	return s_at_status;
}

int8_t ipsend_handle(bool *urc, char*buf)
{
	// TODO: deal with urc,
	// TODO: free heap when send failed.
	char *rep_str[ ] = {"OK", "ERROR", ">", "+CIPSEND:"};
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
		case 0:  //OK
	    {

	    }
	    break;

	    case 1: //ERROR
	    {
	    	if(tcp_sent_cb != NULL)
	    	{
	    		tcp_sent_cb(0,-1);
	    	}
	    	s_at_status = ATC_RSP_FINISH;
	    }
	    break;

	    case 2: // >
	    {
	    	struct fifo_data* tcp_data_buffer = fifo_get_data(&s_at_fifo);
	    	printf("sending.....,%p,\n", module_send_data_buffer);
	    	if(tcp_data_buffer != NULL)
	    	{
	    		usart2_send(tcp_data_buffer->buf, tcp_data_buffer->length);
	    		if(tcp_data_buffer->buf != NULL)
	    		{
	    			myfree(tcp_data_buffer->buf);
	    			tcp_data_buffer->buf = NULL;
	    		}
	    		myfree(tcp_data_buffer);
	    		tcp_data_buffer = NULL;
	    	}

	    }
	    break;

	    case 3:
	    {
	    	if(module_send_data_buffer != NULL)
	    	{
	    		if(module_send_data_buffer->buffer != NULL)
	    		{
	    			myfree(module_send_data_buffer->buffer);
	    		    module_send_data_buffer->buffer = NULL;
	    		}
	    	}
	    	myfree(module_send_data_buffer);
	    	module_send_data_buffer = NULL;
	    	s_at_status = ATC_RSP_FINISH;
	    	if(tcp_sent_cb != NULL)
	    	{
	    		tcp_sent_cb(0,0);
	    	}
	    }
	    break;

	    default:
	    break;
	 }
	   	return s_at_status;
}

void module_tcp_connect(uint16_t fd, uint32_t ip, uint16_t port)
{
	char connect_buf[80];
	uint8_t ip1, ip2, ip3, ip4;
	ip1 = ((uint8_t*)(&ip))[0];
	ip2 = ((uint8_t*)(&ip))[1];
	ip3 = ((uint8_t*)(&ip))[2];
	ip4 = ((uint8_t*)(&ip))[3];
	add_send_at_command("AT+CIPHEAD", "AT+CIPHEAD=0\r\n");
	add_send_at_command("AT+CIPSRIP", "AT+CIPSRIP=0\r\n");
	sprintf(connect_buf, "AT+CIPOPEN=0,\"TCP\",\"%d.%d.%d.%d\",%d\r\n", ip1, ip2, ip3, ip4, port);
	printf("tcp connect:%s\n", connect_buf);
	add_send_at_command("AT+CIPOPEN", connect_buf);
}

//TODO: close callback.
static int8_t tcp_closed_handle(bool *urc, char *buf)
{
	if(tcp_disconnected_cb != NULL)
	{
		tcp_disconnected_cb(0, -1);
	}
	return -1;
}

static int8_t tcp_disconnect_handle(bool *urc, char *buf)
{
	if(tcp_disconnected_cb != NULL)
	{
		tcp_disconnected_cb(0, -1);
	}
	return -1;
}

void register_module_tcp_connect_callback(uint16_t fd, module_tcp_connected_callback connect_cb)
{
	tcp_connect_cb = connect_cb;
}

void register_module_tcp_sent_callback(uint16_t fd, module_tcp_sent_callback sent_callback)
{
	tcp_sent_cb = sent_callback;
}

void register_module_tcp_recv_callback(uint16_t fd, module_tcp_recv_callback recv_callback)
{
	tcp_recv_cb = recv_callback;
}

void register_module_tcp_disconnected_callback(uint16_t fd, module_tcp_disconnected_callback tcp_disconnected_callback)
{
	tcp_disconnected_cb = tcp_disconnected_callback;
}

uint8_t module_system_start()
{
	s_module_status = MODULE_START;
	timer4_init(1000, 1, at_fifo_check);
	timer4_start();
	return s_module_status;
}

uint8_t module_system_init()
{
	FIFO_Init (&s_at_fifo);

    //ATE0, set no echo.
    add_send_at_command("ATE", "ATE0\r\n");

    //AT+IFC=0, set no flow control.
    add_send_at_command("AT+IFC", "AT+IFC=0\r\n");

    //AT+CPIN, query .....
    add_send_at_command("AT+CPIN", "AT+CPIN?\r\n");

    //AT+CSQ
    add_send_at_command("AT+CSQ", "AT+CSQ\r\n");

	//AT+GSN  request for the IMEI of the module.;
    add_send_at_command("AT+GSN", "AT+GSN\r\n");

	// AT+CGSOCKCONT
    add_send_at_command("AT+CGSOCKCONT", "AT+CGSOCKCONT=1,\"IP\",\"CMNET\"\r\n");

    //AT+CIPMODE
    add_send_at_command("AT+CIPMODE", "AT+CIPMODE=0\r\n");

    //AT+NETOPEN
    add_send_at_command("AT+NETOPEN", "AT+NETOPEN\r\n");

	s_module_status = MODULE_INIT;
	return s_module_status;
}


static void at_fifo_check(void *arg)
{
	static uint8_t wait_response_tick = 0;

	if(s_module_status == MODULE_START)
	{
		send_at_command("AT\r\n");
	}
	else if(MODULE_INIT ==s_module_status|| MODULE_INIT_DONE == s_module_status)
	{
		if ((NULL == s_current_at_command))
		{
			if ((!FIFO_isEmpty(&s_at_fifo)) &&(s_at_status == ATC_RSP_FINISH))
			{
				s_current_at_command = (struct at_cmd_entity*)mymalloc(sizeof(struct at_cmd_entity));
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
void add_send_at_command(char *name_buffer, char *cmd_buffer)
{
	char at_cmd_Buff[256] = {0};
	char at_name_Buff[20] = {0};

	strcpy(at_name_Buff, name_buffer);
	strcpy(at_cmd_Buff, cmd_buffer);

	if(FIFO_Put(&s_at_fifo, at_name_Buff,  at_cmd_Buff) ==-1)
	{
		printf("write fifo error!\n");
	}
}


/******************************************************************************
 * FunctionName : get_module_status
 * Description  : get the status of the module.
 * Parameters   : none
 * Returns      :
*******************************************************************************/
int8_t  get_module_status()
{
	return s_module_status;
}

/******************************************************************************
 * FunctionName : set_module_status
 * Description  : set the status of the module.
 * Parameters   : status: the set status.
 * Returns      :
*******************************************************************************/
void set_module_status(int8_t status)
{
	s_module_status = status;
}

/******************************************************************************
 * FunctionName : module_send_data
 * Description  : module send data api.
 * Parameters   : fd: the send connect index.
 * 				  buf: the send data.
 * 				  len: the send length.
 * Returns      : none.
*******************************************************************************/
void module_send_data(uint16_t fd, uint8_t *buf, uint16_t len)
{
	printf("%s, %d\n", __func__, len);
	struct fifo_data* tcp_data_buffer = (struct fifo_data *)mymalloc(sizeof(struct fifo_data));
	tcp_data_buffer->length = len;
	tcp_data_buffer->buf = (uint8_t*)mymalloc(len);
	memcpy(tcp_data_buffer->buf, buf, len);

	char command_buffer[48];
	sprintf(command_buffer, "AT+CIPSEND=%d,%d\r\n", fd, len);
	printf("%s, 22, %d\n", __func__, len);
	add_send_at_command("AT+CIPSEND", command_buffer);
	fifo_put_data(&s_at_fifo, tcp_data_buffer);
	printf("%s, 33, %d\n", __func__, len);
}

/******************************************************************************
 * FunctionName : inquire_signal_quality
 * Description  : inquire the signal quality.
 * Parameters   : none.
 * Returns      : the signal quality.
 // TODO: the acquired signal quality is not the current value, but the last inquired value.
*******************************************************************************/
uint8_t inquire_signal_quality(void)
{
	add_send_at_command("AT+CSQ", "AT+CSQ\r\n");
	return s_csq_value;
}

void module_http_post(const char *url, const char *data, module_http_callback http_cb)
{
	s_http_cb = http_cb;

	char host_name[64] = "";
	char http_path[64] = "";

	bool is_http  = strncmp(url, "http://",  strlen("http://"))  == 0;
	bool is_https = strncmp(url, "https://", strlen("https://")) == 0;

	if (is_http)
	url += strlen("http://"); // Get rid of the protocol.
	else if (is_https)
	{
		url += strlen("https://"); // Get rid of the protocol.
	}
	else
	{
		printf("URL is not HTTP or HTTPS %s\n", url);
		return;
	}

	char * path = strchr(url, '/');
	if (path == NULL)
	{
		path = strchr(url, '\0'); // Pointer to end of string.
	}

	memcpy(host_name, url, path - url);
	host_name[path - url] = '\0';
	memcpy(http_path, path, strlen(url)- strlen(host_name));

	printf("host_name:%s\n", host_name);
	printf("http_path:%s\n", http_path);

	// AT+CCHSET.
	add_send_at_command("AT+CCHSET", "AT+CCHSET=1\r\n");

	// AT+CCHSTART.
	add_send_at_command("AT+CCHSTART", "AT+CCHSTART\r\n");

	char http_url[64];
	memset(http_url, 0x0, sizeof(http_url));
	sprintf(http_url, "AT+CCHOPEN=%d,\"%s\",%d\r\n", 1, host_name, 443);
	printf("http_para:%s\n", http_url);
	add_send_at_command("AT+CCHOPEN", http_url);

	// AT+CCHSEND
	char post_headers[128] = "";
	char at_send_buffer[20] = "";
	sprintf(post_headers,
				"Content-Type:"
				HTTP_HEADER_CONTENT_TYPE
				"\r\n"
				"Content-Length: %d\r\n", strlen(data));

	struct fifo_data* http_post_data =(struct fifo_data*)mymalloc(sizeof(struct fifo_data));
	http_post_data->buf =(uint8_t*)mymalloc(512);
	memset(http_post_data->buf, 0 , 512);
	int len = sprintf(http_post_data->buf,
						"POST %s HTTP/1.1\r\n"
						"Host: %s:%d\r\n"
						"Connection: close\r\n"
						"User-Agent: SIM5360\r\n"
						"%s"
						"\r\n%s",
						http_path, host_name, 443, post_headers, data);
	http_post_data->length = len;
	printf("http post:%s", http_post_data->buf);
	sprintf(at_send_buffer, "AT+CCHSEND=%d,%d\r\n", 1, len);
	add_send_at_command("AT+CCHSEND", at_send_buffer);
	fifo_put_data(&s_at_fifo, http_post_data);

	// AT+CCHCLOSE
	add_send_at_command("AT+CCHCLOSE", "AT+CCHCLOSE=1\r\n");

	// AT+CCHSTOP
	add_send_at_command("AT+CCHSTOP", "AT+CCHSTOP\r\n");
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

int8_t module_data_handler(void* data)
{

	uint8_t urc = 0;
	struct module_buf* module_data = (struct module_buf*)data;
	printf("module response:%s\n", module_data->buf);
	show_package(module_data->buf, module_data->length);
	if(module_data->length < 3)
	{
		printf("module response not enough length!");
		if(module_data != NULL)
		{
			if(module_data->buf != NULL)
			{
				free(module_data->buf);
				module_data->buf = NULL;
			}
			free(module_data);
			module_data = NULL;
		}
		return -1;
	}

	if(MODULE_START == s_module_status)
	{
		if((strstr(module_data->buf, "OK")))
		{
			printf("the \"at\" cmd response \"OK\"\n");
			s_module_status = MODULE_SYNC;
		}
	}
	if (MODULE_INIT ==s_module_status|| MODULE_INIT_DONE == s_module_status)
	{
		int i = 0;
		if (NULL != s_current_at_command)
	    {
			for (i = 0; i <sizeof(atCmdTable)/sizeof(atCmdTable[0]); i++)
	        {
				if(!strcmp(s_current_at_command->at_name, atCmdTable[i].name))
	            {
					s_at_status =atCmdTable[i].at_cmd_handle(&urc, module_data->buf);

	                if (ATC_RSP_FINISH ==s_at_status)
	                {
	                	myfree(s_current_at_command);
	                    s_current_at_command = NULL;
	                }

	                break;
	            }
	        }
		}
		else
		{
			if(urc_process(module_data->buf) == -1) // not urc.
			{
				printf("%s\n", __func__);
				show_package(module_data->buf, module_data->length);
				if(tcp_recv_cb != NULL)
				{
					tcp_recv_cb(0, module_data->buf, (module_data->length)-1); // -1 for identifier of string end.
				}
			}

		}
	}

	if(module_data != NULL)
	{
		if(module_data->buf != NULL)
		{
			free(module_data->buf);
			module_data->buf = NULL;
		}
		free(module_data);
		module_data = NULL;
	}
	return 0;
}


