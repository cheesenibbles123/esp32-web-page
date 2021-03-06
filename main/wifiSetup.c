/*
 * wifiSetup.c
 *
 *  Created on: Jul 25, 2021
 *      Author: jinkies
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifiSetup.h"
#include "config.h"
#include "server.h"

#define MAXIMUM_RETRYS 5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRYS) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void setupWifi()
{
	s_wifi_event_group = xEventGroupCreate();

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
	                                                        ESP_EVENT_ANY_ID,
	                                                        &event_handler,
	                                                        NULL,
	                                                        &instance_any_id));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
	                                                        IP_EVENT_STA_GOT_IP,
	                                                        &event_handler,
	                                                        NULL,
	                                                        &instance_got_ip));

	wifi_config_t wifiConfig = {
			.sta = {
					.ssid = WIFI_SSID,
					.password = WIFI_AUTH_KEY,
					.threshold.authmode = WIFI_AUTH_WPA2_PSK,
					.pmf_cfg = {
							.capable = true,
							.required = false
					},
			},
	};

	ESP_LOGI(TAG, "Return: %u",esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
	esp_wifi_stop();
	esp_wifi_start();

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
	            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
	            pdFALSE,
	            pdFALSE,
	            portMAX_DELAY);

	if (bits & WIFI_CONNECTED_BIT) {
	    ESP_LOGI(TAG, "Connected to AP SSID:%s", WIFI_SSID);
	} else if (bits & WIFI_FAIL_BIT) {
	    ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_AUTH_KEY);
	} else {
		ESP_LOGI(TAG, "UNEXPECTED EVENT");
	}

	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
	vEventGroupDelete(s_wifi_event_group);

	init_server();

}
