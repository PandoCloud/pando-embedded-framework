/*******************************************************
 * File name: pando_net_tcp.c
 * Author:Chongguang Li
 * Versions:0.0.1
 * Description: the tcp api
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/


#include "pando_types.h"
#include "../include/pando_net_tcp.h"
#include "../include/pando_sys.h"
#include "espconn.h"
struct espconn *econn =  NULL;
static net_tcp_connected_callback   temp1 = NULL;
static net_tcp_sent_callback		temp2 = NULL;
static net_tcp_recv_callback		temp3 = NULL;
static net_tcp_disconnected_callback temp4 = NULL;


void ICACHE_FLASH_ATTR temp1_function(void *arg)
{
	uint8 i;
	PRINTF("enter into temp1_function...\n");
	econn = (struct espconn *)arg;
	pd_printf("econn-reverse£º%d\n", econn->reverse);
	struct pando_tcp_conn *pCon = (struct pando_tcp_conn *)pd_malloc(sizeof(struct pando_tcp_conn));
	pd_memset(pCon, 0, sizeof(struct pando_tcp_conn));
	pCon->local_port = econn->proto.tcp->local_port;
	pCon->remote_port = econn->proto.tcp->remote_port;

	for(i=0;i<4;i++)
	{
		pCon->local_ip += ((uint32_t)(econn->proto.tcp->local_ip[i]))<<8*i;
	}
	for(i=0;i<4;i++)
	{
		pCon->remote_ip += ((uint32_t)(econn->proto.tcp->remote_ip[i]))<<8*i;
	}
	pd_printf("pCon->remote_ip: %x\n",pCon->remote_ip);
	pd_printf("eCon remote_ip0: %x,eCon remote_ip1: %x,eCon remote_ip2: %x,eCon remote_ip3: %x\n",
				(econn->proto.tcp->remote_ip)[0],(econn->proto.tcp->remote_ip)[1],
				(econn->proto.tcp->remote_ip)[2],(econn->proto.tcp->remote_ip)[3]);
	pd_printf("econn->reverse: %x\n",econn->reverse);
	pCon->reverse = econn->reverse ;
	temp1(pCon, 0);
}


void ICACHE_FLASH_ATTR temp2_function(void *arg)
{
	PRINTF("enter into temp2_function...");
	temp2(arg, 0);
}

void ICACHE_FLASH_ATTR temp3_function(void *arg, char *pdata, unsigned short len)
{
	PRINTF("enter into temp3_function...");
	struct data_buf data_buffer ;
	data_buffer.data = pdata;
	data_buffer.length = len ;

	temp3(arg,&data_buffer);
}
void ICACHE_FLASH_ATTR temp4_function(void *arg)
{
	PRINTF("enter into temp4_function...");
	temp4(arg, 0);
}



/******************************************************************************
 * FunctionName : net_tcp_connect
 * Description  : The function given as the connect
 * Parameters   : addr: the address used to listen the connection
 * 				  timeout: the connect timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_connect(struct pando_tcp_conn *conn, uint16_t timeout)
{
	sint8 error;
	uint8 i;
	for(i=0;i<4;i++)
	{
		econn->proto.tcp->remote_ip[i] =(uint8)(conn->remote_ip>>8*i) ;
	}
	pd_printf("conn->remote_ip: %x\n",conn->remote_ip);
	pd_printf("econn remote_ip0: %x,econn remote_ip1: %x,econn remote_ip2: %x,econn remote_ip3: %x\n",
			(econn->proto.tcp->remote_ip)[0],(econn->proto.tcp->remote_ip)[1],
			(econn->proto.tcp->remote_ip)[2],(econn->proto.tcp->remote_ip)[3]);

	if(conn->secure==1)
	{
		PRINTF("espconn_secure_connect\n");
		espconn_secure_connect(econn);
	}
	else
	{
		pd_printf("econn remote_port: %d\n",econn->proto.tcp->remote_port);
		pd_printf("econn local_port: %d\n",econn->proto.tcp->local_port);
		PRINTF("espconn_connect\n");
		pd_printf("econn : %d\n",econn);
		error = espconn_connect(econn);
		PRINTF("espconn_connect_error:%d\n",error);

	}
	//pd_free(econn.proto.tcp);
}
void connected_cb1(void *arg)
 {
	 pd_printf("enter into connected_cb1...\n");

 }

/******************************************************************************
 * FunctionName : net_tcp_register_connect_callback
 * Description  : it is used to specify the function that should be called when connected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback  connected_cb)
{
	sint8 error;
	econn =  (struct espconn *)os_malloc(sizeof(struct espconn));
	  pd_memset(econn, 0, sizeof(struct espconn));

	    econn->proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
	    pd_memset(econn->proto.tcp, 0, sizeof(struct _esp_tcp));

		econn->proto.tcp->local_port = conn->local_port ;
		econn->proto.tcp->remote_port = conn->remote_port ;

		econn->reverse = conn->reverse ;

		econn->type = ESPCONN_TCP;
		econn->state = ESPCONN_NONE;

	temp1 = connected_cb;
	error = espconn_regist_connectcb(econn,temp1_function);
	//error = espconn_regist_connectcb(econn,connected_cb1);
	//pd_free(econn.proto.tcp);

}


/******************************************************************************
 * FunctionName : net_tcp_send
 * Description  : The tcp send function.
 * Parameters   : buf: the data buffer to send.
 * 				  timeout: the sent timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_send(struct pando_tcp_conn *conn, struct data_buf buffer, uint16_t timeout)
{
	sint8 error;
	uint8 i;
	//econn->type = ESPCONN_TCP;
	//econn->state = ESPCONN_NONE;
	econn->proto.tcp->local_port = conn->local_port ;
	econn->proto.tcp->remote_port = conn->remote_port ;

		for(i=0;i<4;i++)
		{
			econn->proto.tcp->remote_ip[i] =(uint8)(conn->remote_ip>>8*i) ;
		}
		for(i=0;i<4;i++)
		{
			econn->proto.tcp->local_ip[i] =(uint8)(conn->local_ip>>8*i) ;
		}


	econn->reverse = conn->reverse ;
	pd_printf("length:%d\n",buffer.length);
	pd_printf("con_type:%d\n",econn->type);
	pd_printf("con_state:%d\n",econn->state);
	pd_printf("con_remote_port:%d\n",econn->proto.tcp->remote_port);
	pd_printf("con_local_port:%d\n",econn->proto.tcp->local_port);

	pd_printf("econn remote_ip0: %x,econn remote_ip1: %x,econn remote_ip2: %x,econn remote_ip3: %x\n",
			(econn->proto.tcp->remote_ip)[0],(econn->proto.tcp->remote_ip)[1],
			(econn->proto.tcp->remote_ip)[2],(econn->proto.tcp->remote_ip)[3]);

	error = espconn_send(econn,buffer.data,buffer.length);

	pd_printf("error:%d\n",error);

}


/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : it is used to specify the function that should be called when sent.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb)
{
	pd_printf("enter into net_tcp_register_sent_callback...\n ");
	econn->reverse = conn->reverse ;
	temp2 = sent_cb;
	espconn_regist_sentcb(econn,temp2_function);
	//pd_free(econn.proto.tcp);
}

/******************************************************************************
 * FunctionName : net_tcp_register_recv_callback
 * Description  : it is used to specify the function that should be called when received.
 * Parameters   : recv_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_recv_callback(struct pando_tcp_conn *conn, net_tcp_recv_callback recv_cb)
{
	pd_printf("enter into net_tcp_register_recv_callback...\n ");
	econn->reverse = conn->reverse ;
	espconn_regist_recvcb(econn,temp3_function);
	//pd_free(econn.proto.tcp);
}

/******************************************************************************
 * FunctionName : net_tcp_disconnect
 * Description  : it is used to disconnect the connect.
 * Parameters   : none.
 * Returns      : none
*******************************************************************************/
void net_tcp_disconnect(struct pando_tcp_conn *conn)
{

	espconn_disconnect(econn);
	//pd_free(econn.proto.tcp);
}

/******************************************************************************
 * FunctionName : net_tcp_register_disconnected_callback
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_disconnected_callback(struct pando_tcp_conn *conn, net_tcp_disconnected_callback disconnected_cb)
{
	pd_printf("enter into net_tcp_register_disconnected_callback...\n ");
	econn->reverse = conn->reverse ;
	temp4 = disconnected_cb;
	espconn_regist_disconcb(econn,temp4_function);
	//pd_free(econn.proto.tcp);
}

/******************************************************************************
 * FunctionName : net_tcp_seriver_listen
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : addr: the listen addr.
 * Returns      : the listen result.
*******************************************************************************/
int8_t net_tcp_server_listen(struct pando_tcp_conn *conn)
{
	;
}

/******************************************************************************
 * FunctionName : net_tcp_server_accept
 * Description  : accept the connect.
 * Parameters   : conn: the accept parameter.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_accept(struct pando_tcp_conn *conn)
{
	;
}


