#include <stdio.h>
#include <stdbool.h>
#include "json_api.h"

int main()
{
	int ret = 0;
	json_object *in = NULL;
	json_object *out = NULL;

	/* parse json from file */
	ret = json_parse_file("./test.json", &in);
	if (ret < 0) {
		printf("failed to parse json file.\n");
		ret = -1;
		goto error;
	}
	printf("%s\n", json_object_to_json_string(in));

	/* test api */
	out = json_object_new_object();
	if (out == NULL) {
		printf("failed to new object.\n");
		ret = -1;
		goto error;
	}

	json_add_string(out, "local", "192.168.1.1");
	json_add_int(out, "port", 8080);
	json_add_boolean(out, "btype", true);
	json_add_double(out, "dtype", 1.23);
	json_add_object(out, "object", in);

	json_object *servers = json_object_new_array();
	json_object_array_add(servers, json_object_new_string("119.12.199.128"));
	json_object_array_add(servers, json_object_new_string("119.12.199.129"));
	json_object_array_add(servers, json_object_new_string("119.12.199.138"));

	json_object_object_add(out, "servers", servers);

	printf("%s\n", json_object_to_json_string(out));

	const char *str_tmp = json_get_string(out, "local");
	printf("get json string: %s\n", str_tmp);

	int int_tmp = json_get_int(out, "port");
	printf("get json int: %d\n", int_tmp);

	bool bool_tmp = json_get_boolean(out, "btype");
	printf("get json bool: %d\n", bool_tmp);

	double double_tmp = json_get_double(out, "dtype");
	printf("get json double: %lf\n", double_tmp);

	json_object *obj_tmp = json_get_object(out, "object");
	printf("get json object: %s\n", json_object_to_json_string(obj_tmp));

	/* save json to file */
	ret = json_save_file("./test_tmp.json", out);
	if (ret < 0) {
		printf("failed to save json to file.\n");
		ret = -1;
		goto error;
	}

error:
	if (in != NULL) {
		json_object_put(in);
	}

	if (out != NULL) {
		json_object_put(out);
	}

	return ret;
}
