/*
 * server.h
 *
 *  Created on: Jul 25, 2021
 *      Author: jinkies
 */
#include "esp_http_server.h"

#ifndef MAIN_SERVER_H_
#define MAIN_SERVER_H_

#define HTML_HELLO "<!DOCTYPE HTML5>\n\
			<html>\n\
			<body>\n\
			Hello!\n\
			</body>\n\
			</html>"

#define HTML_MAIN "<!DOCTYPE HTML5>\n\
			<html>\n\
			<body>\n\
			Random webpage!\n\
			</body>\n\
			</html>"

static httpd_handle_t start_webserver(void);

void init_server(void);

#endif /* MAIN_SERVER_H_ */
