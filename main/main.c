#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "string.h"
#include "nvs_flash.h"
#include "./webConfig.h"

#define SCAN_LIST_SIZE 15

static const char* TAG = "ConnectionSetup";

bool compareStrings(char* str1, char* str2)
{
	//ESP_LOGI(TAG, "Checking -%s-", ssid);
	int val = strcmp(str1, str2);
	if (val == 0)
	{
		return true;
	}
	//ESP_LOGI(TAG, "Got -%d-", val);
	return false;
}

void getWifiConnections()
{
	// Sets to STA mode
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);

	// Sets up default Config
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// Will store each record, init's to size of scan list
	wifi_ap_record_t ap_info[SCAN_LIST_SIZE];
	uint16_t ap_count = 0;
	uint16_t num = SCAN_LIST_SIZE; // converts to uint16_t
	memset(ap_info, 0, sizeof(ap_info));


	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());

	esp_wifi_scan_start(NULL, true); // start scanning
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num, ap_info)); // Get AP records, store into ap_info
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count)); // Get total amount of items, store into ap_count
	ESP_LOGI(TAG, "Total scanned \t\t%u", ap_count);
	bool isFound = false;

	for (int i = 0; (i < SCAN_LIST_SIZE) && (i < ap_count); i++)
	{
	        if (!isFound)
	        {
	        	isFound = compareStrings((char*)ap_info[i].ssid, WIFI_SSID);
	        	if (isFound){
	        		ESP_LOGI(TAG,"Found network!");
	        		ESP_LOGI(TAG,"SSID \t\t%s", ap_info[i].ssid);
	        		ESP_LOGI(TAG,"RSSI \t\t%d", ap_info[i].rssi);
	        		ESP_LOGI(TAG,"Channel \t\t%d\n", ap_info[i].primary);
	        	}
	        }
	}

	if (isFound){
		// do stuff
	}else{
		ESP_LOGI(TAG,"Network not found.");
	}
}

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
	        ESP_ERROR_CHECK(nvs_flash_erase());
	        ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

	getWifiConnections();
}

