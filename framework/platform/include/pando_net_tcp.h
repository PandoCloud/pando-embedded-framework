/*******************************************************
 * File name: pando_net_tcp.h
 * Author:Chongguang Li
 * Versions:0.0.1
 * Description: the tcp api
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#ifndef _PANDO_NET_TCP_H_
#define _PANDO_NET_TCP_H_

#include "pando_types.h"


struct data_buf
{
    uint16_t length;
    char *data;
};

typedef void (* net_tcp_connected_callback)(void *tcp_conn, int8_t errno);
typedef void (* net_tcp_sent_callback)(void *tcp_conn, int8_t errno);
typedef void (* net_tcp_recv_callback)(void *tcp_conn, struct data_buf *buffer);
typedef void (* net_tcp_disconnected_callback)(void *tcp_conn, int8_t errno);

struct pando_tcp_conn {
	uint8_t secure;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint32_t local_ip;
	uint16_t local_port;
	uint16_t fd;
	net_tcp_connected_callback connected_callback;
	net_tcp_recv_callback recv_callback;
	net_tcp_sent_callback sent_callback;
	net_tcp_disconnected_callback disconnected_callback;
    void *reverse;
};

/******************************************************************************
 * FunctionName : net_tcp_connect
 * Description  : The function given as the connect
 * Parameters   : addr: the address used to listen the connection
 * 				  timeout: the connect timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_connect(struct pando_tcp_conn *conn, uint16_t timeout);

/******************************************************************************
 * FunctionName : net_tcp_register_connect_callback
 * Description  : it is used to specify the function that should be called when connected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback  connected_cb);


/******************************************************************************
 * FunctionName : net_tcp_send
 * Description  : The tcp send function.
 * Parameters   : buf: the data buffer to send.
 * 				  timeout: the sent timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_send(struct pando_tcp_conn *conn, struct data_buf buffer, uint16_t timeout);


/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : it is used to specify the function that should be called when sent.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb);

/******************************************************************************
 * FunctionName : net_tcp_register_recv_callback
 * Description  : it is used to specify the function that should be called when received.
 * Parameters   : recv_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_recv_callback(struct pando_tcp_conn *conn, net_tcp_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : net_tcp_disconnect
 * Description  : it is used to disconnect the connect.
 * Parameters   : none.
 * Returns      : none
*******************************************************************************/
void net_tcp_disconnect(struct pando_tcp_conn *conn);

/******************************************************************************
 * FunctionName : net_tcp_register_disconnected_callback
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_disconnected_callback(struct pando_tcp_conn *conn, net_tcp_disconnected_callback disconnected_cb);

/******************************************************************************
 * FunctionName : net_tcp_seriver_listen
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : addr: the listen addr.
 * Returns      : the listen result.
*******************************************************************************/
int8_t net_tcp_server_listen(struct pando_tcp_conn *conn);

/******************************************************************************
 * FunctionName : net_tcp_server_accept
 * Description  : accept the connect.
 * Parameters   : conn: the accept parameter.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_accept(struct pando_tcp_conn *conn);

#endif /* _PANDO_NET_TCP_H_ */
