#include <stdio.h>
#include <string.h>

#define offsetof(type, member) ((size_t) & ((type *)0)->member)
#define container_of(ptr, type, member) \
	({const typeof(((type *)0)->member) *_mptr = ptr; \
		(type *)((char *)_mptr - offsetof(type, member));})

struct student {
	int age;
	char *name;
	char id[64];
};

int main()
{
	int ret = 0;
	struct student s1;

	s1.age = 23;
	s1.name = "kysonlok";
	strcpy(s1.id, "12345678");

	size_t offset = offsetof(struct student, age);
	struct student *tmp = container_of(&(s1.name), struct student, name);
	printf("tmp->name: %s, tmp->age: %d, tmp->id: %s\n", tmp->name, tmp->age, tmp->id);
	printf("offset is: %ld\n", offset);
	printf("len: %ld\n", sizeof(s1));
	return ret;
}
