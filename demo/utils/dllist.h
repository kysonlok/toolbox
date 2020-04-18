#ifndef DLLIST_H_
#define DLLIST_H_

struct dllist{
	struct dllist *next;
	struct dllist *prev;
};

#define dllist_init(list)						\
do {											\
	(list)->next = (list)->prev = (list);		\
} while (0)

#define dllist_empty(head) ((head)->next == (head) && (head)->prev == (head))

#define dllist_first(head) (head)->next
#define dllist_last(head) (head)->prev

#define dllist_for_each(head, e)				\
for ((e)=(head)->next;							\
	 (e) != (head);								\
	 (e) = (e)->next)

#define dllist_for_each_safe(head, e, t)		\
for((t) = (head)->next->next, (e) = (t)->prev;	\
	(e) != (head);								\
	(t) = (t)->next, (e) = (t)->prev)

#define dllist_detach(e)						\
do {											\
	(e)->prev->next = (e)->next;				\
	(e)->next->prev = (e)->prev;				\
	dllist_init(e);								\
} while (0)

#define dllist_add_tail(head, e)				\
do {											\
	(head)->prev->next = (e);					\
	(e)->prev = (head)->prev;					\
	(head)->prev = (e);							\
	(e)->next = (head);							\
} while (0)

#endif
