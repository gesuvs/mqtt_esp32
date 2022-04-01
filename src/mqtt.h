#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _level_opts
{
	UNDEFINE = -1,
	MQTT_CONNECTED = 1,
	MQTT_DISCONNECTED = 2,
	MQTT_SUBSCRIBED = 3,
	MQTT_UNSUBSCRIBED = 4,
	MQTT_PUBLISHED = 5,
	MQTT_DATA = 6,
	MQTT_ERROR = 0
} level_opts;

typedef int (*Sign_up_fee_func)(void *this_mqtt);
typedef int (*event_id_func)(void *this_mqtt);

typedef struct _mqtt_client
{
	level_opts level;
	Sign_up_fee_func sign_up_fee;
	event_id_func event_id;

} mqtt_client;

mqtt_client *handle_mqtt_client(level_opts level);

#endif
