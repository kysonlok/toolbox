#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <glob.h>
#include <libubox/avl.h>
#include <libubox/utils.h>

struct station {
	char *ssid;
	char *key;

	int signal;
};

struct station_node {
	struct avl_node avl;
	struct station *sta;
};

static struct avl_tree stations;

/*
static inline char weight(char c)
{
	return c == '_' ? '-' : c;
}
*/

static int avl_strcmp(const void *k1, const void *k2, void *ptr)
{
	/*
	const char *s1 = k1;
	const char *s2 = k2;

	while (*s1 && (weight(*s1) == weight(*s2)))
	{
		s1++;
		s2++;
	}

	return (unsigned char)weight(*s1) - (unsigned char)weight(*s2);
	*/
	return strcmp(k1, k2);
}

static struct station_node *alloc_station_node(const char *name, struct station *sta)
{
	struct station_node *sn;
	char *_name = NULL;

	sn = calloc_a(sizeof(*sn), &_name, strlen(name) + 1);
	if (sn) {
		sn->avl.key = strcpy(_name, name);
		sn->sta = sta;
		avl_insert(&stations, &sn->avl);
	}
}

static struct station *alloc_station(const char *ssid, const char *key, int signal)
{
	struct station *sta;
	char *_ssid = NULL;
	char *_key = NULL;

	sta = calloc_a(sizeof(*sta), &_ssid, strlen(ssid) + 1, &_key, strlen(key) + 1);
	if (!sta) {
		return NULL;
	}

	sta->ssid = strcpy(_ssid, ssid);
	sta->key = strcpy(_key, key);
	sta->signal = signal;

	alloc_station_node(sta->ssid, sta);
}

static void free_station(struct station *sta)
{
	if (sta) {
		free(sta);
	}
}

static void free_stations(void)
{
	struct station_node *sn, *tmp;
	avl_remove_all_elements(&stations, sn, avl, tmp) {
		struct station *sta = sn->sta;

		free_station(sta);
		free(sn);
	}
}

int main(int argc, char **argv)
{
	int ret = 0;

	int i = 0;
	char ssid[128] = {0};
	char key[128] = {0};
	int signal = 0;
	struct station_node *sn;
	struct station *sta;

	avl_init(&stations, avl_strcmp, false, NULL);

	for (i = 0; i < 10; i++) {
		sprintf(ssid, "SSID%d", i);
		sprintf(key, "key%d", i);
		signal = i;

		alloc_station(ssid, key, signal);
	}

	/* Find key */
	struct station_node *tmp_node;
	tmp_node = avl_find_element(&stations, "SSID5", tmp_node, avl);
	if (tmp_node) {
		struct station *tmp_sta = tmp_node->sta;
		printf("key found, %s %s %d\n", tmp_sta->ssid, tmp_sta->key, tmp_sta->signal);
	}

	/* Traversal tree */
	avl_for_each_element(&stations, sn, avl) {
		sta = sn->sta;
		printf("%s %s %d\n", sta->ssid, sta->key, sta->signal);
	}

	/* Free all nodes and tree*/
	free_stations();

	return ret;
}
