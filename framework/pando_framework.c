#include "gateway/pando_channel.h"
#include "pando_subdevice.h"
#include "pando_framework.h"
#include "pando_types.h"
#include "gateway/pando_gateway.h"

void FUNCTION_ATTRIBUTE
pando_framework_init()
{
    pando_gateway_init();

    on_subdevice_channel_recv(PANDO_CHANNEL_PORT_1, pando_subdevice_recv);
}
