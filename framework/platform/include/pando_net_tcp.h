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

struct net_tcp_addr {
    char ip[16];
    uint16 port;
    bool security;
};

typedef void (* net_connected_callback)(sint8_t errno);
typedef void (* net_sent_callback)(sint8_t errno);
typedef void (* net_recv_callback)(struct sys_buf);
typedef void (* net_disconnected_callback)(sint8_t errno);
typedef void (* net_server_recv_callback)(int fd, struct sy_buf);
typedef void (* net_accepted_callback)(void* arg);
typedef void (* net_server_sent_callback)(sint8_t fd, sint8_t errno);
typedef void (* net_server_disconnected_callback)(sint8_t fd, sint8_t errno);

/******************************************************************************
 * FunctionName : net_tcp_connect
 * Description  : The function given as the connect
 * Parameters   : addr: the address used to listen the connection
 * 				  timeout: the connect timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_connect(struct net_tcp_addr addr, uint16 timeout);

/******************************************************************************
 * FunctionName : net_tcp_register_connect_callback
 * Description  : it is used to specify the function that should be called when connected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_connected_callback(net_connected_callback connected_cb);


/******************************************************************************
 * FunctionName : net_tcp_send
 * Description  : The tcp send function.
 * Parameters   : buf: the data buffer to send.
 * 				  timeout: the sent timeout set value.
 * Returns      : none
*******************************************************************************/
void net_tcp_send(struct sys_buf buf, uint16 timeout);


/******************************************************************************
 * FunctionName : net_tcp_register_sent_callback
 * Description  : it is used to specify the function that should be called when sent.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_sent_callback(net_sent_callback sent_cb);

/******************************************************************************
 * FunctionName : net_tcp_register_recv_callback
 * Description  : it is used to specify the function that should be called when received.
 * Parameters   : recv_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_recv_callback(net_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : net_tcp_disconnect
 * Description  : it is used to disconnect the connect.
 * Parameters   : none.
 * Returns      : none
*******************************************************************************/
void net_tcp_disconnect();

/******************************************************************************
 * FunctionName : net_tcp_register_disconnected_callback
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : connected_cb: the specify function.
 * Returns      : none
*******************************************************************************/
void net_tcp_register_disconnected_callback(net_disconnected_callback disconnected_cb);

/******************************************************************************
 * FunctionName : net_tcp_seriver_listen
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : addr: the listen addr.
 * Returns      : the listen result.
*******************************************************************************/
sint8_t net_tcp_server_listen(struct net_tcp_addr addr);

/******************************************************************************
 * FunctionName : net_tcp_seriver_register_accepted_callback
 * Description  : it is used to specify the function that should be called when connect accepted.
 * Parameters   : accept_cb: the specify function.
 * Returns      : none.
*******************************************************************************/
void net_tcp_server_register_accepted_callback(net_accepted_callback accept_cb);

/******************************************************************************
 * FunctionName : net_tcp_seriver_register_recv_callback
 * Description  : it is used to specify the function that should be called when received package.
 * Parameters   : recv_cb: the specify function.
 * Returns      : the listen result.
*******************************************************************************/
void net_tcp_seriver_register_recv_callback(net_server_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : net_tcp_seriver_send
 * Description  : The tcp send function.
 * Parameters   : fd: the specify descriptor.
 * 				  buf: the send buffer.
 * Returns      : none
*******************************************************************************/
void net_tcp_seriver_send(int fd, struct sys_buf buf);


/******************************************************************************
 * FunctionName : net_tcp_server_disconnect
 * Description  : it is used to disconnect specify connect.
 * Parameters   : fd: the specify connect descriptor.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_disconnect(int fd);

/******************************************************************************
 * FunctionName : net_tcp_server_register_disconnected_callback.
 * Description  : it is used to specify the function that should be called when disconnected.
 * Parameters   : fd: the specify connect descriptor.
 * Returns      : none
*******************************************************************************/
void net_tcp_server_register_disconnected_callback(net_server_disconnected_callback disconnected_cb);

#endif /* _PANDO_NET_TCP_H_ */
