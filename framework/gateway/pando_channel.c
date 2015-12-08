#include "pando_channel.h"
#include "../platform/include/pando_sys.h"

#define MAX_CHAN_LEN 8

struct pando_channel
{
    PANDO_CHANNEL_NAME name;
    channel_recv_callback subdevice_cb;
    channel_recv_callback device_cb;
};

static struct pando_channel channels[MAX_CHAN_LEN];

void FUNCTION_ATTRIBUTE
on_subdevice_channel_recv(PANDO_CHANNEL_NAME name, channel_recv_callback cb)
{
    channels[name].subdevice_cb = cb;
}

void FUNCTION_ATTRIBUTE
on_device_channel_recv(PANDO_CHANNEL_NAME name, channel_recv_callback cb)
{
    channels[name].device_cb = cb;
}

void FUNCTION_ATTRIBUTE
channel_send_to_subdevice(PANDO_CHANNEL_NAME name, uint8_t * buffer, uint16_t length)
{
    if(channels[name].subdevice_cb != NULL ){
        channels[name].subdevice_cb(buffer, length);
    }
}

void FUNCTION_ATTRIBUTE
channel_send_to_device(PANDO_CHANNEL_NAME name, uint8_t * buffer, uint16_t length)
{
	pd_printf("send package to device\n");
    if(channels[name].device_cb != NULL ){
        channels[name].device_cb(buffer, length);
    }
}
