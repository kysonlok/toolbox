#include <stdio.h>
#include <mntent.h>
#include <sys/statvfs.h>
#include <string.h>
#include <stdlib.h>

struct mounted_disk_info {
	char dev_node[64];
	char mount_path[64];
	struct statvfs vfs_info;
	struct mounted_disk_info *next;
};

void disk_info_insert(struct mounted_disk_info *list, struct mounted_disk_info *node)
{
	if (list == NULL || node == NULL) {
		return;
	}

	struct mounted_disk_info *current = list;

	while (current->next != NULL) {
		current = current->next;
	}

	current->next = (struct mounted_disk_info *)malloc(sizeof(struct mounted_disk_info));
	strcpy(current->next->dev_node, node->dev_node);
	strcpy(current->next->mount_path, node->mount_path);
	memcpy(&(current->next->vfs_info), &(node->vfs_info), sizeof(struct statvfs));
	current->next->next = NULL;
}

void disk_info_free(struct mounted_disk_info *list)
{
	if (list == NULL) {
		return;
	}

	struct mounted_disk_info *node = list;
	struct mounted_disk_info *tmp = NULL;

	while (node != NULL) {
		tmp = node->next;
		free(node);
		node = tmp;
	}
}

void disk_info(struct mounted_disk_info **disks)
{
	int count = 0;
	FILE *fp = NULL;
	const char *fs_path = "/etc/mtab";
	struct mntent *mnt_entry = NULL;
	struct mounted_disk_info *head = NULL;

	fp = setmntent(fs_path, "r");
	if (fp == NULL) {
		printf("Failed to open filesystem description file\n");
		return;
	}

	/* create head node */
	head = (struct mounted_disk_info *)malloc(sizeof(struct mounted_disk_info));
	memset(head, 0, sizeof(struct mounted_disk_info));
	
	/* for each every node */
	while (1) {
		struct statvfs s;
		struct mounted_disk_info node;
		const char *mnt_point = NULL;

		mnt_entry = getmntent(fp);
		if (mnt_entry == NULL) {
			goto out;
		}

		count++;

		mnt_point = mnt_entry->mnt_dir;
		/* get filesystem statistics */
		statvfs(mnt_point, &s);

		strcpy(node.dev_node, mnt_entry->mnt_fsname);
		strcpy(node.mount_path, mnt_entry->mnt_dir);
		node.vfs_info = s;

		if (count == 1) {
			/* head node */
			strcpy(head->dev_node, node.dev_node);
			strcpy(head->mount_path, node.mount_path);
			memcpy(&(head->vfs_info), &(node.vfs_info), sizeof(struct statvfs));
			head->next = NULL;
		} else {
			/* add node to list, except for head node */
			disk_info_insert(head, &node);
		}
	}

out:
	*disks = head;
	if (fp != NULL) {
		endmntent(fp);
	}
}

void print_all_disk_info(struct mounted_disk_info *list)
{
	struct mounted_disk_info *node = list;

	while (node != NULL) {
		printf("dev_node ===> %s, mount_path ===> %s, total ===> %f MB\n", node->dev_node, node->mount_path, node->vfs_info.f_bsize * node->vfs_info.f_blocks / 1024.0 / 1024.0);
		node = node->next;
	}
}

int main()
{
	int ret = 0;
	struct mounted_disk_info *head = NULL;

	disk_info(&head);

	print_all_disk_info(head);

	disk_info_free(head);

	return ret;
}
