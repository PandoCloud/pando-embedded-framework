#include "framework/platform/include/pando_net_tcp.h"

#include <linux/socket.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netdb.h>


//define the max tcp client count connected.
#define MAX_TCP_CLIENT_SIZE 10

//declare the array to store the tcp client identifiers used.
uint32_t g_tcp_client_identifiers_ip[MAX_TCP_CLIENT_SIZE] = {0};

//declare the array to store the tcp client identifiers used.
//use the ip and port to identify the client uniquely, not use fd because the fd is not generated when registering conn_cb.
uint16_t g_tcp_client_identifiers_port[MAX_TCP_CLIENT_SIZE] = {0};

//declare the connected callback functions. Notes: the callback in the array corresponds with the order in the id array.
net_tcp_connected_callback g_tcp_connected_cbs[MAX_TCP_CLIENT_SIZE] = {NULL};

//declare the sent callback functions. Notes: the callback in the array corresponds with the order in the id array.
net_tcp_sent_callback g_tcp_sent_cbs[MAX_TCP_CLIENT_SIZE] = {NULL};

//declare the receive callback function. Notes: the callback in the array corresponds with the order in the id array.
net_tcp_recv_callback g_tcp_recv_cbs[MAX_TCP_CLIENT_SIZE] = {NULL};

//declare the disconnected callback function. Notes: the callback in the array corresponds with the order in the id array.
net_tcp_disconnected_callback g_tcp_disconnected_cbs[MAX_TCP_CLIENT_SIZE] = {NULL};

void net_tcp_connect(struct pando_tcp_conn *conn, uint16_t timeout)
{
    uint8_t index = 0;
        
    if(NULL == conn)
    {
        return;
    }

    net_tcp_connected_callback connected_cb = NULL;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            connected_cb = g_tcp_connected_cbs[index];
            break;
        }
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        if(NULL != connected_cb)
        {
            connected_cb(conn, NET_TCP_CONNECT_ERROR);
            return;
        }

        return;
    }

    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(conn->remote_ip);
    sock_addr.sin_port = htons(conn->remote_port);
    if(connect(fd, &sock_addr, sizeof(sock_addr)) < 0)
    {
        if(NULL != connected_cb)
        {
            connected_cb(conn, NET_TCP_CONNECT_ERROR);
            return;
        }
    }

    conn->fd = fd;
    if(NULL != connected_cb)
    {
        connected_cb(conn, NET_TCP_RET_OK);
        return;
    }

    return;
}

void net_tcp_register_connected_callback(struct pando_tcp_conn *conn , net_tcp_connected_callback connected_cb)
{
    if(NULL == conn)
    {
        return;
    }

    if(0 == conn->remote_ip && 0 == conn->remote_port)
    {
        return;
    }

    uint8_t index = 0;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            break;
        }

        if(0 == g_tcp_client_identifiers_ip[index] && 0 == g_tcp_client_identifiers_port[index])
        {
            break;
        }
    }

    if(index >= MAX_TCP_CLIENT_SIZE)
    {
        return;
    }

    g_tcp_client_identifiers_ip[index] = conn->remote_ip;
    g_tcp_client_identifiers_port[index] = conn->remote_port;

    g_tcp_connected_cbs[index] = connected_cb;
}

void net_tcp_send(struct pando_tcp_conn *conn, struct data_buf buffer, uint16_t timeout)
{
    uint8_t index = 0;
        
    if(NULL == conn)
    {
        return;
    }

    net_tcp_sent_callback sent_cb = NULL;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            sent_cb = g_tcp_sent_cbs[index];
            break;
        }
    }

    if(conn->fd <= 0)
    {
        if(NULL == sent_cb)
        {
            sent_cb(conn, NET_TCP_SEND_ERROR);
            return;
        }

        return;
    }

    if(send(conn->fd, buffer.data, buffer.length, 0) < 0)
    {
        if(NULL != sent_cb)
        {
            sent_cb(conn, NET_TCP_SEND_ERROR);
            return;
        }

        return;
    }

    if(NULL != sent_cb)
    {
        sent_cb(conn, NET_TCP_RET_OK);
        return;
    }

    return;
}

void net_tcp_register_sent_callback(struct pando_tcp_conn *conn, net_tcp_sent_callback sent_cb)
{
    if(NULL == conn)
    {
        return;
    }

    if(0 == conn->remote_ip && 0 == conn->remote_port)
    {
        return;
    }

    uint8_t index = 0;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            break;
        }

        if(0 == g_tcp_client_identifiers_ip[index] && 0 == g_tcp_client_identifiers_port[index])
        {
            break;
        }
    }

    if(index >= MAX_TCP_CLIENT_SIZE)
    {
        return;
    }

    g_tcp_client_identifiers_ip[MAX_TCP_CLIENT_SIZE] = conn->remote_ip;
    g_tcp_client_identifiers_port[MAX_TCP_CLIENT_SIZE] = conn->remote_port;
    g_tcp_sent_cbs[index] = sent_cb;
}

void net_tcp_register_recv_callback(struct pando_tcp_conn *conn, net_tcp_recv_callback recv_cb)
{
    if(NULL == conn)
    {
        return;
    }

    if(0 == conn->remote_ip && 0 == conn->remote_port)
    {
        return;
    }

    uint8_t index = 0;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            break;
        }

        if(0 == g_tcp_client_identifiers_ip[index] && 0 == g_tcp_client_identifiers_port[index])
        {
            break;
        }
    }

    if(index >= MAX_TCP_CLIENT_SIZE)
    {
        return;
    }

    g_tcp_client_identifiers_ip[index] = conn->remote_ip;
    g_tcp_client_identifiers_port[index] = conn->remote_port;

    g_tcp_recv_cbs[index] = recv_cb;
}

void net_tcp_disconnect(struct pando_tcp_conn *conn)
{
    uint8_t index = 0;
    
    if(NULL == conn)
    {
        return;
    }

    net_tcp_disconnected_callback disconnected_cb = NULL;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            disconnected_cb = g_tcp_disconnected_cbs[index];
            break;
        }
    }

    if(conn->fd <= 0)
    {
        if(NULL != disconnected_cb)
        {
            disconnected_cb(conn, NET_TCP_DISCONNECT_ERROR);
            return;
        }

        return;
    }

    if(close(conn->fd) < 0)
    {
        if(NULL != disconnected_cb)
        {
            disconnected_cb(conn, NET_TCP_DISCONNECT_ERROR);
            return;
        }
    }

    if(NULL != disconnected_cb)
    {
        disconnected_cb(conn, NET_TCP_RET_OK);
        return;
    }

    return;
}

void net_tcp_register_disconnected_callback(struct pando_tcp_conn *conn, net_tcp_disconnected_callback disconnected_cb)
{
    if(NULL == conn)
    {
        return;
    }

    if(0 == conn->remote_ip && 0 == conn->remote_port)
    {
        return;
    }

    uint8_t index = 0;
    for(index = 0; index < MAX_TCP_CLIENT_SIZE; index++)
    {
        if(conn->remote_ip == g_tcp_client_identifiers_ip[index] && conn->remote_port == g_tcp_client_identifiers_port[index])
        {
            break;
        }

        if(0 == g_tcp_client_identifiers_ip[index] && 0 == g_tcp_client_identifiers_port[index])
        {
            break;
        }
    }

    if(index >= MAX_TCP_CLIENT_SIZE)
    {
        return;
    }

    g_tcp_client_identifiers_ip[index] = conn->remote_ip;
    g_tcp_client_identifiers_port[index] = conn->remote_port;

    g_tcp_disconnected_cbs[index] = disconnected_cb;
}

int8_t net_tcp_server_listen(struct pando_tcp_conn *conn)
{
    if(NULL == conn)
    {
        return;
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        return -1;
    }

    conn->fd = listenfd;
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(conn->local_ip);
    server_addr.sin_port = htons(conn->local_port);
    if(bind(listenfd, (struct sock_addr*)&server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    }

    return listen(listenfd, 5);
}

void net_tcp_server_accept(struct pando_tcp_conn *conn)
{
    if(NULL == conn)
    {
        return;
    }

    if(conn->fd < 0)
    {
        return;
    }

    struct sockaddr_in client_addr;
    socklen_t client_len;
    if(accept(conn->fd, (struct sock_addr*)&client_addr, &client_len) < 0)
    {
        return;
    }

    conn->remote_ip = ntohl(client_addr.sin_addr.s_addr);
    conn->remote_port = ntohs(client_addr.sin_port);
    return;
}
