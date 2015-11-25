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

enum NET_TCP_ERR_NO {
    NET_TCP_RET_OK = 0,
    NET_TCP_CONNECT_ERROR = 1,
    NET_TCP_SEND_ERROR = 2,
    NET_TCP_DISCONNECT_ERROR = 3,
};

//define the struct to include the buffer data and the length.
struct data_buf
{
    uint16_t length;
    char *data;
};

typedef void (* net_tcp_connected_callback)(void *tcp_conn, int8_t errno);
typedef void (* net_tcp_sent_callback)(void *tcp_conn, int8_t errno);
typedef void (* net_tcp_recv_callback)(void *tcp_conn, struct data_buf *buffer);
typedef void (* net_tcp_disconnected_callback)(void *tcp_conn, int8_t errno);

//define the struct to include all the params tcp_connection related.
struct pando_tcp_conn {
    uint32_t remote_ip;
    uint16_t remote_port;
    uint32_t local_ip;
    uint16_t local_port;
    uint16_t fd;
    net_tcp_connected_callback connected_callback;
    net_tcp_recv_callback recv_callback;
    net_tcp_sent_callback sent_callback;
    net_tcp_disconnected_callback disconnected_callback;
    void *reserved;
};

/******************************************************************************
 * FunctionName : net_tcp_connect
 * Description  : to connect via tcp.
 * Parameters   : conn: the params used in the connection.
 *                timeout: the connect timeout.
 * Returns      : none
*******************************************************************************/
void net_tcp_connect(struct pando_tcp_conn *conn, uint16_t timeout);

/******************************************************************************
 * FunctionName : net_tcp_register_connected_callback
 * Description  : register the function that should be called when connected.
 * Parameters   : conn: the connection params.
 *                connected_cb: the callback function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback connected_cb);

/******************************************************************************
 * FunctionName : net_tcp_send
 * Description  : the tcp send function.
 * Parameters   : conn: the connection params.
 *                buffer: the data buffer to send.
 *                timeout: the sent timeout.
 * Returns      : none
*******************************************************************************/
void net_tcp_send(struct pando_tcp_conn *conn, struct data_buf buffer, uint16_t timeout);

/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : register the function that should be called when sent.
 * Parameters   : conn: the connection params.
 *                sent_cb: the callback function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb);

/******************************************************************************
 * FunctionName : net_tcp_register_recv_callback
 * Description  : register the function that should be called when received.
 * Parameters   : conn: the connection params.
 *                recv_cb: the callback function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_recv_callback(struct pando_tcp_conn *conn, net_tcp_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : net_tcp_disconnect
 * Description  : to disconnect the connect.
 * Parameters   : conn: the connection params.
 * Returns      : none
*******************************************************************************/
void net_tcp_disconnect(struct pando_tcp_conn *conn);

/******************************************************************************
 * FunctionName : net_tcp_register_disconnected_callback
 * Description  : register the function that should be called when disconnected.
 * Parameters   : conn: the connection params.
 *                disconnected_cb: the callback function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_disconnected_callback(struct pando_tcp_conn *conn, net_tcp_disconnected_callback disconnected_cb);

/******************************************************************************
 * FunctionName : net_tcp_server_listen
 * Description  : tcp server listen.
 * Parameters   : conn: the connection params.
 * Returns      : the listen result.
*******************************************************************************/
int8_t net_tcp_server_listen(struct pando_tcp_conn *conn);

/******************************************************************************
 * FunctionName : net_tcp_server_accept
 * Description  : accept the connect.
 * Parameters   : conn: the connection parameter.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_accept(struct pando_tcp_conn *conn);

#endif /* _PANDO_NET_TCP_H_ */
