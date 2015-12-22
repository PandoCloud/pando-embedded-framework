/* mqtt.c
*  Protocol: http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "mqtt_msg.h"
#include "debug.h"
#include "mqtt.h"
#include "queue.h"
#include "../../platform/include/pando_sys.h"
#include "../../platform/include/pando_types.h"
#include "../../platform/include/pando_timer.h"
#include "../../platform/include/pando_net_tcp.h"
#include "utils.h"

#define MQTT_BUF_SIZE		1024
#define MQTT_RECONNECT_TIMEOUT 5

#define MQTT_TASK_PRIO        		0
#define MQTT_TASK_QUEUE_SIZE    	1
#define MQTT_SEND_TIMOUT			5

#ifndef QUEUE_BUFFER_SIZE
#define QUEUE_BUFFER_SIZE		 	2048
#endif

void MQTT_Task(MQTT_Client * arg);


static void FUNCTION_ATTRIBUTE
deliver_publish(MQTT_Client* client, uint8_t* message, int length)
{
	mqtt_event_data_t event_data;

	event_data.topic_length = length;
	event_data.topic = mqtt_get_publish_topic(message, &event_data.topic_length);
	event_data.data_length = length;
	event_data.data = mqtt_get_publish_data(message, &event_data.data_length);

	if(client->dataCb)
		client->dataCb((uint32_t*)client, event_data.topic, event_data.topic_length, event_data.data, event_data.data_length);

}


/**
  * @brief  Client received callback function.
  * @param  arg: contain the ip link information
  * @param  pdata: received data
  * @param  len: the lenght of received data
  * @retval None
  */
void FUNCTION_ATTRIBUTE
mqtt_tcpclient_recv(void *arg, struct data_buf *buffer)
{
	uint8_t msg_type;
	uint8_t msg_qos;
	uint16_t msg_id;

	struct pando_tcp_conn *pCon = (struct pando_tcp_conn*)arg;
	MQTT_Client *client = (MQTT_Client *)pCon->reverse;

READPACKET:
	INFO("TCP: data received %d bytes\r\n", buffer->length);
	show_package(buffer->data, buffer->length);
	if(buffer->length < MQTT_BUF_SIZE && buffer->length > 0){
		pd_memcpy(client->mqtt_state.in_buffer, buffer->data, buffer->length);

		msg_type = mqtt_get_type(client->mqtt_state.in_buffer);
		msg_qos = mqtt_get_qos(client->mqtt_state.in_buffer);
		msg_id = mqtt_get_id(client->mqtt_state.in_buffer, client->mqtt_state.in_buffer_length);
		PRINTF("client->connstate:%d, type:%d, Qos:%d, id:%d\n", client->connState, msg_type, msg_qos, msg_id);
		switch(client->connState){
		case MQTT_CONNECT_SENDING:
			if(msg_type == MQTT_MSG_TYPE_CONNACK){
				if(client->mqtt_state.pending_msg_type != MQTT_MSG_TYPE_CONNECT){
					INFO("MQTT: Invalid packet\r\n");
					if(client->security){
						//espconn_secure_disconnect(client->pCon);
                        net_tcp_disconnect(client->pCon);
					}
					else {
						//espconn_disconnect(client->pCon);
						net_tcp_disconnect(client->pCon);
					}
				} else {
					INFO("MQTT: Connected to %s:%d\r\n", client->host, client->port);
					client->connState = MQTT_DATA;
					if(client->connectedCb)
						client->connectedCb((uint32_t*)client);
				}

			}
			break;
		case MQTT_DATA:
			client->mqtt_state.message_length_read = buffer->length;
			client->mqtt_state.message_length = mqtt_get_total_length(client->mqtt_state.in_buffer, client->mqtt_state.message_length_read);
			INFO("mqtt actual length:%d\n", client->mqtt_state.message_length);

			switch(msg_type)
			{

			  case MQTT_MSG_TYPE_SUBACK:
				if(client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_SUBSCRIBE && client->mqtt_state.pending_msg_id == msg_id)
				  INFO("MQTT: Subscribe successful\r\n");
				break;
			  case MQTT_MSG_TYPE_UNSUBACK:
				if(client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_UNSUBSCRIBE && client->mqtt_state.pending_msg_id == msg_id)
				  INFO("MQTT: UnSubscribe successful\r\n");
				break;
			  case MQTT_MSG_TYPE_PUBLISH:
				if(msg_qos == 1)
					client->mqtt_state.outbound_message = mqtt_msg_puback(&client->mqtt_state.mqtt_connection, msg_id);
				else if(msg_qos == 2)
					client->mqtt_state.outbound_message = mqtt_msg_pubrec(&client->mqtt_state.mqtt_connection, msg_id);
				if(msg_qos == 1 || msg_qos == 2){
					INFO("MQTT: Queue response QoS: %d\r\n", msg_qos);
					if(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
						INFO("MQTT: Queue full\r\n");
					}
				}

				deliver_publish(client, client->mqtt_state.in_buffer, client->mqtt_state.message_length_read);
				break;
			  case MQTT_MSG_TYPE_PUBACK:
				if(client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_PUBLISH && client->mqtt_state.pending_msg_id == msg_id){
				  INFO("MQTT: received MQTT_MSG_TYPE_PUBACK, finish QoS1 publish\r\n");
				}

				break;
			  case MQTT_MSG_TYPE_PUBREC:
				  client->mqtt_state.outbound_message = mqtt_msg_pubrel(&client->mqtt_state.mqtt_connection, msg_id);
				  if(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
				  	INFO("MQTT: Queue full\r\n");
				  }
				break;
			  case MQTT_MSG_TYPE_PUBREL:
				  client->mqtt_state.outbound_message = mqtt_msg_pubcomp(&client->mqtt_state.mqtt_connection, msg_id);
				  if(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
					INFO("MQTT: Queue full\r\n");
				  }
				break;
			  case MQTT_MSG_TYPE_PUBCOMP:
				if(client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_PUBLISH && client->mqtt_state.pending_msg_id == msg_id){
				  INFO("MQTT: receive MQTT_MSG_TYPE_PUBCOMP, finish QoS2 publish\r\n");
				}
				break;
			  case MQTT_MSG_TYPE_PINGREQ:
				  client->mqtt_state.outbound_message = mqtt_msg_pingresp(&client->mqtt_state.mqtt_connection);
				  if(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
					INFO("MQTT: Queue full\r\n");
				  }
				break;
			  case MQTT_MSG_TYPE_PINGRESP:
				// Ignore
				break;
			}
			// NOTE: this is done down here and not in the switch case above
			// because the PSOCK_READBUF_LEN() won't work inside a switch
			// statement due to the way protothreads resume.
			if(msg_type == MQTT_MSG_TYPE_PUBLISH)
			{
			  INFO("PUBLISH MESSAGE,receive length:%d, actual length:%d\n",client->mqtt_state.message_length, client->mqtt_state.message_length_read);
			  buffer->length = client->mqtt_state.message_length_read;

			  if(client->mqtt_state.message_length < client->mqtt_state.message_length_read)
			  {
				  //client->connState = MQTT_PUBLISH_RECV;
				  //Not Implement yet
				  buffer->length -= client->mqtt_state.message_length;
				  buffer->data += client->mqtt_state.message_length;

				  INFO("Get another published message\r\n");
				  goto READPACKET;
			  }

			}
			break;
		}
	} else {
		INFO("ERROR: Message too long\r\n");
	}
	MQTT_Task(client);
}

/**
  * @brief  Client send over callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
void FUNCTION_ATTRIBUTE
mqtt_tcpclient_sent_cb(void *arg, int8_t errorno)
{
	struct pando_tcp_conn *pCon = (struct pando_tcp_conn *)arg;
	MQTT_Client* client = (MQTT_Client *)pCon->reverse;
	INFO("TCP: Sent\r\n");
	client->sendTimeout = 0;
	if(client->connState == MQTT_DATA && client->mqtt_state.pending_msg_type == MQTT_MSG_TYPE_PUBLISH){
		if(client->publishedCb)
			client->publishedCb((uint32_t*)client);
	}
	MQTT_Task(client);
}

void FUNCTION_ATTRIBUTE mqtt_timer(void *arg)
{
	pd_printf("enter into mqtt timer!!!\n");
	MQTT_Client* client = (MQTT_Client*)arg;
    struct data_buf buffer;
	if(client->connState == MQTT_DATA)
	{
		client->keepAliveTick ++;
		if(client->keepAliveTick > client->mqtt_state.connect_info->keepalive){
			INFO("\r\nMQTT: Send keepalive packet to %s:%d!\r\n", client->host, client->port);
			client->mqtt_state.outbound_message = mqtt_msg_pingreq(&client->mqtt_state.mqtt_connection);
			client->mqtt_state.pending_msg_type = MQTT_MSG_TYPE_PINGREQ;
			client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
			client->mqtt_state.pending_msg_id = mqtt_get_id(client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length);


			client->sendTimeout = MQTT_SEND_TIMOUT;
            buffer.length = client->mqtt_state.outbound_message->length;
            buffer.data = client->mqtt_state.outbound_message->data;
			INFO("MQTT: Sending, type: %d, id: %04X\r\n",client->mqtt_state.pending_msg_type, client->mqtt_state.pending_msg_id);
			if(client->security){
				//espconn_secure_sent(client->pCon, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length);
                net_tcp_send(client->pCon, buffer, client->sendTimeout);
            }
			else{
				net_tcp_send(client->pCon, buffer, client->sendTimeout);
			}

			client->mqtt_state.outbound_message = NULL;

			client->keepAliveTick = 0;
			MQTT_Task(client);
		}

	} else if(client->connState == TCP_RECONNECT_REQ)
	{
		client->reconnectTick ++;
		if(client->reconnectTick > MQTT_RECONNECT_TIMEOUT) {
			client->reconnectTick = 0;
			client->connState = TCP_RECONNECT;
			MQTT_Task(client);
		}
	}else if(client->connState == TCP_CONNECTING){
		client->connState = TCP_CONNECTING_ERROR;
		MQTT_Task(client);
	}

	if(client->sendTimeout > 0)
		client->sendTimeout --;
}

void FUNCTION_ATTRIBUTE
mqtt_tcpclient_discon_cb(void *arg, int8_t errno)
{

	struct pando_tcp_conn *pespconn = (struct pando_tcp_conn *)arg;
	MQTT_Client* client = (MQTT_Client *)pespconn->reverse;
	INFO("TCP: Disconnected callback\r\n");
	client->connState = TCP_RECONNECT_REQ;
	if(client->disconnectedCb)
		client->disconnectedCb((uint32_t*)client);

	MQTT_Task(client);
}



/**
  * @brief  Tcp client connect success callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
void FUNCTION_ATTRIBUTE
mqtt_tcpclient_connect_cb(void *arg , int8_t errno)
{
	pd_printf("enter into mqtt_tcpclient_connect_cb\n");
	struct pando_tcp_conn *pCon = (struct pando_tcp_conn *)arg;
	MQTT_Client* client = (MQTT_Client *)pCon->reverse;

    struct data_buf buffer;

	net_tcp_register_disconnected_callback(pCon, mqtt_tcpclient_discon_cb);
	net_tcp_register_recv_callback(pCon, mqtt_tcpclient_recv);////////
	net_tcp_register_sent_callback(pCon, mqtt_tcpclient_sent_cb);///////
	INFO("MQTT: Connected to broker %s:%d\r\n", client->host, client->port);


	mqtt_msg_init(&client->mqtt_state.mqtt_connection, client->mqtt_state.out_buffer, client->mqtt_state.out_buffer_length);
	client->mqtt_state.outbound_message = mqtt_msg_connect(&client->mqtt_state.mqtt_connection, client->mqtt_state.connect_info);
	client->mqtt_state.pending_msg_type = mqtt_get_type(client->mqtt_state.outbound_message->data);
	client->mqtt_state.pending_msg_id = mqtt_get_id(client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length);

	client->sendTimeout = MQTT_SEND_TIMOUT;

    buffer.length = client->mqtt_state.outbound_message->length;
    buffer.data = client->mqtt_state.outbound_message->data;
    INFO("MQTT: Sending, type: %d, id: %04X\r\n",client->mqtt_state.pending_msg_type, client->mqtt_state.pending_msg_id);
	if(client->security){
        net_tcp_send(pCon, buffer, client->sendTimeout);
        //espconn_secure_sent(client->pCon, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length);
	}
	else{
        net_tcp_send(pCon, buffer, client->sendTimeout);
		//espconn_sent(client->pCon, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length);
	}

	client->mqtt_state.outbound_message = NULL;
	client->connState = MQTT_CONNECT_SENDING;
	MQTT_Task(client);
}

/**
  * @brief  Tcp client connect repeat callback function.
  * @param  arg: contain the ip link information
  * @retval None
  */
void FUNCTION_ATTRIBUTE
mqtt_tcpclient_recon_cb(void *arg, int8_t errType)
{
	struct pando_tcp_conn *pCon = (struct pando_tcp_conn *)arg;
	MQTT_Client* client = (MQTT_Client *)pCon->reverse;

	INFO("TCP: Reconnect to %s:%d\r\n", client->host, client->port);

	client->connState = TCP_RECONNECT_REQ;

	MQTT_Task(client);

}

/**
  * @brief  MQTT publish function.
  * @param  client: 	MQTT_Client reference
  * @param  topic: 		string topic will publish to
  * @param  data: 		buffer data send point to
  * @param  data_length: length of data
  * @param  qos:		qos
  * @param  retain:		retain
  * @retval TRUE if success queue
  */
BOOL FUNCTION_ATTRIBUTE
MQTT_Publish(MQTT_Client *client, const char* topic, const char* data, int data_length, int qos, int retain)
{
	uint8_t dataBuffer[MQTT_BUF_SIZE];
	uint16_t dataLen;
	client->mqtt_state.outbound_message = mqtt_msg_publish(&client->mqtt_state.mqtt_connection,
										 topic, data, data_length,
										 qos, retain,
										 &client->mqtt_state.pending_msg_id);
	if(client->mqtt_state.outbound_message->length == 0){
		INFO("MQTT: Queuing publish failed\r\n");
		return FALSE;
	}
	INFO("MQTT: queuing publish, length: %d, queue size(%d/%d)\r\n", client->mqtt_state.outbound_message->length, client->msgQueue.rb.fill_cnt, client->msgQueue.rb.size);
	while(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
		INFO("MQTT: Queue full\r\n");
		if(QUEUE_Gets(&client->msgQueue, dataBuffer, &dataLen, MQTT_BUF_SIZE) == -1) {
			INFO("MQTT: Serious buffer error\r\n");
			return FALSE;
		}
	}
	pd_printf("client->mqtt_state.outbound_message->length:%d",client->mqtt_state.outbound_message->length);
	MQTT_Task(client);
	return TRUE;
}

/**
  * @brief  MQTT subscibe function.
  * @param  client: 	MQTT_Client reference
  * @param  topic: 		string topic will subscribe
  * @param  qos:		qos
  * @retval TRUE if success queue
  */
BOOL FUNCTION_ATTRIBUTE
MQTT_Subscribe(MQTT_Client *client, char* topic, uint8_t qos)
{
	uint8_t dataBuffer[MQTT_BUF_SIZE];
	uint16_t dataLen;

	client->mqtt_state.outbound_message = mqtt_msg_subscribe(&client->mqtt_state.mqtt_connection,
											topic, 0,
											&client->mqtt_state.pending_msg_id);
	INFO("MQTT: queue subscribe, topic\"%s\", id: %d\r\n",topic, client->mqtt_state.pending_msg_id);
	while(QUEUE_Puts(&client->msgQueue, client->mqtt_state.outbound_message->data, client->mqtt_state.outbound_message->length) == -1){
		INFO("MQTT: Queue full\r\n");
		if(QUEUE_Gets(&client->msgQueue, dataBuffer, &dataLen, MQTT_BUF_SIZE) == -1) {
			INFO("MQTT: Serious buffer error\r\n");
			return FALSE;
		}
	}
	MQTT_Task(client);
	return TRUE;
}

void FUNCTION_ATTRIBUTE
MQTT_Task(MQTT_Client *client)
{
	INFO("MQTT TASK\n");
	uint8_t dataBuffer[MQTT_BUF_SIZE];
	uint16_t dataLen;
    struct data_buf buffer;
    
	if(client == NULL)
		return;
	switch(client->connState){

	case TCP_RECONNECT_REQ:
		break;
	case TCP_RECONNECT:
		MQTT_Connect(client);
		INFO("TCP: Reconnect to: %s:%d\r\n", client->host, client->port);
		client->connState = TCP_CONNECTING;
		break;
	case MQTT_DATA:
		INFO("MQTT TASK DATA\n");
		if(QUEUE_IsEmpty(&client->msgQueue) || client->sendTimeout != 0)
		{
			break;
		}
		if(QUEUE_Gets(&client->msgQueue, dataBuffer, &dataLen, MQTT_BUF_SIZE) == 0)
		{
			client->mqtt_state.pending_msg_type = mqtt_get_type(dataBuffer);
			client->mqtt_state.pending_msg_id = mqtt_get_id(dataBuffer, dataLen);

			client->sendTimeout = MQTT_SEND_TIMOUT;
			buffer.data = dataBuffer;
			buffer.length = dataLen;

			INFO("MQTT: Sending, type: %d, id: %04X\r\n",client->mqtt_state.pending_msg_type, client->mqtt_state.pending_msg_id);
			if(client->security){
                net_tcp_send(client->pCon, buffer, client->sendTimeout);
                //espconn_secure_sent(client->pCon, dataBuffer, dataLen);
			}
			else{
				INFO("net_tcp_send\n");
				INFO("client:%d",client);

                net_tcp_send(client->pCon, buffer, client->sendTimeout);
				//espconn_sent(client->pCon, dataBuffer, dataLen);
			}

			client->mqtt_state.outbound_message = NULL;
			break;
		}
		break;
	}
}

/**
  * @brief  MQTT initialization connection function
  * @param  client: 	MQTT_Client reference
  * @param  host: 	Domain or IP string
  * @param  port: 	Port to connect
  * @param  security:		1 for ssl, 0 for none
  * @retval None
  */
void FUNCTION_ATTRIBUTE
MQTT_InitConnection(MQTT_Client *mqttClient, uint8_t* host, uint32_t port, uint8_t security)
{
	uint32_t temp;
	INFO("MQTT_InitConnection\r\n");
	pd_memset(mqttClient, 0, sizeof(MQTT_Client));
	temp = pd_strlen(host);
	mqttClient->host = (uint8_t*)pd_malloc(temp + 1);
    pd_memset(mqttClient->host, 0, temp + 1);
	pd_strncpy(mqttClient->host, host);
	mqttClient->host[temp] = 0;
	mqttClient->port = port;
	mqttClient->security = security;
	PRINTF("MQTT_InitConnection END...\n");
}

/**
  * @brief  MQTT initialization mqtt client function
  * @param  client: 	MQTT_Client reference
  * @param  clientid: 	MQTT client id
  * @param  client_user:MQTT client user
  * @param  client_pass:MQTT client password
  * @param  client_pass:MQTT keep alive timer, in second
  * @retval None
  */
void FUNCTION_ATTRIBUTE
MQTT_InitClient(MQTT_Client *mqttClient, uint8_t* client_id, uint8_t* client_user, uint8_t* client_pass, uint32_t keepAliveTime, uint8_t cleanSession)
{
	uint32_t temp;
	INFO("MQTT_InitClient\r\n");

	pd_memset(&mqttClient->connect_info, 0, sizeof(mqtt_connect_info_t));

	temp = pd_strlen(client_id);
	mqttClient->connect_info.client_id = (uint8_t*)pd_malloc(temp + 1);
    pd_memset(mqttClient->connect_info.client_id, 0, temp + 1);
	pd_strncpy(mqttClient->connect_info.client_id, client_id);
	mqttClient->connect_info.client_id[temp] = 0;

	if(client_user == NULL)
	{
		mqttClient->connect_info.username = NULL;
	}

	else
	{
		temp = pd_strlen(client_user);
		mqttClient->connect_info.username = (uint8_t*)pd_malloc(temp + 1);
        pd_memset(mqttClient->connect_info.username, 0, temp + 1);
		pd_strncpy(mqttClient->connect_info.username, client_user);
		mqttClient->connect_info.username[temp] = 0;
	}
	if(client_pass == NULL)
	{
		PRINTF("client_pass == NULL...\n");
		mqttClient->connect_info.password = NULL;
	}

	else
	{
		temp = pd_strlen(client_pass);
		mqttClient->connect_info.password = (uint8_t*)pd_malloc(temp + 1);
        pd_memset(mqttClient->connect_info.password, 0, temp + 1);
		pd_strncpy(mqttClient->connect_info.password, client_pass);
		mqttClient->connect_info.password[temp] = 0;
	}

	mqttClient->connect_info.keepalive = keepAliveTime;
	mqttClient->connect_info.clean_session = cleanSession;

	mqttClient->mqtt_state.in_buffer = (uint8_t *)pd_malloc(MQTT_BUF_SIZE);
    pd_memset(mqttClient->mqtt_state.in_buffer, 0, MQTT_BUF_SIZE);
	mqttClient->mqtt_state.in_buffer_length = MQTT_BUF_SIZE;
	mqttClient->mqtt_state.out_buffer =  (uint8_t *)pd_malloc(MQTT_BUF_SIZE);
    pd_memset(mqttClient->mqtt_state.out_buffer, 0, MQTT_BUF_SIZE);
    mqttClient->mqtt_state.out_buffer_length = MQTT_BUF_SIZE;
	mqttClient->mqtt_state.connect_info = &mqttClient->connect_info;

	mqtt_msg_init(&mqttClient->mqtt_state.mqtt_connection, mqttClient->mqtt_state.out_buffer, mqttClient->mqtt_state.out_buffer_length);

	QUEUE_Init(&mqttClient->msgQueue, QUEUE_BUFFER_SIZE);
#if 0
	system_os_task(MQTT_Task, MQTT_TASK_PRIO, mqtt_procTaskQueue, MQTT_TASK_QUEUE_SIZE);
	system_os_post(MQTT_TASK_PRIO, 0, (os_param_t)mqttClient);
#endif
    MQTT_Task(mqttClient);
}

void FUNCTION_ATTRIBUTE
MQTT_InitLWT(MQTT_Client *mqttClient, uint8_t* will_topic, uint8_t* will_msg, uint8_t will_qos, uint8_t will_retain)
{
	uint32_t temp;
	temp = pd_strlen(will_topic);
	mqttClient->connect_info.will_topic = (uint8_t*)pd_malloc(temp + 1);
    pd_memset(mqttClient->connect_info.will_topic, 0, temp + 1);
	pd_strncpy(mqttClient->connect_info.will_topic, will_topic);
	mqttClient->connect_info.will_topic[temp] = 0;

	temp = pd_strlen(will_msg);
	mqttClient->connect_info.will_message = (uint8_t*)pd_malloc(temp + 1);
    pd_memset(mqttClient->connect_info.will_message, 0, temp + 1);
    
	pd_strncpy(mqttClient->connect_info.will_message, will_msg);
	mqttClient->connect_info.will_message[temp] = 0;


	mqttClient->connect_info.will_qos = will_qos;
	mqttClient->connect_info.will_retain = will_retain;
}
/**
  * @brief  Begin connect to MQTT broker
  * @param  client: MQTT_Client reference
  * @retval None
  */
void FUNCTION_ATTRIBUTE
MQTT_Connect(MQTT_Client *mqttClient)
{
	INFO("MQTT_Connect start..\n");
	MQTT_Disconnect(mqttClient);
#if 0   
	mqttClient->pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
	mqttClient->pCon->type = ESPCONN_TCP;
	mqttClient->pCon->state = ESPCONN_NONE;
	mqttClient->pCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	mqttClient->pCon->proto.tcp->local_port = espconn_port();
	mqttClient->pCon->proto.tcp->remote_port = mqttClient->port;
	mqttClient->pCon->reverse = mqttClient;
	espconn_regist_connectcb(mqttClient->pCon, mqtt_tcpclient_connect_cb);
	espconn_regist_reconcb(mqttClient->pCon, mqtt_tcpclient_recon_cb);
#endif
    mqttClient->pCon = (struct pando_tcp_conn *)pd_malloc(sizeof(struct pando_tcp_conn));
    pd_memset(mqttClient->pCon, 0, sizeof(struct pando_tcp_conn));



	mqttClient->pCon->local_port = espconn_port();
	mqttClient->pCon->remote_port = mqttClient->port;
	mqttClient->pCon->reverse = mqttClient;
	mqttClient->pCon->secure = 0;
	//mqttClient->pCon->reverse = mqttClient;
	PRINTF("MQTT_Connect_mqttClient->pCon->reverse:%d\n",mqttClient->pCon->reverse);
    net_tcp_register_connected_callback(mqttClient->pCon, mqtt_tcpclient_connect_cb);
    //no reconnection call back. TODO
    

	mqttClient->keepAliveTick = 0;
	mqttClient->reconnectTick = 0;

#if 0
	os_timer_disarm(&mqttClient->mqttTimer);
	os_timer_setfn(&mqttClient->mqttTimer, (os_timer_func_t *)mqtt_timer, mqttClient);
	os_timer_arm(&mqttClient->mqttTimer, 1000, 1);
#endif
	INFO("timer1 init statrt...");
	mqttClient->mqttTimer.interval = 1000;
	mqttClient->mqttTimer.repeated = 1;
	mqttClient->mqttTimer.arg = mqttClient;
	mqttClient->mqttTimer.timer_cb = mqtt_timer;
    timer1_init(mqttClient->mqttTimer);
    timer1_stop();
    timer1_start();
    INFO("timer1 init end...");
    
	if(UTILS_StrToIP(mqttClient->host, &mqttClient->pCon->remote_ip)) {
		INFO("TCP: Connect to ip  %s:%d\r\n", mqttClient->host, mqttClient->port);
		if(mqttClient->security){
			//espconn_secure_connect(mqttClient->pCon);
			//need a connect time out? TODO
			net_tcp_connect(mqttClient->pCon, mqttClient->sendTimeout);
		}
		else {
			//espconn_connect(mqttClient->pCon);
            //need to distinguish secure and non secure? TODO
			net_tcp_connect(mqttClient->pCon, mqttClient->sendTimeout);
		}
	}
	else {
		INFO("TCP: Connect to domain %s:%d\r\n", mqttClient->host, mqttClient->port);
        //need a host name function. TODO
        //espconn_gethostbyname(mqttClient->pCon, mqttClient->host, &mqttClient->ip, mqtt_dns_found);
	}
	mqttClient->connState = TCP_CONNECTING;
}

void FUNCTION_ATTRIBUTE
MQTT_Disconnect(MQTT_Client *mqttClient)
{
	if(mqttClient->pCon){
		INFO("Free memory\r\n");
        #if 0
		if(mqttClient->pCon->proto.tcp)
			pd_free(mqttClient->pCon->proto.tcp);
        #endif
		pd_free(mqttClient->pCon);
		mqttClient->pCon = NULL;
	}

   // timer1_stop();
	os_timer_disarm(&mqttClient->mqttTimer);
}
void FUNCTION_ATTRIBUTE
MQTT_OnConnected(MQTT_Client *mqttClient, MqttCallback connectedCb)
{
	mqttClient->connectedCb = connectedCb;
}

void FUNCTION_ATTRIBUTE
MQTT_OnConnect_Error(MQTT_Client *mqttClient, MqttCallback error_cb)
{
	mqttClient->errorCb= error_cb;
}

void FUNCTION_ATTRIBUTE
MQTT_OnDisconnected(MQTT_Client *mqttClient, MqttCallback disconnectedCb)
{
	mqttClient->disconnectedCb = disconnectedCb;
}

void FUNCTION_ATTRIBUTE
MQTT_OnData(MQTT_Client *mqttClient, MqttDataCallback dataCb)
{
	mqttClient->dataCb = dataCb;
}

void FUNCTION_ATTRIBUTE
MQTT_OnPublished(MQTT_Client *mqttClient, MqttCallback publishedCb)
{
	mqttClient->publishedCb = publishedCb;
}
