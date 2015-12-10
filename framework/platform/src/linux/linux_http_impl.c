#include "platform/include/pando_net_http.h"

#include <openssl/err.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define TCP_DEFAULT_PORT "80"
#define TCP_SSL_DEFAULT_PORT "443"

#define HTTP_BUF_LEN 1024

SSL_CTX *p_g_ctx = NULL;

SSL *p_g_ssl = NULL;

//define the struct http related info.
struct pd_http_info {
    char path[50];
    char port[10];
    char host[50];
    char isHttps; //indicate isHttps or not.
    int socketfd;
};

int strstrpos(const char* oristr, const char* searchstr)
{
    char *pos = strstr(oristr, searchstr);
    if(pos == NULL)
    {
        return -1;
    }

    return pos - oristr;
}

HTTP_RET parse_url(const char* url, struct pd_http_info *p_http_info);

HTTP_RET create_tcp_socket(struct pd_http_info *p_http_info);

void net_http_post(const char* url, const char* data, net_http_callback http_cb)
{
    struct pd_http_info *p_http_info;
    if(HTTP_OK != parse_url(url, p_http_info))
    {
        if(NULL != http_cb)
        {
            http_cb(HTTP_URL_ERR, NULL);
        }

        return;
    }

    HTTP_RET ret = create_tcp_socket(p_http_info);
    if(HTTP_OK != ret)
    {
        if(NULL != http_cb)
        {
            http_cb(ret, NULL);
        }

        return;
    }

    char temp[10] = {0};
    sprintf(temp, "%d", strlen(data));
    char message[100];
    sprintf(message, "POST %s HTTP/1.1\nhost: %s\nConnection: keep-alive\nContent-Length: %s\nUser-Agent: Linux\nContent-Type: application/json\nAccept: */*\n\n%s", 
        p_http_info->path, p_http_info->host, temp, data);
    if(0 == p_http_info->isHttps)
    {
        ssize_t bytes_sent;
        bytes_sent = send(p_http_info->socketfd, message, strlen(message), 0);
    }
    else //https request
    {
        SSL_library_init();
        SSL_load_error_strings();
        if(NULL != p_g_ctx)
        {
            SSL_CTX_free(p_g_ctx);
        }

        if(NULL != p_g_ssl)
        {
            if(SSL_shutdown(p_g_ssl) != 1)
            {
                printf("SSL_shutdown failed.\n");
            }

            SSL_free(p_g_ssl);
        }

        p_g_ctx = SSL_CTX_new(SSLv23_client_method());
        if(NULL == p_g_ctx)
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_CTX_ERR, NULL);
            }

            return;
        }

        p_g_ssl = SSL_new(p_g_ctx);
        if(NULL == p_g_ssl)
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_NEW_ERR, NULL);
            }

            return;
        }

        if(0 == SSL_set_fd(p_g_ssl, p_http_info->socketfd))
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_FD_ERR, NULL);
            }

            return;
        }

        if(1 != SSL_connect(p_g_ssl))
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_CONNECT_ERR, NULL);
            }

            return;
        }

        SSL_write(p_g_ssl, message, strlen(message));
    }

    if(NULL != http_cb)
    {
        http_cb(HTTP_OK, NULL);
    }

    return;
}

void net_http_get(const char* url, net_http_callback http_cb)
{
    struct pd_http_info *p_http_info;
    if(HTTP_OK != parse_url(url, p_http_info))
    {
        if(NULL != http_cb)
        {
            http_cb(HTTP_URL_ERR, NULL);
        }

        return;
    }

    HTTP_RET ret = create_tcp_socket(p_http_info);
    if(HTTP_OK != ret)
    {
        if(NULL != http_cb)
        {
            http_cb(ret, NULL);
        }

        return;
    }

    ssize_t bytes_recieved = 0;
    char buf[HTTP_BUF_LEN];
    memset(buf, 0, HTTP_BUF_LEN);
    if(0 == p_http_info->isHttps)
    {
        bytes_recieved = recv(p_http_info->socketfd, buf, HTTP_BUF_LEN, 0);
        if(0 == bytes_recieved)
        {
            if(http_cb != NULL)
            {
                http_cb(HTTP_RECV_NO_DATA, NULL);
            }

            return;
        }

        if(bytes_recieved < 0)
        {
            if(http_cb != NULL)
            {
                http_cb(HTTP_RECV_ERR, NULL);
            }

            return;
        }

        buf[bytes_recieved] = '\0';
    }
    else
    {
        SSL_library_init();
        SSL_load_error_strings();
        if(NULL != p_g_ctx)
        {
            SSL_CTX_free(p_g_ctx);
        }

        if(NULL != p_g_ssl)
        {
            if(SSL_shutdown(p_g_ssl) != 1)
            {
                printf("SSL_shutdown failed.\n");
            }

            SSL_free(p_g_ssl);
        }
        
        p_g_ctx = SSL_CTX_new(SSLv23_client_method());
        if(NULL == p_g_ctx)
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_CTX_ERR, NULL);
            }

            return;
        }

        p_g_ssl = SSL_new(p_g_ctx);
        if(NULL == p_g_ssl)
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_NEW_ERR, NULL);
            }

            return;
        }

        if(0 == SSL_set_fd(p_g_ssl, p_http_info->socketfd))
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_FD_ERR, NULL);
            }

            return;
        }

        if(1 != SSL_connect(p_g_ssl))
        {
            if(NULL != http_cb)
            {
                http_cb(HTTP_SSL_CONNECT_ERR, NULL);
            }

            return;
        }

        bytes_recieved = SSL_read(p_g_ssl, buf, HTTP_BUF_LEN - 1);
        if(bytes_recieved < 0)
        {
            if(http_cb != NULL)
            {
                http_cb(HTTP_RECV_ERR, 0);
            }

            return;
        }
        else
        {
            buf[bytes_recieved] = '\0';
        }

        if(0 == strlen(buf))
        {
            if(http_cb != NULL)
            {
                http_cb(HTTP_RECV_NO_DATA, NULL);
            }

            return;
        }
    }

    int pos = strstrpos(buf, "HTTP/1.1 200");
    if(pos < 0)
    {
        if(http_cb != NULL)
        {
            http_cb(HTTP_RECV_HTTP_ERR, NULL);
        }

        return;
    }

    pos = strstrpos(buf, "Content-Length:");
    char result[HTTP_BUF_LEN] = {0};
    strcpy(result, buf + pos);
    pos = strstrpos(result, "\n");
    char response[HTTP_BUF_LEN];
    strcpy(response, result + pos + 1);
    if(http_cb != NULL)
    {
        http_cb(HTTP_OK, response);
    }

    return;
}

HTTP_RET parse_url(const char* url, struct pd_http_info *p_http_info)
{
    int pos = strstrpos(url, "://");
    char protocol[20];
    char url_without_head[50];
    strcpy(url_without_head, url);
    if(pos == -1)
    {
        strcpy(protocol, "http");
        pos = 0;
    }
    else
    {
        strncpy(protocol, url, pos);
        strcpy(url_without_head, url + pos + 3); //remove "://"
    }

    int host_end_pos = strstrpos(url_without_head, "/");
    int port_end_pos = strstrpos(url_without_head, ":");
    if(host_end_pos == -1)
    {
        strcpy(p_http_info->path, "/");
    }
    else
    {
        strcpy(p_http_info->path, url_without_head);
    }

    if(strcmp(protocol, "https") == 0)
    {
        p_http_info->isHttps = 1;
    }
    else
    {
        p_http_info->isHttps = 0;
    }

    //set default port
    if(port_end_pos == -1)
    {
        if(p_http_info->isHttps == 1)
        {
            strcpy(p_http_info->port, TCP_SSL_DEFAULT_PORT);
        }
        else
        {
            strcpy(p_http_info->port, TCP_DEFAULT_PORT);
        }

        if(host_end_pos == -1)
        {
            strcpy(p_http_info->host, url_without_head);
        }
        else
        {
            strncpy(p_http_info->host, url_without_head, host_end_pos);
        }
    }
    else
    {
        int end_pos = host_end_pos;
        if(host_end_pos == -1)
        {
            end_pos = strlen(url_without_head);
        }

        strncpy(p_http_info->port, url_without_head + port_end_pos + 1, strlen(url_without_head) - (end_pos + 1));
        strncpy(p_http_info->host, url_without_head, port_end_pos);
    }

    return HTTP_OK;
}

HTTP_RET create_tcp_socket(struct pd_http_info *p_http_info)
{
    struct addrinfo host_info;
    struct addrinfo *p_host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC; //both IPv4 & IPv6
    host_info.ai_socktype = SOCK_STREAM; //for TCP
    if(getaddrinfo(p_http_info->host, p_http_info->port, &host_info, &p_host_info_list) < 0)
    {
        return HTTP_HOST_ERR;
    }

    p_http_info->socketfd = socket(p_host_info_list->ai_family, p_host_info_list->ai_socktype, p_host_info_list->ai_protocol);
    if(p_http_info->socketfd < 0)
    {
        return HTTP_SOCKET_ERR;
    }

    if(connect(p_http_info->socketfd, p_host_info_list->ai_addr, p_host_info_list->ai_addrlen) < 0)
    {
        return HTTP_CONNECT_ERR;
    }

    freeaddrinfo(p_host_info_list);
    return HTTP_OK;
}
