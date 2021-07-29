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

#define HTML_MAIN "<!DOCTYPE html>\
<html lang=\"en-UK\">\
<head>\
</head>\
<script type=\"text/javascript\">\
	let ip = window.location.href.split(\"/\")[1];\
	function updateLoop(){\
		fetch(`${ip}/update`).then(response => response.json()).then(data => {\
			let output = `Volume: ${data.Volume}<br>Random value 1: ${data.val1}<br>Random value 2: ${data.val2}`;\
			document.getElementById(\"data\").innerHTML = output;\
		});\
	}\
	window.onload = function() {\
		updateLoop();\
		setInterval(()=>{\
			updateLoop();\
		},30000);\
	}\
</script>\
<body>\
<div id=\"data\">\
	Getting Data....\
</div>\
</body>\
</html>"

void init_server(void);

#endif /* MAIN_SERVER_H_ */
