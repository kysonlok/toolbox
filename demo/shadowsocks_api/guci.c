#include <stdlib.h>
#include <string.h>
#include <uci.h>
#include "guci.h"

struct uci_context *ctx = NULL;

/**
 * guci_init: Wrapper allocate a new uci context
 */
int guci_init()
{
	ctx = uci_alloc_context();
	if (!ctx)
		return UCI_ERR_MEM;

	return UCI_OK;
}

/**
 * guci_load: Load an uci config file and store to uci context
 * @config: uci config file name
 */
int guci_load(void)
{
	int ret = UCI_OK;

	return ret;
}

/**
 * guci_free: Wrapper free a uci context
 */
void guci_free()
{
	uci_free_context(ctx);
}

/**
 * guci_unload: Unload a config file from uci context
 * @pkg: pointer to the uci_package struct
 */
int guci_unload(struct uci_package *pkg)
{
	int ret = UCI_OK;

	if (pkg == NULL)
		return UCI_ERR_INVAL;

	ret = uci_unload(ctx, pkg);
	if (ret != UCI_OK)
		return ret;

	return ret;
}

void guci_get_option_string(struct uci_option *o, char *value)
{
	if (o == NULL) 
		return;

	struct uci_element *e = NULL;

	switch (o->type) {
	case UCI_TYPE_STRING:
		sprintf(value, "%s", o->v.string);
		break;
	case UCI_TYPE_LIST:
		uci_foreach_element(&o->v.list, e) {
			sprintf(value, "%s ", e->name);
		}
		break;
	default:
		strcpy(value, "");
		break;
	}
}

/**
 * guci_get: Get uci value
 * @key: <config>.<section>[.<option>]
 * @value: the return buffer
 */
int guci_get(const char *key, char *value)
{
	if (key == NULL || value == NULL) 
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_INVAL;
	char *str = NULL;
	struct uci_ptr ptr;
	struct uci_element *e = NULL;

	str = (char *)malloc(strlen(key) + 1);
	strcpy(str, key);

	ret = uci_lookup_ptr(ctx, &ptr, str, true);
	if (ret != UCI_OK)
		goto err;

	e = ptr.last;
	switch (e->type) {
	case UCI_TYPE_SECTION:
		sprintf(value, "%s", ptr.s->type);
		break;
	case UCI_TYPE_OPTION:
		guci_get_option_string(ptr.o, value);
		break;
	default:
		strcpy(value, "");
		goto err;
		break;
	}

	ret = UCI_OK;

err:
	if (str != NULL)
		free(str);

	return ret;
}

/**
 * guci_get_idx: Getting nameless section
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @idx: index
 * @option: uci option
 * @value: return buffer
 */
int guci_get_idx(const char *config, const char *section_type, int idx, const char *option, char *value)
{
	if (config == NULL || section_type == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};

	if (option != NULL)
		sprintf(str, "%s.@%s[%d].%s", config, section_type, idx, option);
	else 
		sprintf(str, "%s.@%s[%d]", config, section_type, idx);

	return guci_get(str, value);
}

/**
 * guci_get_name: Getting named section
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @option: uci option
 * @value: return buffer
 */
int guci_get_name(const char *config, const char *section_name, const char *option, char *value)
{
	if (config == NULL || section_name == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};

	if (option != NULL)
		sprintf(str, "%s.%s.%s", config, section_name, option);
	else
		sprintf(str, "%s.%s", config, section_name);

	return guci_get(str, value);
}

/**
 * guci_set: Setting uci
 * @key: <config>.<section>.<option>
 * @value: string
 */
int guci_set(const char *key, const char *value)
{
	if (key == NULL || value == NULL)
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_UNKNOWN;
	struct uci_ptr ptr;
	char *str = NULL;

	str = (char *)malloc(strlen(key) + strlen(value) + 3);
	if (str == NULL)
		return UCI_ERR_MEM;

	sprintf(str, "%s=%s", key, value);
	if (uci_lookup_ptr(ctx, &ptr, str, true) != UCI_OK) 
		goto err;

	if (uci_set(ctx, &ptr) != UCI_OK)
		goto err;

	if (uci_save(ctx, ptr.p) != UCI_OK)
		goto err;

	ret = UCI_OK;
err:
	if (str != NULL)
		free(str);

	return ret;
}

/**
 * guci_set_idx: Setting nameless section, call like: guci_set_idx("network", "lan", 0, "proto", "static");
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @idx: index
 * @option: uci option
 * @value: value to set
 */
int guci_set_idx(const char *config, const char *section_type, int idx, const char *option, const char *value)
{
	if (config == NULL || section_type == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};

	if (option != NULL)
		sprintf(str, "%s.@%s[%d].%s", config, section_type, idx, option);
	else 
		sprintf(str, "%s.@%s[%d]", config, section_type, idx);

	return guci_set(str, value);
}

/**
 * guci_set_name: Setting named section
 * @config: uci file name
 * @section: uci section, section's name filed
 * @option: uci option
 * @value: value to set
 */
int guci_set_name(const char *config, const char *section_name, const char *option, const char *value)
{
	if (config == NULL || section_name == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};

	if (option != NULL)
		sprintf(str, "%s.%s.%s", config, section_name, option);
	else
		sprintf(str, "%s.%s", config, section_name);

	return guci_set(str, value);
}

/**
 * guci_add_named_section: Add a named section
 * @config: uci file name
 * @section_name: uci section, section name
 * @section_type: uci section, section type
 */
int guci_add_named_section(const char *config, const char *section_name, const char *section_type)
{
	if (config == NULL || section_name == NULL || section_type == NULL)
		return UCI_ERR_INVAL;

	return guci_set_name(config, section_name, NULL, section_type);
}

/**
 * guci_add_section: Add an nameless section
 * @config: uci config file
 * @section_type: section type
 */ 
int guci_add_section(const char *config, const char *section_type)
{
	if (config == NULL || section_type == NULL)
		return UCI_ERR_INVAL;

    int ret = UCI_OK;
	struct uci_package *p = NULL;
    struct uci_section *s = NULL;

	ret = uci_load(ctx, config, &p);
	if (ret != UCI_OK)
		return ret;

    ret = uci_add_section(ctx, p, section_type, &s);
    if (ret != UCI_OK)
		return ret;

    ret = uci_save(ctx, p);

	if (p != NULL)
		uci_unload(ctx, p);
	
	return ret;
}

/**
 * guci_commit: Commit uci
 * @config: config file
 */
int guci_commit(const char *config)
{
	if (config == NULL)
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_UNKNOWN;
	struct uci_ptr ptr;
	char *str = NULL;

	str = (char *)malloc(strlen(config) + 1);
	if (str == NULL)
		return UCI_ERR_MEM;

	strcpy(str, config);

	if (uci_lookup_ptr(ctx, &ptr, str, true) != UCI_OK) {
		goto err;
	}

	if (uci_commit(ctx, &ptr.p, false) != UCI_OK)
		goto err;

	ret = UCI_OK;
err:
	if (str != NULL)
		free(str);

	return ret;
}

/**
 * guci_add_list: Append a string to list
 * @key: <config>.<section>.<option>
 * @value: value to append
 */
int guci_add_list(const char *key, const char *value)
{
	if (key == NULL || value == NULL)
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_UNKNOWN;
	char *str = NULL;
	struct uci_ptr ptr;

	str = (char *)malloc(strlen(key) + strlen(value) + 3);
	if (str == NULL)
		return UCI_ERR_MEM;

	sprintf(str, "%s=%s", key, value);

	ret = uci_lookup_ptr(ctx, &ptr, str, true);
	if (ret != UCI_OK)
		goto err;

	ret = uci_add_list(ctx, &ptr);
	if (ret != UCI_OK)
		goto err;

	ret = uci_save(ctx, ptr.p);

err:
	if (str != NULL)
		free(str);

	return ret;
}

/**
 * guci_add_named_list: Append string to list with section
 * @config: uci file name
 * @section_name: uci section, section's name filed
 * @list: uci list
 * @value: value to append
 */
int guci_add_named_list(const char *config, const char *section_name, const char *list, const char *value)
{
	if (config == NULL || section_name == NULL || list == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};
	sprintf(str, "%s.%s.%s", config, section_name, list);
	
	return guci_add_list(str, value);
}

/**
 * guci_add_idx_list: Append string to list with idx
 * @config: uci file name
 * @section_type: uci section, nameless section only exist type filed
 * @idx: index of section
 * @list: uci list
 * @value: value to append
 */
int guci_add_idx_list(const char *config, const char *section_type, int idx, const char *list, const char *value)
{
	if (config == NULL || section_type == NULL || list == NULL || value == NULL)
		return UCI_ERR_INVAL;

	char str[128] = {0};
	sprintf(str, "%s.@%s[%d].%s", config, section_type, idx, list);
	
	return guci_add_list(str, value);
}

/**
 * guci_del_list: Remove a string from list
 * @key: section and option filed
 * @value: value to remove
 */
int guci_del_list(const char *key, const char *value)
{
	if (key == NULL || value == NULL)
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_UNKNOWN;
	char *str = NULL;
	struct uci_ptr ptr;

	str = (char *)malloc(strlen(key) + strlen(value) + 3);
	if (str == NULL)
		return UCI_ERR_MEM;

	sprintf(str, "%s=%s", key, value);

	ret = uci_lookup_ptr(ctx, &ptr, str, true);
	if (ret != UCI_OK)
		goto err;

	ret = uci_del_list(ctx, &ptr);
	if (ret != UCI_OK)
		goto err;

	ret = uci_save(ctx, ptr.p);

err:
	if (str != NULL)
		free(str);

	return ret;
}

/**
 * guci_delete: Delete uci section or option
 * @key: section filed
 */
int guci_delete(const char *key)
{
	if (key == NULL)
		return UCI_ERR_INVAL;

	int ret = UCI_ERR_UNKNOWN;
	char *str = NULL;
	struct uci_ptr ptr;

	str = (char *)malloc(strlen(key) + 1);
	if (str == NULL)
		return UCI_ERR_MEM;

	strcpy(str, key);

	if (uci_lookup_ptr(ctx, &ptr, str, true) != UCI_OK)
		goto err;

	if (uci_delete(ctx, &ptr) != UCI_OK)
		goto err;

	if (uci_save(ctx, ptr.p) != UCI_OK)
		goto err;

	ret = UCI_OK;
err:
	if (str != NULL)
		free(str);

	return ret;
}

