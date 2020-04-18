#ifdef _SHADOWSOCKSAPI_H
#define _SHADOWSOCKSAPI_H

#include "json_api.h"

/**
 * @server: ss server address
 * @port: ss server port
 * @password: passwd
 * @encryption: encryption
 * @name: ss server name
 */
int set_client(json_object *input, json_object *output);

/**
 * @name: connect to the specified server
 */
int connect2server(json_object *input, json_object *output);

int disconnect_from_server(json_object *input, json_object *output);

/**
 * @status: disconnected or connecting or connected
 * @recevied: receive n byte data
 * @sent: send n byte data
 * @log: client log
 */
int get_client_status(json_object *input, json_object *output);

/**
 * @port: ss server port, default is 443
 * @password: passwd
 * @encryption: encryption
 */
int set_server(json_object *input, json_object *output);

int start_server(json_object *input, json_object *output);
int stop_server(json_object *input, json_object *output);

/**
 * @status: stopped or started
 * @client: [{ip: '', port: ''}, {ip: '', port: ''}, ...]
 */
int get_server_status(json_object *input, json_object *output);

#endif
