/*******************************************************
 * File name: pando_protocol.h
 * Author:    Zhao Wenwu
 * Versions:  0.1
 * Description: APIs for access device layer to manipulate packets.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/



#ifndef PANDP_PROTOCOL_TOOL_H
#define PANDP_PROTOCOL_TOOL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common_functions.h"
#include "sub_device_protocol.h"
#include "pando_endian.h"

#define MAX_PANDO_TOKEN_SIZE 16

#define MAGIC_HEAD_PANDO 0x7064
#define BIN_PANDO_TAG 0x0001
#define EMPTY_HEART_BEAT 0xffff

#define GATE_HEADER_LEN (sizeof(struct mqtt_bin_header))

#pragma pack(1)

struct mqtt_bin_header
{
    uint8_t flags;
    uint64_t timestamp;
    uint8_t token[MAX_PANDO_TOKEN_SIZE];
};


/* Payload must begin with sub device id, for access device can modify it directly */
struct pando_payload
{
	uint16_t sub_device_id;
};


/* pando_buffer is descriptor of buffer.
* This struct is used in access device, buff_len show the size of buffer, buffer is 
* address of memory, offset is the valid start position of packet in the buffer.
* Once you want to visit a packet, you should start from buffer+offset, end with buffer+buf_len. 
* This way need less malloc operation.
*/
struct pando_buffer
{
	uint16_t buff_len;   /* size of buffer */
	uint16_t offset;     /* begin of valid packet */
	uint8_t  *buffer;    /* addr of buffer */
};

/* basic data of access device, some member of this struct is unuseful. */
struct protocol_base
{
	uint64_t device_id;      /* unique id of access device */
	uint64_t event_sequence; /* event sequence of access device */
	uint64_t data_sequence;  /* data sequence of access device */
	uint64_t command_sequence;	/* command sequence of access device */
	uint32_t sub_device_cmd_seq; /* command sequence between access device and sub device */
	uint8_t  token[MAX_PANDO_TOKEN_SIZE];      /* token */
};
#pragma pack()


/* Init basic params of access device */
int pando_protocol_init(struct protocol_base init_params);

uint8_t *pando_get_package_begin(struct pando_buffer *buf);
uint16_t pando_get_package_length(struct pando_buffer *buf);


/* malloc a buffer, size equals length, valid package is from offset. offset usually is 0 */
struct pando_buffer* pando_buffer_create(int length, int offset);

/* release memory of buffer and buffer descriptor */
void pando_buffer_delete(struct pando_buffer *pdbuf);

/* encode packet from sub device into accss device, then send to cloud server */
int pando_protocol_encode(struct pando_buffer *pdbuf, uint16_t *payload_type);

/* decode packet from cloud server, then send to sub device */
int pando_protocol_decode(struct pando_buffer *pdbuf, uint16_t payload_type);

/* get sequence of command, this api seems unused now */
uint64_t pando_protocol_get_cmd_sequence(void);

/* get or set sub device id directly */
int pando_protocol_get_sub_device_id(struct pando_buffer *buf, uint16_t *sub_device_id);

int pando_protocol_set_sub_device_id(struct pando_buffer *buf, uint16_t sub_device_id);


/* get command type after gateway completes decoding the command from server. */
uint16_t pando_protocol_get_payload_type(struct pando_buffer *pdbuf);

#ifdef __cplusplus
}
#endif
#endif //PANDP_PROTOCOL_TOOL_H



