#ifndef __LINUX_PERCPU_H
#define	__LINUX_PERCPU_H

#define	for_each_possible_cpu(x)

static inline void *per_cpu_ptr(void *ptr, unsigned cpu)
{
	return ptr;
}

static inline void *__alloc_percpu(size_t size)
{
	void *ret = kmalloc(size, GFP_KERNEL);
	if (ret)
		memset(ret, 0, size);
	return ret;
}

static inline void free_percpu(void *p)
{
	kfree(p);
}

#define	alloc_percpu(type)	__alloc_percpu(sizeof(type))

#endif
