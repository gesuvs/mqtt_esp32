#include "mqtt.h"

extern int upgrade_client(mqtt_client *this_mqtt, level_opts level);

static int _msg_id_func(mqtt_client *this_mqtt);

mqtt_client *handle_mqtt_client(level_opts level)
{

	mqtt_client *client = (mqtt_client *)malloc(sizeof(mqtt_client));

	if (client == NULL)
	{
		return NULL;
	}
	do
	{
		if (upgrade_client(client, level) != 0)
		{
			break;
		}
		client->event_id = _msg_id_func;
		return client;
	} while (0);

	free(client);

	return NULL;
}

static int _msg_id_func(mqtt_client *this_mqtt)
{
	if (this_mqtt == NULL)
	{
		return -1;
	}
	int fee = 0;
	fee += this_mqtt->sign_up_fee(this_mqtt);
	return fee;
}
