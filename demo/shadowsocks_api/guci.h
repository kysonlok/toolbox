#ifndef _GUCI_H
#define _GUCI_H

#include <uci.h>

/**
 * guci_init: Wrapper allocate a new uci context
 */
int guci_init();

/**
 * guci_load: Load an uci config file and store to uci context
 * @config: uci config file name
 */
int guci_load(void);

/**
 * guci_free: Wrapper free a uci context
 */
void guci_free();

/**
 * guci_unload: Unload a config file from uci context
 * @pkg: pointer to the uci_package struct
 */
int guci_unload(struct uci_package *pkg);

/**
 * guci_get: Get uci value
 * @key: <config>.<section>[.<option>]
 * @value: the return buffer
 */
int guci_get(const char *key, char *value);

/**
 * guci_get_idx: Getting nameless section
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @idx: index
 * @option: uci option
 * @value: return buffer
 */
int guci_get_idx(const char *config, const char *section_type, int idx, const char *option, char *value);

/**
 * guci_get_name: Getting named section
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @option: uci option
 * @value: return buffer
 */
int guci_get_name(const char *config, const char *section_name, const char *option, char *value);

/**
 * guci_set: Setting uci
 * @key: <config>.<section>.<option>
 * @value: string
 */
int guci_set(const char *key, const char *value);

/**
 * guci_set_idx: Setting nameless section, call like: guci_set_idx("network", "lan", 0, "proto", "static");
 * @config: uci file name
 * @section_type: uci section, nameless section, only have section type filed
 * @idx: index
 * @option: uci option
 * @value: value to set
 */
int guci_set_idx(const char *config, const char *section_type, int idx, const char *option, const char *value);

/**
 * guci_set_name: Setting named section
 * @config: uci file name
 * @section: uci section, section's name filed
 * @option: uci option
 * @value: value to set
 */
int guci_set_name(const char *config, const char *section_name, const char *option, const char *value);

/**
 * guci_add_named_section: Add a named section
 * @config: uci file name
 * @section_name: uci section, section name
 * @section_type: uci section, section type
 */
int guci_add_named_section(const char *config, const char *section_name, const char *section_type);

/**
 * guci_add_section: Add an nameless section
 * @config: uci config file
 * @section_type: section type
 */ 
int guci_add_section(const char *config, const char *section_type);

/**
 * guci_commit: Commit uci
 * @config: config file
 */
int guci_commit(const char *config);

/**
 * guci_add_list: Append a string to list
 * @key: <config>.<section>.<option>
 * @value: value to append
 */
int guci_add_list(const char *key, const char *value);

/**
 * guci_add_named_list: Append string to list with section
 * @config: uci file name
 * @section_name: uci section, section's name filed
 * @list: uci list
 * @value: value to append
 */
int guci_add_named_list(const char *config, const char *section_name, const char *list, const char *value);

/**
 * guci_add_idx_list: Append string to list with idx
 * @config: uci file name
 * @section_type: uci section, nameless section only exist type filed
 * @idx: index of section
 * @list: uci list
 * @value: value to append
 */
int guci_add_idx_list(const char *config, const char *section_type, int idx, const char *list, const char *value);

/**
 * guci_del_list: Remove a string from list
 * @key: section and option filed
 * @value: value to remove
 */
int guci_del_list(const char *key, const char *value);

/**
 * guci_delete: Delete uci section or option
 * @key: section filed
 */
int guci_delete(const char *key);

#endif
