/*******************************************************
 * File name: pando_net_http.c
 * Author:Chongguang Li
 * Versions:0.0.1
 * Description: the http api
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/


#include "../../include/pando_net_http.h"
#include "../../include/pando_types.h"
#include "../../include/pando_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "../../../lib/cert.h"
#include "../../../lib/private_key.h"

typedef enum {
    DNS_FAIL = 0,
	HTTP_TIMEOUT,
    HTTP_ERR,
    HTTP_OK,
} HTTP_RESULT;

// Suport different Content-Type header
#define HTTP_HEADER_CONTENT_TYPE "application/json"

typedef struct{
	char* buffer;
	int buffer_size;
} HTTP_BUF;


static char* http_path = NULL ;

static char* http_post_data = NULL;

static char* http_hostname = NULL;

static int http_port = 0;

static struct espconn* conn = NULL;

static http_callback user_cb;

static HTTP_BUF* http_buf;

static os_timer_t timeout_timer;

static uint8 http_flag = 0;

static void FUNCTION_ATTRIBUTE
free_req(void)
{

	if(http_hostname != NULL)
	{
		os_free(http_hostname);
		http_hostname = NULL;
	}
	if(http_path != NULL)
	{
		os_free(http_path);
		http_path = NULL;
	}
	if(http_post_data != NULL)
	{
		os_free(http_post_data);
		http_post_data = NULL;
	}

}

static void FUNCTION_ATTRIBUTE
free_http_buf(void)
{
	if(NULL == http_buf)
	{
		return;
	}

	if(http_buf->buffer != NULL)
	{
		os_free(http_buf->buffer);
		http_buf->buffer = NULL;
	}

	if(http_buf != NULL)
	{
		os_free(http_buf);
		http_buf = NULL;
	}

}

static void FUNCTION_ATTRIBUTE
free_conn(void)
{
	PRINTF("free conn\n");

	if(conn == NULL)
	{
		return;
	}

	espconn_delete(conn);

	if(conn->proto.tcp != NULL)
	{
		os_free(conn->proto.tcp);
		conn->proto.tcp = NULL;
	}

	if(conn != NULL)
	{
		os_free(conn);
		conn = NULL;
	}
}

static void  FUNCTION_ATTRIBUTE
http_exit(HTTP_RESULT http_result)
{
	http_flag = 0;
	PRINTF("htt_result:%d\n", http_result);

	free_req();

	if(HTTP_TIMEOUT == http_result)
	{
		PRINTF("http time out!\n");
		free_http_buf();
		free_conn();
		user_cb(NULL);
	}
	else
	{
		os_timer_disarm(&timeout_timer);
	}

	if(DNS_FAIL == http_result)
	{
		free_http_buf();
		user_cb(NULL);
	}

	else if(HTTP_ERR == http_result)
	{
		free_http_buf();
		free_conn();
		user_cb(NULL);

	}

	if(HTTP_OK == http_result)
	{
		free_conn();

		const char * version = "HTTP/1.1 ";
		if (os_strncmp(http_buf->buffer, version, os_strlen(version)) != 0)
		{
			PRINTF("Invalid version in %s\n", http_buf->buffer);
			return;
		}

		int http_status = atoi(http_buf->buffer + os_strlen(version));

		char * body = (char *)os_strstr(http_buf->buffer, "\r\n\r\n") + 4;

		if (user_cb != NULL)
		{
			 // Callback is optional.
			user_cb(body);
		}

		free_http_buf();
	}

}

static char * FUNCTION_ATTRIBUTE my_strdup(const char * str)
{
    if(str == NULL)
    {
        return NULL;
    }
	char * new_str = (char *)os_malloc(os_strlen(str) + 1 /*null character*/);
	if (new_str == NULL)
	{
		return NULL;
	}
	*(new_str + os_strlen(str)) = '\0';
	os_strcpy(new_str, str);
	return new_str;
}


static void FUNCTION_ATTRIBUTE receive_callback(void * arg, char * buf, unsigned short len)
{
	PRINTF("Receive Response...\n");
	struct espconn * conn = (struct espconn *)arg;

	if (http_buf == NULL)
	{
		PRINTF("http_buf=null\n");
		return;
	}

	// Let's do the equivalent of a realloc().
	const int new_size = http_buf->buffer_size + len;
	char * new_buffer;
	if (new_size > BUFFER_SIZE_MAX || NULL == (new_buffer = (char *)os_malloc(new_size)))
	{
		PRINTF("Response too long %d\n", new_size);

		if(new_buffer != NULL)
		{
			os_free(new_buffer);
			new_buffer = NULL;
		}
		return;
		// TODO: espconn_disconnect(conn) without crashing.
	}

	os_memcpy(new_buffer, http_buf->buffer, http_buf->buffer_size);
	os_memcpy(new_buffer + http_buf->buffer_size - 1 /*overwrite the null character*/, buf, len); // Append new data.
	new_buffer[new_size - 1] = '\0'; // Make sure there is an end of string.

	os_free(http_buf->buffer);
	http_buf->buffer = new_buffer;
	http_buf->buffer_size = new_size;

}

static void FUNCTION_ATTRIBUTE sent_callback(void * arg)
{
	struct espconn * conn = (struct espconn *)arg;

	if (http_post_data == NULL)
	{
		PRINTF("All sent\n");
	}
	else
	{
		// The headers were sent, now send the contents.
		PRINTF("Sending request body\n");
		espconn_secure_sent(conn, (uint8_t *)http_post_data, os_strlen(http_post_data));
		os_free(http_post_data);
		http_post_data = NULL;
	}
}

static void FUNCTION_ATTRIBUTE connect_callback(void * arg)
{
	PRINTF("http server Connected\n");
	struct espconn * conn = (struct espconn *)arg;

	espconn_regist_recvcb(conn, receive_callback);
	espconn_regist_sentcb(conn, sent_callback);

	char method[5] = "GET";
	char post_headers[128] = "";

	if (http_post_data != NULL)
	{
		// If there is data this is a POST request.
		os_memcpy(method, "POST", 5);
		os_sprintf(post_headers,
				   "Content-Type:"
				   HTTP_HEADER_CONTENT_TYPE
				   "\r\n"
				   "Content-Length: %d\r\n", os_strlen(http_post_data));
	}

	char buf[2048];
	int len = os_sprintf(buf,
						 "%s %s HTTP/1.1\r\n"
						 "Host: %s:%d\r\n"
						 "Connection: close\r\n"
						 "User-Agent: ESP8266\r\n"
						 "%s"
						 "\r\n",
						 method, http_path, http_hostname, http_port, post_headers);

	espconn_secure_sent(conn, (uint8_t *)buf, len);
	PRINTF("Sending request header\n");
}

static void FUNCTION_ATTRIBUTE disconnect_callback(void * arg)
{
	PRINTF("http disconnected\n");

	if(http_buf != NULL)
	{
		if((http_buf->buffer_size > 1)&&(http_buf->buffer != NULL))
		{
			return http_exit(HTTP_OK);
		}
	}

	return http_exit(HTTP_ERR);
}

static void FUNCTION_ATTRIBUTE reconnect_callback(void *arg, sint8 err)
{
	PRINTF("http reconnected, error:%d\n", err);

	if(-61 == err)
	{
		if(http_buf != NULL)
		{
			if((http_buf->buffer_size > 1)&&(http_buf->buffer != NULL))
			{
				return http_exit(HTTP_OK);
			}
		}
	}
	http_exit(HTTP_ERR);
}

static void FUNCTION_ATTRIBUTE dns_callback(const char * hostname, ip_addr_t * addr, void * arg)
{
	//request_args * req = (request_args *)arg;

	if (addr == NULL)
	{
		PRINTF("DNS failed %s\n", hostname);
		http_exit(DNS_FAIL);
	}

	else
	{
		PRINTF("DNS found %s " IPSTR "\n", hostname, IP2STR(addr));
		conn = (struct espconn *)os_malloc(sizeof(struct espconn));
		conn->type = ESPCONN_TCP;
		conn->state = ESPCONN_NONE;
		conn->proto.tcp = (esp_tcp *)os_malloc(sizeof(esp_tcp));
		conn->proto.tcp->local_port = espconn_port();
		conn->proto.tcp->remote_port = http_port;
		os_memcpy(conn->proto.tcp->remote_ip, addr, 4);
		espconn_regist_connectcb(conn, connect_callback);
		espconn_regist_disconcb(conn, disconnect_callback);
		espconn_regist_reconcb(conn, reconnect_callback);
		PRINTF("start connect http server\n");
		// TODO: consider using espconn_regist_reconcb (for timeouts?)
		// cf esp8266_sdk_v0.9.1/examples/at/user/at_ipCmd.c  (TCP ARQ retransmission?)
		espconn_secure_connect(conn);
	}
}

void FUNCTION_ATTRIBUTE http_raw_request(const char * hostname, int port, const char * path, const char * post_data, http_callback user_callback)
{
	if(http_flag == 1)
	{
		PRINTF("http client is running, exit");
		return;
	}

	http_flag = 1;

	PRINTF("DNS request\n");
	os_timer_disarm(&timeout_timer);
	os_timer_setfn(&timeout_timer, (os_timer_func_t *)http_exit, HTTP_TIMEOUT);
	os_timer_arm(&timeout_timer, 20000, 0);

	http_hostname = my_strdup(hostname);
	http_path = my_strdup(path);
	http_port = port;
	http_post_data = my_strdup(post_data);
    // respond buf
	http_buf = (HTTP_BUF*)os_malloc(sizeof(HTTP_BUF));
	http_buf->buffer = (char *)os_malloc(1);
	http_buf->buffer[0] = '\0'; // Empty string.
	http_buf->buffer_size = 1;
	user_cb= user_callback;
	ip_addr_t addr;
	err_t error = espconn_gethostbyname(NULL, // It seems we don't need a real espconn pointer here.
										hostname, &addr, dns_callback);

	if (error == ESPCONN_INPROGRESS) {
		PRINTF("DNS pending\n");
	}
	else if (error == ESPCONN_OK)
	{
		// Already in the local names table (or hostname was an IP address), execute the callback ourselves.
		dns_callback(hostname, &addr, NULL);
	}
	else if (error == ESPCONN_ARG) {
		PRINTF("DNS error %s\n", hostname);
	}
	else {
		PRINTF("DNS error code %d\n", error);
	}
}


void net_http_post(const char* url, const char* data, http_callback http_cb)
{
	// FIXME: handle HTTP auth with http://user:pass@host/
		// FIXME: make https work.
		// FIXME: get rid of the #anchor part if present.

		char hostname[128] = "";
		int port = 443;
		PRINTF("URL is %s\n", url);
		if (pd_strncmp(url, "http://", pd_strlen("http://")) != 0) {
			PRINTF("URL is not HTTP %s\n", url);
			return;
		}
		url += pd_strlen("http://"); // Get rid of the protocol.

		char * path = pd_strchr(url, '/');
		if (path == NULL) {
			path = pd_strchr(url, '\0'); // Pointer to end of string.
		}

		char * colon = pd_strchr(url, ':');
		if (colon > path) {
			colon = NULL; // Limit the search to characters before the path.
		}

		if (colon == NULL) { // The port is not present.
			pd_memcpy(hostname, url, path - url);
			hostname[path - url] = '\0';
		}
		else {
			port = atoi(colon + 1);
			if (port == 0) {
				PRINTF("Port error %s\n", url);
				return;
			}

			pd_memcpy(hostname, url, colon - url);
			hostname[colon - url] = '\0';
		}


		if (path[0] == '\0') { // Empty path is not allowed.
			path = "/";
		}

		PRINTF("hostname=%s\n", hostname);
		PRINTF("port=%d\n", port);
		PRINTF("path=%s\n", path);
		http_raw_request(hostname, port, path, data, http_cb);
}


void net_http_get(const char* url, http_callback http_cb)
{
	;
}

