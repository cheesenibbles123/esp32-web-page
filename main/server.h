/*
 * server.h
 *
 *  Created on: Jul 25, 2021
 *      Author: jinkies
 */
#include "esp_http_server.h"

#ifndef MAIN_SERVER_H_
#define MAIN_SERVER_H_


static httpd_handle_t start_webserver(void);

void init_server(void);

#endif /* MAIN_SERVER_H_ */
