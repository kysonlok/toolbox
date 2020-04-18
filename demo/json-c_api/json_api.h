#ifndef _JSON_API_H
#define _JSON_API_H

#include <stdbool.h>
#include <json-c/json.h>

/* api for type */
extern void json_add_string(json_object *obj, const char *key, const char *str);
extern void json_add_int(json_object *obj, const char *key, int value);
extern void json_add_boolean(json_object *obj, const char *key, bool bval);
extern void json_add_double(json_object *obj, const char *key, double dval);
extern void json_add_object(json_object *obj, const char *key, json_object *oval);
extern void json_add_array(json_object *obj, const char *key, void *array, int len);

/* api for get */
extern const char *json_get_string(json_object *parent, const char *key);
extern int json_get_int(json_object *parent, const char *key);
extern bool json_get_boolean(json_object *parent, const char *key);
extern double json_get_double(json_object *parent, const char *key);
extern json_object *json_get_object(json_object *parent, const char *key);
extern void *json_get_array(json_object *parent, const char *key);

/* api for del */
extern void json_del_key(json_object *parent, const char *key);

/* api for set */
extern int json_set_string(json_object *parent, const char *key, const char *str);
extern int json_set_int(json_object *parent, const char *key, int value);
extern int json_set_boolean(json_object *parent, const char *key, bool bval);
extern int json_set_double(json_object *parent, const char *key, double dval);
extern int json_set_object(json_object *parent, const char *key, json_object *oval);
extern int json_set_array(json_object *parent, const char *key, void *array, int len);

/* api for file */
extern int json_parse_file(const char *filename, json_object **obj);
extern int json_save_file(const char *filename, json_object *obj);

#endif
