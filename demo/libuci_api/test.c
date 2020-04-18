#include <stdio.h>
#include "guci.h"

int main()
{
	int ret = 0;

	ret = guci_init();
	if (ret < 0) {
		printf("failed to alloc uci_context.\n");
		return -1;
	}

	/* test guci_add_list api */
	/*
	ret = guci_add_list("network.@interface[0].servers", "hello");
	if (ret != UCI_OK) {
		printf("cannot append list.\n");
		goto err;
	}
	*/

	/*
	ret = guci_add_named_list("network", "interface", "ports", "8080");
	if (ret != UCI_OK) {
		printf("cannot append list.\n");
		goto err;
	}

	ret = guci_add_idx_list("network", "interface", 0, "servers", "world");
	if (ret != UCI_OK) {
		printf("cannot append list.\n");
		goto err;
	}
	*/

	/* test guci_delete api */
	/*
	ret = guci_delete("network.interface.ports");
	if (ret != UCI_OK) {
		printf("cannot delete list filed.\n");
		goto err;
	}
	*/


	/* test guci_add_section api */
	ret = guci_add_section("network", "interface");
	if (ret != UCI_OK) {
		printf("cannot add section.\n");
		goto err;
	}

	ret = guci_commit("network");
	if (ret != UCI_OK) {
		printf("cannot commit config.\n");
		goto err;
	}

	/* test guci_del_list api */
	/*
	ret = guci_del_list("network.@interface[0].servers", "hello");
	if (ret != UCI_OK) {
		printf("cannot delete list.\n");
		goto err;
	}
	*/

	ret = UCI_OK;

err:
	guci_free();
	return ret;
}
