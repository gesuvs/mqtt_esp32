#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

// #include "strategy_pattern.h"

static const char *TAG = "MQTT_EXAMPLE";

static void wifi_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
  ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

  switch (event_id)
  {
  case WIFI_EVENT_STA_START:
    ESP_LOGI(TAG, "WIFI_EVENT_STA_START\n");
    break;
  case WIFI_EVENT_STA_CONNECTED:
    ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED\n");
    break;
  case WIFI_EVENT_STA_DISCONNECTED:
    ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED\n");
    break;
  case IP_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP\n");
    break;
  default:
    ESP_LOGI(TAG, "Other event id:%d\n", event_id);
    break;
  }
  // mqtt_client *mqtt_client = handle_mqtt_client(event_id);
  // mqtt_client->event_id(mqtt_client);
}

void wifi_connection()
{
  ESP_LOGI(TAG, "WIFI CONNECTION START...\n");

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_initiation));

  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

  wifi_config_t wifi_configuration = {
      .sta = {
          .ssid = "gesuvs",
          .password = "gesuvs968778"}};

  ESP_LOGI(TAG, "Connecting to %s...\n", wifi_configuration.sta.ssid);
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration));

  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_connect());
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
  esp_mqtt_client_handle_t client = event->client;

  switch (event->event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED\n");
    esp_mqtt_client_subscribe(client, "mqtt_topic", 1);
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED\n");
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED\n");
    break;

  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED\n");
    break;

  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED\n");
    break;

  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "DATA RECEIVED: %.*s\r\n", event->data_len, event->data);
    break;

  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR\n");
    break;

  default:
    ESP_LOGI(TAG, "OTHER EVENT ID: %d\n", event->event_id);
    break;
  }
  return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  ESP_LOGI(TAG, "Event dispatched from event loop base:%s, event_id:%d\n", base, event_id);
  mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start()
{
  ESP_LOGI(TAG, "SETUP APP START");

  esp_mqtt_client_config_t mqtt_cfg = {
      .uri = "mqtt://192.168.15.71",
      .port = 1883};

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void app_main()
{
  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
  esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
  esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
  esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
  esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
  esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_connection();

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  printf("WIFI was initiated...\n");
  mqtt_app_start();
}
