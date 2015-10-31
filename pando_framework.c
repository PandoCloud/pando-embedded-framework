#include "lib/pando_channel.h"
#include "gateway/pando_cloud_access.h"
#include "subdevice/pando_subdevice.h"
#include "pando_framework.h"

void ICACHE_FLASH_ATTR
pando_framework_init()
{
    pando_gateway_init();

    channel_init(1, CHANNEL_MEMORY, pando_recv_publish_data, pando_subdevice_recv);
}
