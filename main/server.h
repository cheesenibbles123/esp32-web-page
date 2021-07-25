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
<script type=\"text/javascript\">\
	let ip = window.location.href.split(\"/\")[1];\
	function updateLoop(){\
		let data = fetch(`${ip}/update`).then(response => {\
			if (response.status !== 200){\
				return `Error code: ${response.status}`;\
			}\
			response.json().then(data => {\
				return data;\
			});\
			return null;\
		});\
		let output = `Volume: ${data.Volume}\\nRandom value 1: ${data.val1}\\nRandom value 2: ${data.val2}`;\
		document.getElementById(\"data\").innerHTML = output;\
	}\
	window.onload = function() {\
		updateLoop();\
		setInterval(()=>{\
			updateLoop();\
		},30000); // Every 10 sec\
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
