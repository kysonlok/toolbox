#include <stdio.h>
#include <uci.h>
#include <uci_blob.h>
#include <libubox/blob.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

static struct uci_context *uci_ctx;
static struct blob_buf b;

enum {
	POLICY_ATTR_STRING,
	POLICY_ATTR_INT,
	POLICY_ATTR_BOOL,
	POLICY_ATTR_ARRAY,
	__POLICY_ATTR_MAX
};

struct blobmsg_policy policy_attrs[__POLICY_ATTR_MAX] = {
	[POLICY_ATTR_STRING] = { .name = "string", .type = BLOBMSG_TYPE_STRING },
	[POLICY_ATTR_INT] = { .name = "int", .type = BLOBMSG_TYPE_INT32 },
	[POLICY_ATTR_BOOL] = { .name = "bool", .type = BLOBMSG_TYPE_BOOL },
	[POLICY_ATTR_ARRAY] = { .name = "array", .type = BLOBMSG_TYPE_ARRAY },
};

struct uci_blob_param_info policy_attr_info[__POLICY_ATTR_MAX] = {
	[POLICY_ATTR_ARRAY] = { .type = BLOBMSG_TYPE_STRING },
};

struct uci_blob_param_list policy_attr_list = {
	.n_params = __POLICY_ATTR_MAX,
	.params = policy_attrs,
	.info = policy_attr_info,
};

int main(int argc, char **argv)
{
	int ret = 0;
	char *str = NULL;
	struct uci_package *pkg;
	struct uci_element *e;
	char name[] = "test";

	/* create uci context */
	uci_ctx = uci_alloc_context();
	if (!uci_ctx) {
		printf("Failed to create uci context\n");
		return -1;
	}

	/* if package has loaded to memory, unload it firstly */
	pkg = uci_lookup_package(uci_ctx, name);
	if (pkg) {
		uci_unload(uci_ctx, pkg);
	}

	/* initial blob buffer */
	blob_buf_init(&b, 0);

	uci_load(uci_ctx, name, &pkg);

	/* foreach uci section */
	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);

		uci_to_blob(&b, s, &policy_attr_list);
	}

	/* blob protocol to json format */
	str = blobmsg_format_json(b.head, true);
	if (str) {
		printf("%s\n", str);
	}

out:
	if (str) {
		free(str);
	}

	if (pkg) {
		uci_unload(uci_ctx, pkg);
	}

	if (uci_ctx) {
		uci_free_context(uci_ctx);
	}

	return ret;
}
