#include "mqtt_function.h"

#include "mqtt_event_connected.h"

int upgrade_client(mqtt_client *this_mqtt, level_opts level)
{
    if (this_mqtt == NULL)
    {
        return -1;
    }
    switch (level)
    {
    case MQTT_CONNECTED:
        this_mqtt->sign_up_fee = mqtt_event_connected_func;
        break;
    default:
        return -2;
    }
    return 0;
}
