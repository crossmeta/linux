#ifndef __LINUX_RCUPDATE_H
#define	__LINUX_RCUPDATE_H

struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *head);
};


extern rwsleep_t kernel_rculock;
#define rcu_read_lock() 	RWSLEEP_RDLOCK(&kernel_rculock)
#define rcu_read_unlock() 	RWSLEEP_UNLOCK(&kernel_rculock)
#define	rcu_barrier()

static inline void call_rcu(struct rcu_head *head,
                              void (*func)(struct rcu_head *head))
{
	RWSLEEP_WRLOCK(&kernel_rculock);
	func(head);
	RWSLEEP_UNLOCK(&kernel_rculock);
}

static inline void list_add_rcu(struct list_head *new, struct list_head *head)
{
	list_add(new, head);
}

static inline void list_del_rcu(struct list_head *entry)
{
	list_del(entry);
}

static inline void list_add_tail_rcu(struct list_head *new,
					struct list_head *head)
{
	__list_add(new, head->prev, head);
}

#define	rcu_dereference(ptr)	(ptr)

#define	list_entry_rcu(ptr, type, member) \
		container_of(rcu_dereference(ptr), type, member)

#define	list_for_each_entry_rcu(type, pos, head, member) \
	for (pos = list_entry_rcu((head)->next, type, member); \
		&pos->member != (head); \
		pos = list_entry_rcu(pos->member.next, type, member))

#endif
