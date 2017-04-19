#ifndef	_LINUX_PERCPU_COUNTER_H
#define	_LINUX_PERCPU_COUNTER_H

#include <linux/percpu.h>

struct percpu_counter {
	atomic_t count;
};

static inline int percpu_counter_init(struct percpu_counter *fbc, s64 amount)
{
	atomic_set(&fbc->count, amount);
	return 0;
}

static inline void
percpu_counter_add(struct percpu_counter *fbc, s64 amount)
{
	atomic_add(amount, &fbc->count);
}

static inline void percpu_counter_inc(struct percpu_counter *fbc)
{
	percpu_counter_add(&fbc->count, 1);
}

static inline void percpu_counter_dec(struct percpu_counter *fbc)
{
	percpu_counter_add(&fbc->count, -1);
}

static inline void percpu_counter_sub(struct percpu_counter *fbc, s64 amount)
{
	percpu_counter_add(&fbc->count, -amount);
}


static inline void percpu_counter_destroy(struct percpu_counter *fbc)
{
}

static inline s64 percpu_counter_sum_positive(struct percpu_counter *fbc)
{
	s64 ret = atomic_read(&fbc->count);
	return ret < 0 ? 0 : ret;
}

static inline s64 percpu_counter_sum(struct percpu_counter *fbc)
{
	return atomic_read(&fbc->count);
}

static inline s64 percpu_counter_read_positive(struct percpu_counter *fbc)
{
	return atomic_read(&fbc->count);
}

#endif

