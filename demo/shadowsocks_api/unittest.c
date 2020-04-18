#include <stdio.h>
#include <assert.h>

#include "shadowsocksapi.h"
#include "json_api.h"
#include "guci.h"


int main()
{
	int ret = 0;
	json_object *in = NULL;
	json_object *out = NULL;

	guci_init();

	in = NULL;
	out = json_object_new_object();

#if 0
	/**
	 * @server: ss server address
	 * @port: ss server port
	 * @password: passwd
	 * @encryption: encryption
	 * @name: ss server name
	 */
	in = json_tokener_parse("{\"server\": \"192.168.1.1\", \"port\": \"8080\", \"password\": \"good4you\", \"encryption\": \"rc-md5\", \"name\": \"HongKong\"}");

	ret = set_client(in, out);
	if (ret != 0) {
		printf("err: %s\n", json_get_string(out, "err"));
	}

	ret = connect2server(in, out);
	if (ret != 0) {
		printf("err: %s\n", json_get_string(out, "err"));
	}
	
	/*
	char value[128] = {0};
	ret = guci_get("shadowsocks.@servers[1]", value);
	if (ret != 0) {
		printf("guci get error.\n");
		return -1;
	}

	printf("%s\n", value);
	*/
#endif

	/*
	in = json_tokener_parse("{ \"server\": \"119.29.231.103\", \"server_port\": 443, \"local_address\": \"127.0.0.1\", \"local_port\": 1080, \"password\": \"good4you\", \"timeout\": 300, \"method\": \"rc4-md5\", \"fast_open\": false }");
	*/
#if 0
	in = json_tokener_parse("{\"password\": \"good4you\", \"encryption\": \"rc-md5\"}");

	ret = set_server(in, out);
	if (ret != 0) {
		printf("err: %s\n", json_get_string(out, "err"));
	}

err:
	if (in != NULL) {
		json_object_put(in);
	}

	if (out != NULL) {
		json_object_put(out);
	}
#endif

#if 0
	in = json_object_new_object();
	ret = get_server_status(in, out);
	if (ret != 0) {
		printf("failed to call server_status_uci2json.\n");
	}
	printf("output json string is: \n%s\n", json_object_to_json_string(in));
#endif

	in = json_object_new_object();

	ret = get_client_status(in, out);
	if (ret != 0) {
		printf("failed to call get_client_status.\n");
	}
	printf("output json string is: \n%s\n", json_object_to_json_string(in));
	
	return ret;
}
