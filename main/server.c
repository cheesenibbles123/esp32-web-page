/*
 * fileServer.c
 *
 *  Created on: Jul 25, 2021
 *      Author: jinkies
 */
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "cJSON.h"

#include "server.h"
#include "config.h"

const char *type = "text/html";

static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static uint16_t volume = 35;
static uint16_t val1 = 15;
static uint16_t val2 = 52;

/* An HTTP GET handler */
static esp_err_t main_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Type", "Info");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static esp_err_t update_get_handler(httpd_req_t *req){
	httpd_resp_set_hdr(req, "Type", "Update");

	/* Send response with custom headers and body set as the
	* string passed in user context*/
	/*char * val = strcat("{\"Volume\" : ",(char *)volume);
	val = strcat(val,",\"val1\" :");
	val = strcat(val,(char *)val1);
	val = strcat(val,", \"val2\" : ");
	val = strcat(val,(char *)val2);
	val = strcat(val,"}");*/
	cJSON *obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(obj, "Volume", volume);
	cJSON_AddNumberToObject(obj, "val1", val1);
	cJSON_AddNumberToObject(obj, "val2", val2);

	char *objData = cJSON_PrintUnformatted(obj);
	ESP_LOGI(TAG, "%s",objData);

	httpd_resp_send(req, objData, HTTPD_RESP_USE_STRLEN);

	/* After sending the HTTP response the old HTTP request
	* headers are lost. Check if HTTP request headers can be read now. */
	if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
		ESP_LOGI(TAG, "Request headers lost");
	}
	return ESP_OK;
}


static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = HTML_HELLO
};

static const httpd_uri_t info = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = main_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = HTML_MAIN
};

static const httpd_uri_t update = {
    .uri       = "/update",
    .method    = HTTP_GET,
    .handler   = update_get_handler,
	.user_ctx  = HTML_MAIN
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &info);
        httpd_register_uri_handler(server, &update);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        httpd_stop(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void init_server(void)
{
	static httpd_handle_t server = NULL;

	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server);
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server);

	server = start_webserver();
	ESP_LOGI(TAG, "Server started.");
}
