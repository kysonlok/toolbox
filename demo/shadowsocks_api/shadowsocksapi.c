#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "shadowsocksapi.h"
#include "json_api.h"
#include "guci.h"

extern struct uci_context *ctx;

#define MAX_FILE_SIZE 10*1024
#define SERVER_LOG_FILE "/var/log/ss-server.log"
#define CLIENT_LOG_FILE "/var/log/ss-client.log"

#define SERVER_JSON_FILE "/etc/shadowsocks-sever.json"

struct client_uci_info {
	char *alias;
	char *server;
	char *server_port;
	char *timeout;
	char *password;
	char *encrypt_method;
};

/* external api */
static int set_main_server(char *name);
static int set_client_to_uci(struct client_uci_info cli);
static int server_status_uci2json(json_object *input);
static ssize_t read_log_file(const char *filename, char *buf, ssize_t len);

static int set_main_server(char *srv_name)
{
	/* TODO */
	if (srv_name == NULL) 
		return -1;

	int ret = 0;
	struct uci_package *uci_config;

	/* foreach */
	struct uci_element *e = NULL; 
	struct uci_section *s = NULL;

	/* section name */
	char *section_name = NULL;

	/* load uci config to ctx */
	uci_config = uci_lookup_package(ctx, "shadowsocks");
	if (uci_config != NULL)
		return -1;

	ret = uci_load(ctx, "shadowsocks", &uci_config);
	if (ret != 0)
		return ret;

	/* foreach section */
	uci_foreach_element(&uci_config->sections, e) {
		s = uci_to_section(e);

		/* skip the section type isn't servers */
		if (strcmp(s->type, "servers"))
			continue;

		/* skip no alias option section */
		const char *name = uci_lookup_option_string(ctx, s, "alias");
		if (name == NULL)
			continue;

		/* check ip exist or not */
		if (!strcmp(name, srv_name)) {
			/* set shadowsocks.@transparent_proxy[0].main_server */
			section_name = s->e.name;
			if (section_name != NULL) {
				guci_set("shadowsocks.@transparent_proxy[0].main_server", section_name);
			}
			goto out;
		}
	}

out:
	uci_commit(ctx, &uci_config, false);
	uci_save(ctx, uci_config);
	if (uci_config != NULL)
		uci_unload(ctx, uci_config);

	return ret;
}

static int set_client_to_uci(struct client_uci_info cli)
{
	/* TODO */
	int ret = 0;
	struct uci_package *uci_config;

	/* foreach */
	int idx = -1;
	struct uci_element *e = NULL; 
	struct uci_section *s = NULL;

	// shadowsocks.@servers
	/* load uci config to ctx */
	uci_config = uci_lookup_package(ctx, "shadowsocks");
	if (uci_config != NULL)
		return -1;

	ret = uci_load(ctx, "shadowsocks", &uci_config);
	if (ret != 0)
		return ret;

	/* foreach section */
	uci_foreach_element(&uci_config->sections, e) {
		s = uci_to_section(e);

		/* skip the section type isn't servers */
		if (strcmp(s->type, "servers"))
			continue;

		/* section type index */
		idx++;

		/* skip no alias option section */
		const char *name = uci_lookup_option_string(ctx, s, "alias");
		if (name == NULL)
			continue;

		/* check ip exist or not */
		if (!strcmp(name, cli.alias)) {
			guci_set_idx("shadowsocks", "servers", idx, "server", cli.server);
			guci_set_idx("shadowsocks", "servers", idx, "server_port", cli.server_port);
			guci_set_idx("shadowsocks", "servers", idx, "timeout", cli.timeout);
			guci_set_idx("shadowsocks", "servers", idx, "password", cli.password);
			guci_set_idx("shadowsocks", "servers", idx, "encrypt_method", cli.encrypt_method);
			goto out;
		}
	}

	/* add client info to uci */
	uci_add_section(ctx, uci_config, "servers", &s);
	guci_set_idx("shadowsocks", "servers", idx+1, "alias", cli.alias);
	guci_set_idx("shadowsocks", "servers", idx+1, "server", cli.server);
	guci_set_idx("shadowsocks", "servers", idx+1, "server_port", cli.server_port);
	guci_set_idx("shadowsocks", "servers", idx+1, "timeout", cli.timeout);
	guci_set_idx("shadowsocks", "servers", idx+1, "password", cli.password);
	guci_set_idx("shadowsocks", "servers", idx+1, "encrypt_method", cli.encrypt_method);

out:
	uci_commit(ctx, &uci_config, false);
	uci_save(ctx, uci_config);
	if (uci_config != NULL)
		uci_unload(ctx, uci_config);

	return 0;
}

static ssize_t read_log_file(const char *filename, char *buf, ssize_t len)
{
	int fd = 0;
	ssize_t bytes = 0;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("cannot open file: %s\n", filename);
		return -1;
	}
	
	bytes = read(fd, buf, len - 1);
	buf[bytes] = '\0';

	close(fd);

	return bytes;
}

static int server_status_uci2json(json_object *input)
{

	assert(input != NULL);

	int ret = 0;
	struct uci_package *uci_config;

	json_object *jobj = NULL;
	json_object *jarray = json_object_new_array();

	/* foreach */
	struct uci_element *e = NULL; 
	struct uci_section *s = NULL;

	char buf[MAX_FILE_SIZE] = {0};
	ssize_t len = MAX_FILE_SIZE;

	/* ss server is running or not */
#if 0
	ret = system("pidof ss-server >/dev/null");
	if (ret != 0) {
		/* ss server stop */
		json_add_string(input, "status", "stopped");
		json_object_object_add(input, "clients", jarray);
		json_add_string(input, "log", " ");
		return 0;
	}
#endif

	json_add_string(input, "status", "started");

	len = read_log_file(SERVER_LOG_FILE, buf, len);
	if (len < 0) {
		/* error, print null string */
		json_add_string(input, "log", " ");
	} else {
		json_add_string(input, "log", buf);
	}

	/* load uci config to ctx */
	uci_config = uci_lookup_package(ctx, "ss_api");
	if (uci_config != NULL)
		return -1;

	ret = uci_load(ctx, "ss_api", &uci_config);
	if (ret != 0)
		return ret;

	/* foreach section */
	uci_foreach_element(&uci_config->sections, e) {
		s = uci_to_section(e);

		/* skip the section type isn't servers */
		if (strcmp(s->type, "server"))
			continue;

		/* skip no alias option section */
		const char *cli_ip = uci_lookup_option_string(ctx, s, "cli_ip");
		const char *cli_port = uci_lookup_option_string(ctx, s, "cli_port");
		if (cli_ip != NULL && cli_port != NULL) {
			jobj = json_object_new_object();
			json_add_string(jobj, cli_ip, cli_port);
			json_object_array_add(jarray, jobj);
		}
	}

	json_object_object_add(input, "clients", jarray);

out:
	if (uci_config != NULL)
		uci_unload(ctx, uci_config);

	return ret;
}

/**
 * @server: ss server address
 * @port: ss server port
 * @password: passwd
 * @encryption: encryption
 * @name: ss server name
 */
int set_client(json_object *input, json_object *output)
{
	assert(input != NULL);
	assert(output != NULL);

	int ret = 0;

	char *srv_ip = NULL;
	char *srv_port = NULL;
	char *passwd = NULL;
	char *enc = NULL;
	char *name = NULL;
	char *timeout = NULL;

	struct client_uci_info cli;

	srv_ip = json_get_string(input, "server");
	srv_port = json_get_string(input, "port");
	passwd = json_get_string(input, "password");
	enc = json_get_string(input, "encryption");
	name = json_get_string(input, "name");

	if (srv_ip == NULL || srv_port == NULL || passwd == NULL || enc == NULL || name == NULL) {
		json_add_string(output, "err", "invalid input");
		ret = -1;
		return ret;
	}

	cli.alias = name;
	cli.server = srv_ip;
	cli.server_port = srv_port;
	cli.timeout = (timeout?timeout:"60");
	cli.password = passwd;
	cli.encrypt_method = enc;

	ret = set_client_to_uci(cli);
	if (ret != 0) {
		json_add_string(output, "err", "cannot set client info to uci file");
		return ret;
	}

	return ret;
}

/**
 * @name: connect to the specified server
 */
int connect2server(json_object *input, json_object *output)
{
	assert(input != NULL);
	assert(output != NULL);

	int ret = 0;
	char *name = NULL;

	name = json_get_string(input, "name");
	if (name == NULL) {
		json_add_string(output, "err", "invalid input");
		ret = -1;
		return ret;
	}

	/* set shadowsocks.@transparent_proxy[0].main_server */
	ret = set_main_server(name);
	if (ret != 0) {
		json_add_string(output, "err", "cannot set main server to transparent proxy");
		return ret;
	}

	/* should be start chinadns and dns-forwarder */
	/* TODO */

	/* start ss-redir */
	system("/etc/init.d/shadowsocks start");

	return 0;
}

int disconnect_from_server(json_object *input, json_object *output)
{
	/* stop ss-redir */
	system("/etc/init.d/shadowsocks stop");

	/* should be stop chinadns and dns-forwarder */
	/* TODO */

	return 0;
}

/**
 * @status: disconnected or connecting or connected
 * @recevied: receive n byte data
 * @sent: send n byte data
 * @log: client log
 */
int get_client_status(json_object *input, json_object *output)
{
	int ret = 0;
	struct uci_package *uci_config;

	/* foreach */
	struct uci_element *e = NULL; 
	struct uci_section *s = NULL;

	char buf[MAX_FILE_SIZE] = {0};
	ssize_t len = MAX_FILE_SIZE;

	/* ss server is running or not */
#if 0
	ret = system("pidof ss-redir >/dev/null");
	if (ret != 0) {
		/* ss server stop */
		json_add_string(input, "status", "disconnected");
		json_add_string(input, "recevied", "0");
		json_add_string(input, "sent", "0");
		json_add_string(input, "log", " ");
		return 0;
	}
#endif

	json_add_string(input, "status", "connected");

	len = read_log_file(CLIENT_LOG_FILE, buf, len);
	if (len < 0) {
		/* error, print null string */
		json_add_string(input, "log", " ");
	} else {
		json_add_string(input, "log", buf);
	}

	/* load uci config to ctx */
	uci_config = uci_lookup_package(ctx, "ss_api");
	if (uci_config != NULL)
		return -1;

	ret = uci_load(ctx, "ss_api", &uci_config);
	if (ret != 0)
		return ret;

	/* foreach section */
	uci_foreach_element(&uci_config->sections, e) {
		s = uci_to_section(e);

		/* skip the section type isn't servers */
		if (strcmp(s->type, "client"))
			continue;

		/* skip no alias option section */
		const char *srv_ip = uci_lookup_option_string(ctx, s, "srv_ip");
		const char *srv_port = uci_lookup_option_string(ctx, s, "srv_port");
		const char *recevied = uci_lookup_option_string(ctx, s, "cli_recv");
		const char *sent = uci_lookup_option_string(ctx, s, "cli_sent");
		if (srv_ip != NULL) {
			json_add_string(input, "srv_ip", srv_ip);
		} else {
			json_add_string(input, "srv_ip", "null");
		}

		if (srv_port != NULL) {
			json_add_string(input, "srv_port", srv_port);
		} else {
			json_add_string(input, "srv_port", "null");
		}

		if (recevied != NULL) {
			json_add_string(input, "recevied", recevied);
		} else {
			json_add_string(input, "recevied", "0");
		}

		if (sent != NULL) {
			json_add_string(input, "sent", sent);
		} else {
			json_add_string(input, "sent", "0");
		}
	}

out:
	if (uci_config != NULL)
		uci_unload(ctx, uci_config);

	return ret;
}

/**
 * @port: ss server port, default is 443
 * @password: passwd
 * @encryption: encryption
 */
int set_server(json_object *input, json_object *output)
{
	assert(input != NULL);
	assert(output != NULL);

	int ret = 0;
	json_object *tmp = NULL;

	/* create a temperature json object to save json file content */
	tmp = json_object_new_object();
	if (tmp == NULL) {
		json_add_string(output, "err", "failed to create json object");
		ret = -1;
		return ret;
	}

	/* read server json file */
	ret = json_parse_file(SERVER_JSON_FILE, &tmp);
	if (ret != 0) {
		json_add_string(output, "err", "failed to parse json file");
		goto err;
	}

	/* save json input element to ss_info */
	const char *server = json_get_string(input, "server");
	const char *server_port = json_get_string(input, "port");
	const char *local_address = json_get_string(input, "local_address");
	const char *local_port = json_get_string(input, "local_port");
	const char *password = json_get_string(input, "password");
	const char *timeout = json_get_string(input, "timeout");
	const char *method = json_get_string(input, "encryption");
	const char *fast_open = json_get_string(input, "fast_open");

	if (server != NULL) {
		json_set_string(tmp, "server", server);
	}

	if (server_port != NULL) {
		json_set_string(tmp, "server_port", server_port);
	}

	if (local_address != NULL) {
		json_set_string(tmp, "local_address", local_address);
	}
	
	if (local_port != NULL) {
		json_set_string(tmp, "local_port", local_port);
	}
	
	if (password != NULL) {
		printf("password is %s\n", password);
		json_set_string(tmp, "password", password);
	}
	
	if (timeout != NULL) {
		json_set_string(tmp, "timeout", timeout);
	}
	
	if (method != NULL) {
		printf("methon is %s\n", method);
		json_set_string(tmp, "method", method);
	}
	
	if (fast_open != NULL) {
		json_set_string(tmp, "fast_open", fast_open);
	}

	ret = json_save_file(SERVER_JSON_FILE, tmp);
	if (ret != 0) {
		json_add_string(output, "err", "failed to save json file");
		goto err;
	}

err:
	if (tmp != NULL) {
		json_object_put(tmp);
	}

	return ret;
}

int start_server(json_object *input, json_object *output)
{
	/* start ss server */
	system("/usr/bin/ss-server -c /etc/shadowsocks-sever.json -u -f /var/run/ss-server.pid");

	return 0;
}

int stop_server(json_object *input, json_object *output)
{
	system("kill -9 $(pidof ss-server) >/dev/null 2>&1");
	system("rm -fr /var/run/ss-server.pid");
	return 0;
}

/**
 * @status: stopped or started
 * @client: [{ip: '', port: ''}, {ip: '', port: ''}, ...]
 */
int get_server_status(json_object *input, json_object *output)
{
	return server_status_uci2json(input);
}

