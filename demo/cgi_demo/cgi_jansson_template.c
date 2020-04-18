#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cgi.h"
#include "jansson.h"


int test_command_run(json_t *json_in, json_t *json_out)
{
	json_object_set_new(json_out, "Response", json_string("NotImplemented"));
	return 0;
}

struct command_handler {
	char *command;
	int (*action)(json_t *json_in, json_t *json_out);
};

static struct command_handler cmds[] = {
	{ "TestCommand1", test_command_run},
	{ NULL, NULL}
};

int main()
{
	json_t *json_in;
	json_t *json_out;
	json_t *object;
	json_error_t error;
	char buffer[CGI_POST_BUF_MAX];
	const char *command;
	char *env;
	char *out;
	int length;
	int i;
	int ret = 0;

	/* Initialize output JSON object */
	json_out = json_object();
	if (!json_out)
		return -ENOMEM;

	/* Get the content length and check enough buffer space */
	env = getenv("CONTENT_LENGTH");
	length = env ? atoi(env) : 0;
	if (length >= CGI_POST_BUF_MAX) {
		json_object_set_new(json_out, "Response", json_string("Content Length to large"));
		goto error;
	}

	/* Read the stdin CGI input */
	if (length == 0) {
		while (1) {
			env = fgets(buffer+length, CGI_POST_BUF_MAX, stdin);
			if (env[0] == ';')
				break;
			length += strlen(env);
		}
	} else {
		ret = fread(buffer, sizeof(char), length, stdin);
	}

	/* Form input JSON object */
	json_in = json_loadb(buffer, length, 0, &error);
	if (!json_in) {
		cgi_log("json error: [%s][%d][%d][%d] - [%s]\n", error.source, error.line, error.column, error.position, error.text);
		goto error;
	}

	/* Extrack key-values needed */
	object = json_object_get(json_in, "Command");
	if (!object) {
		cgi_log("No command found\n");
		ret = -EINVAL;
		goto error;
	}

	command = json_string_value(object);

	for (i = 0; 1; i++) {
		if (cmds[i].command == NULL) {
			cgi_log("Command [%s] not handled\n", command);
			break;
		}

		if (!strcmp(cmds[i].command, command)) {
			cmds[i].action(json_in, json_out);
			break;
		}
	}

error:

	/* Dump to standard out */
	out = json_dumps(json_out, JSON_PRESERVE_ORDER);
	printf("Content-type: application/json; charset=utf-8\n\n");
	printf("%s\n", out);
	free(out);

	/* Deinitialize JSON objects */
	json_decref(json_in);
	json_decref(json_out);

	return ret;
}

