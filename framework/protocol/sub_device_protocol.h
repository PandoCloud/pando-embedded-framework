//  Copyright (c) 2015 Pando. All rights reserved.
//  PtotoBuf:   ProtocolBuffer.h
//
//  Create By ZhaoWenwu On 15/01/24.

#ifndef SUB_DEVICE_PROTOCOL_TOOL_H
#define SUB_DEVICE_PROTOCOL_TOOL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common_functions.h"
#include "pando_endian.h"

#define MAGIC_HEAD_SUB_DEVICE 0x34
#define PAYLOAD_TYPE_COMMAND 1
#define PAYLOAD_TYPE_EVENT	 2
#define PAYLOAD_TYPE_DATA	 3

#define	TLV_TYPE_FLOAT64 1 
#define TLV_TYPE_FLOAT32 2 
#define	TLV_TYPE_INT8    3
#define	TLV_TYPE_INT16   4
#define	TLV_TYPE_INT32   5
#define TLV_TYPE_INT64   6 
#define	TLV_TYPE_UINT8   7
#define TLV_TYPE_UINT16  8
#define TLV_TYPE_UINT32  9
#define TLV_TYPE_UINT64 10
#define	TLV_TYPE_BYTES  11
#define	TLV_TYPE_URI    12
#define	TLV_TYPE_BOOL   13

#define DEV_HEADER_LEN (sizeof(struct device_header))

#pragma pack(1)

struct device_header
{
    uint8_t  magic;         /* 寮�濮嬫爣蹇� (0x34) */
    uint8_t  crc;           /* 鏍￠獙鍜� */
    uint16_t payload_type;  /* 杞借嵎绫诲瀷 */
    uint16_t payload_len;   /* 杞借嵎闀垮害 */
    uint16_t flags;         /* 鏍囧織浣� */
    uint32_t frame_seq;     /* 甯у簭鍒� */
};

/*TLV淇℃伅鍖猴紝鍖呭惈count*/
struct TLV 
{
	uint16_t type;
	uint16_t length;
	uint8_t value[];
};

struct TLVs
{
    uint16_t count;
    //struct TLV tlv[];
};

/*鍛戒护锛屼簨浠跺拰鏁版嵁鍏蜂綋鐨勬暟鎹粨鏋�*/
struct pando_command
{
	uint16_t sub_device_id;   /* 瀛愯澶嘔D */
	uint16_t command_id;      /* 鍛戒护ID */
	uint16_t priority;        /* 浼樺厛绾� */
	struct TLVs params[1];          /* 鍙傛暟 */
};

struct pando_event
{
	uint16_t sub_device_id;   /* 瀛愯澶嘔D */
	uint16_t event_num;        /* 浜嬩欢ID */
    uint16_t priority;
	struct TLVs params[1];          /* 鍙傛暟 */
};

/*灞炴�х殑瀹氫箟*/
struct pando_property
{
	uint16_t sub_device_id;   /* 瀛愯澶嘔D */
	uint16_t property_num;	/* 灞炴�х紪鍙� */
	struct TLVs params[1];          /* 鍙傛暟 */
};

struct sub_device_buffer
{
	uint16_t buffer_length;
	uint8_t *buffer;
};

struct sub_device_base_params
{
	uint32_t event_sequence;
	uint32_t data_sequence;
	uint32_t command_sequence;
};
#pragma pack()


//鍒濆鍖栧瓙璁惧妯″潡
int init_sub_device(struct sub_device_base_params base_params);

//鍦ㄥ垱寤烘暟鎹寘鎴栬�呬簨浠跺寘鍓嶏紝鍏堝垱寤哄ソ鍙傛暟鐨勪俊鎭尯, 鍚屾椂娣诲姞绗竴涓弬鏁帮紝寰呬俊鎭尯琚玞reate_event绛夊嚱鏁版垚鍔熶娇鐢ㄥ悗锛岃灏嗕俊鎭尯delete
struct TLVs *create_params_block(void);

//鍒涘缓浜嬩欢鍖咃紝杩斿洖缂撳啿鍖猴紝
//鏁版嵁鍙戦�佸畬鎴愬悗锛岃灏嗚繑鍥炵殑缂撳啿鍖篸elete鎺�
struct sub_device_buffer *create_command_package(uint16_t flags);
struct sub_device_buffer *create_event_package(uint16_t flags);
struct sub_device_buffer *create_data_package(uint16_t flags);

uint16_t get_sub_device_payloadtype(struct sub_device_buffer *package);

int finish_package(struct sub_device_buffer *package_buf);


int add_next_property(struct sub_device_buffer *data_package, uint16_t property_num, 
    struct TLVs *next_data_params);
int add_command(struct sub_device_buffer *command_package, uint16_t command_num,
    uint16_t priority, struct TLVs *command_params);
int add_event(struct sub_device_buffer *event_package, uint16_t event_num,
    uint16_t priority, struct TLVs *event_params);


//瑙ｆ瀽鍛戒护鍖�,command_body浼犲嚭command_id銆佸弬鏁颁釜鏁扮瓑淇℃伅,杩斿洖绗竴涓弬鏁扮殑鎸囬拡锛岀敤浜巊et_tlv_param鑾峰彇鍙傛暟
struct TLVs *get_sub_device_command(struct sub_device_buffer *device_buffer, struct pando_command *command_body);
struct TLVs *get_sub_device_event(struct sub_device_buffer *device_buffer, struct pando_event *event_body);

//get data's property id and property num with property_body, return tlv param block
struct TLVs *get_sub_device_property(struct sub_device_buffer *device_buffer, struct pando_property *property_body);


//鍒犻櫎瀛愯澶囩紦鍐插尯锛屽鏋滀负鍙傛暟鍒涘缓杩囦俊鎭尯锛岃繕闇�瑕佸垹闄や俊鎭尯
void delete_device_package(struct sub_device_buffer *device_buffer);
void delete_params_block(struct TLVs *params_block);

int is_device_file_command(struct sub_device_buffer *device_buffer);



// tlv operation functions, maybe need move to other file.
// you must decode all the tlv params, otherwise can't decode next packet correctly.  
uint8_t     get_next_uint8(struct TLVs *params);
uint16_t    get_next_uint16(struct TLVs *params);
uint32_t    get_next_uint32(struct TLVs *params);
uint64_t    get_next_uint64(struct TLVs *params);
int8_t      get_next_int8(struct TLVs *params);
int16_t     get_next_int16(struct TLVs *params);
int32_t     get_next_int32(struct TLVs *params);
int64_t     get_next_int64(struct TLVs *params);
float       get_next_float32(struct TLVs *params);
double      get_next_float64(struct TLVs *params);
uint8_t     get_next_bool(struct TLVs *params);
void        *get_next_uri(struct TLVs *params, uint16_t *length);
void        *get_next_bytes(struct TLVs *params, uint16_t *length);

//澶氭璋冪敤鐩磋嚦娣诲姞瀹屾墍鏈夊弬鏁�
int add_next_param(struct TLVs *params_block, uint16_t next_type, uint16_t next_length, void *next_value);
int    add_next_uint8(struct TLVs *params, uint8_t next_value);
int    add_next_uint16(struct TLVs *params, uint16_t next_value);
int    add_next_uint32(struct TLVs *params, uint32_t next_value);
int    add_next_uint64(struct TLVs *params, uint64_t next_value);
int    add_next_int8(struct TLVs *params, int8_t next_value);
int    add_next_int16(struct TLVs *params, int16_t next_value);
int    add_next_int32(struct TLVs *params, int32_t next_value);
int    add_next_int64(struct TLVs *params, int64_t next_value);
int    add_next_float32(struct TLVs *params, float next_value);
int    add_next_float64(struct TLVs *params, double next_value);
int    add_next_bool(struct TLVs *params, uint8_t next_value);
int    add_next_uri(struct TLVs *params, uint16_t length, void *next_value);
int    add_next_bytes(struct TLVs *params, uint16_t length, void *next_value);

#ifdef __cplusplus
}
#endif
#endif



