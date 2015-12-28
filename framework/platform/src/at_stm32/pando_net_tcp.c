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
#include "pando_net_tcp.h"
#include "sim5360.h"
#include "pando_sys.h"
#include "common_functions.h"

#define MAX_CONNECT_NUM 10

static struct pando_tcp_conn *conn_arry[MAX_CONNECT_NUM] ={NULL, NULL, NULL, NULL,NULL, NULL,NULL, NULL,NULL, NULL};

static void tcp_connected_callback(int16_t fd, int8_t error_no)
{
	printf("%s,debug:net connect callback\n", __func__);
	int8_t ret = error_no;
	if(fd == -1)
	{
		printf("module callback error!\n");
	}
	else
	{
		if(conn_arry[fd] != NULL)
		{
			if(conn_arry[fd]->connected_callback != NULL)
			{
				conn_arry[fd]->connected_callback(conn_arry[fd], ret);
			}
		}
	}
}

static void tcp_sent_callback(int16_t fd, int8_t error_no)
{
	printf("%s,debug:net_tcp ok\n", __func__);
	int8_t ret = error_no;
	if(fd == -1)
	{
		printf("module callback error!\n");
	}
	else
	{
		if(conn_arry[fd] != NULL)
		{
		    if(conn_arry[fd]->sent_callback != NULL)
		    {
		    	conn_arry[fd]->sent_callback(conn_arry[fd], ret);
		    }
		}
	}
}

static void tcp_recv_callback(int16_t fd, uint8_t* data, uint16_t length)
{
	struct data_buf buf;
	buf.data = data;
	buf.length = length;
	//printf("%s,debug:net recv ok\n", __func__);
	if(fd == -1)
	{
		printf("module recv callback error!\n");
	}
	else
	{
		//printf("%s, 10\n", __func__);
		if(conn_arry[fd] != NULL)
		{
			//printf("%s, 20\n", __func__);
			if(conn_arry[fd]->recv_callback != NULL)
			{
				//printf("%s, 30\n", __func__);
				conn_arry[fd]->recv_callback(conn_arry[fd], &buf);
			}
			//printf("%s, 40\n", __func__);
		}
	}
}

static void tcp_disconnected_callback(int16_t fd, int8_t error_no)
{
	printf("%s,debug:net disconect ok\n", __func__);
	if(fd == -1)
	{
		printf("module disconnect callback error!\n");
	}
	else
	{
		if(conn_arry[fd] != NULL)
		{
			if(conn_arry[fd]->disconnected_callback != NULL)
			{
				conn_arry[fd]->disconnected_callback(conn_arry[fd], error_no);
			}
		}
	}
	
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
	uint32_t ip = conn->remote_ip;
	uint16_t port = conn->remote_port;
	int i = 0;
	for(i = 0; i < MAX_CONNECT_NUM; i++)
	{
		if(conn_arry[i] == NULL)
		{
			conn->fd = i;
			conn_arry[i] = conn;
			break;
		}
	}

	module_tcp_connect(conn->fd, ip, port);
	/*//AT+CIPSTART
	char connect_buf[80];
	uint8_t ip1, ip2, ip3, ip4;
	ip1 = ((uint8_t*)(&ip))[0];
	ip2 = ((uint8_t*)(&ip))[1];
	ip3 = ((uint8_t*)(&ip))[2];
	ip4 = ((uint8_t*)(&ip))[3];

	sprintf(connect_buf, "AT+CIPSTART=\"TCP\",\"%d.%d.%d.%d\",\"%d\"\r\n", ip1, ip2, ip3, ip4, port);
	printf("tcp connect:%s\n", connect_buf);
	add_send_at_command("AT+CIPSTART", connect_buf);*/
}

/******************************************************************************
 * FunctionName : net_tcp_register_connect_callback
 * Description  : it is used to specify the function that should be called when connected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback connected_cb)
{
	conn->connected_callback = connected_cb;
	register_module_tcp_connect_callback(conn->fd, tcp_connected_callback);
}

/******************************************************************************
 * FunctionName : net_tcp_send
 * Description  : The tcp send function.
 * Parameters   : buf: the data buffer to send.
 * 				  timeout: the sent timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_send(struct pando_tcp_conn *conn, struct data_buf buffer,  uint16_t timeout)
{
	pd_printf("%s,tcp send\n", __func__);
	show_package(buffer.data, buffer.length);
	module_send_data(0, buffer.data, buffer.length);
}


/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : it is used to specify the function that should be called when sent.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb)
{
	conn->sent_callback = sent_cb;
	register_module_tcp_sent_callback(0, tcp_sent_callback);
}

/******************************************************************************
 * FunctionName : net_tcp_register_recv_callback
 * Description  : it is used to specify the function that should be called when received.
 * Parameters   : recv_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_recv_callback(struct pando_tcp_conn *conn, net_tcp_recv_callback recv_cb)
{
	conn->recv_callback = recv_cb;
	register_module_tcp_recv_callback(0, tcp_recv_callback);
}

/******************************************************************************
 * FunctionName : net_tcp_disconnect
 * Description  : it is used to disconnect the connect.
 * Parameters   : none.
 * Returns      : none
*******************************************************************************/
void net_tcp_disconnect(struct pando_tcp_conn *conn)
{
	int i= 0;
	for( i = 0; i < MAX_CONNECT_NUM; i++)
	{
		if(conn->fd == conn_arry[i]->fd)
		{
			conn_arry[i] = NULL;
			break;
		}
	}

	//clear_module();
	//add_send_at_command("AT+CIPCLOSE", "AT+CIPCLOSE\r\n");
}

/******************************************************************************
 * FunctionName : net_tcp_register_disconnected_callback
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_disconnected_callback(struct pando_tcp_conn *conn, \
	net_tcp_disconnected_callback disconnected_cb)
{
	conn->disconnected_callback = disconnected_cb;
	register_module_tcp_disconnected_callback(0, tcp_disconnected_callback);
}

/******************************************************************************
 * FunctionName : net_tcp_seriver_listen
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : addr: the listen addr.
 * Returns      : the listen result.
*******************************************************************************/
int8_t net_tcp_server_listen(struct pando_tcp_conn *conn)
{
}

/******************************************************************************
 * FunctionName : net_tcp_server_accept
 * Description  : accept the connect.
 * Parameters   : conn: the accept parameter.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_accept(struct pando_tcp_conn *conn)
{
	
}
