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

static net_tcp_connected_callback   temp1 = NULL;
static net_tcp_sent_callback		temp2 = NULL;
static net_tcp_recv_callback		temp3 = NULL;
static net_tcp_disconnected_callback temp4 = NULL;


void temp1_function(void *tcp_conn)
{
	PRINTF("enter into temp1_function...");
	temp1(tcp_conn, 0);
}

void temp2_function(void *tcp_conn)
{
	PRINTF("enter into temp2_function...");
	temp2(tcp_conn, 0);
}

void temp3_function(void *tcp_conn, char *pdata, unsigned short len)
{
	PRINTF("enter into temp3_function...");
	struct data_buf data_buffer ;
	data_buffer.data = pdata;
	data_buffer.length = len ;

	temp3(tcp_conn,&data_buffer);
}
void temp4_function(void *tcp_conn)
{
	PRINTF("enter into temp4_function...");
	temp4(tcp_conn, 0);
}


struct espconn econn ;
/******************************************************************************
 * FunctionName : net_tcp_connect
 * Description  : The function given as the connect
 * Parameters   : addr: the address used to listen the connection
 * 				  timeout: the connect timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_connect(struct pando_tcp_conn *conn, uint16_t timeout)
{
	 econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

			pd_memcpy(&(econn.proto.tcp->local_port),&(conn->local_port),sizeof(uint32_t));
			pd_memcpy(&(econn.proto.tcp->remote_port),&(conn->remote_port),sizeof(uint32_t));
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	if(conn->secure==1)
	{
		PRINTF("espconn_secure_connect\n");
		espconn_secure_connect(&econn);
	}
	else
	{
		PRINTF("espconn_connect\n");
		espconn_connect(&econn);
	}
	//pd_free(econn.proto.tcp);
}

/******************************************************************************
 * FunctionName : net_tcp_register_connect_callback
 * Description  : it is used to specify the function that should be called when connected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback connected_cb)
{
	    econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
	    pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

		conn->local_port = espconn_port();


		econn.proto.tcp->local_port = conn->local_port ;
		econn.proto.tcp->remote_port = conn->remote_port ;

		econn.reverse = conn->reverse ;

		econn.type = ESPCONN_TCP;
		econn.state = ESPCONN_NONE;


		pd_printf("econn local_port: %d\n",econn.proto.tcp->local_port);

		pd_printf("econn remote_port: %d\n",econn.proto.tcp->remote_port);


	temp1 = connected_cb;
	espconn_regist_connectcb(&econn,temp1_function);

	pd_printf("net_tcp_register_connected_callback END...\n");
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
	econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

			econn.proto.tcp->local_port = conn->local_port ;
					econn.proto.tcp->remote_port = conn->remote_port ;
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	espconn_secure_send(&econn,buffer.data,buffer.length);
	//pd_free(econn.proto.tcp);
}


/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : it is used to specify the function that should be called when sent.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb)
{
	econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));
			econn.proto.tcp->local_port = conn->local_port ;
					econn.proto.tcp->remote_port = conn->remote_port ;
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	temp2 = sent_cb;
	espconn_regist_sentcb(&econn,temp2_function);
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
	econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

			econn.proto.tcp->local_port = conn->local_port ;
					econn.proto.tcp->remote_port = conn->remote_port ;
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	temp3 = recv_cb;
	espconn_regist_recvcb(&econn,temp3_function);
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
	econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

			econn.proto.tcp->local_port = conn->local_port ;
					econn.proto.tcp->remote_port = conn->remote_port ;
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	espconn_secure_disconnect(&econn);
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
	econn.proto.tcp = (esp_tcp *)pd_malloc(sizeof(struct _esp_tcp));
			conn->local_port = espconn_port();

			pd_memset(econn.proto.tcp, 0, sizeof(struct _esp_tcp));

			econn.proto.tcp->local_port = conn->local_port ;
					econn.proto.tcp->remote_port = conn->remote_port ;
			econn.reverse = conn->reverse ;

			econn.type = ESPCONN_TCP;
			econn.state = ESPCONN_NONE;

	temp4 = disconnected_cb;
	espconn_regist_disconcb(&econn,temp4_function);
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


