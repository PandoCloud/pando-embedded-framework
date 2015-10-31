#include <stdarg.h>
#include "pando_channel.h"
#include "gateway/pando_zero_device.h"

struct pando_channel
{
    PANDO_CHANNEL_TYPE type;
    channel_recv_callback subdevice_cb;
    channel_recv_callback device_cb;
};

//include the zero_device
static struct pando_channel channels[MAX_CHAN_LEN + 1];

void FUNCTION_ATTRIBUTE
channel_init(uint8_t channel_count, PANDO_CHANNEL_TYPE ch1_type, channel_recv_callback ch1_dev_re_cb, channel_recv_callback ch1_subdev_re_cb, ...)
{
    uint8_t index;
    va_list arg_ptr;
    channels[0].type = CHANNEL_MEMORY;
    channels[0].device_cb = NULL;
    channels[0].subdevice_cb = zero_device_data_process;
    channels[1].type = ch1_type;
    channels[1].device_cb = ch1_dev_re_cb;
    channels[1].subdevice_cb = ch1_subdev_re_cb;
    va_start(arg_ptr, ch1_subdev_re_cb);
    for(index = 2; index <= channel_count && index <= MAX_CHAN_LEN; index++)
    {
        channels[index].type = va_arg(arg_ptr, PANDO_CHANNEL_TYPE);
        channels[index].device_cb = va_arg(arg_ptr, channel_recv_callback);
        channels[index].subdevice_cb = va_arg(arg_ptr, channel_recv_callback);
    }

    va_end(arg_ptr);
    return;
}

void FUNCTION_ATTRIBUTE
channel_send_to_subdevice(uint8_t channel_no, uint8_t * buffer, uint16_t length)
{
    if(channels[channel_no].subdevice_cb == NULL )
    {
        return;
    }

    if(channels[channel_no].type == CHANNEL_MEMORY)
    {
        channels[channel_no].subdevice_cb(buffer, length);
        return;
    }
}

void FUNCTION_ATTRIBUTE
channel_send_to_device(uint8_t channel_no, uint8_t * buffer, uint16_t length)
{
    if(channels[channel_no].device_cb == NULL )
    {
        return;
    }

    if(channels[channel_no].type == CHANNEL_MEMORY)
    {
        channels[name].device_cb(buffer, length);
        return;
    }
}
