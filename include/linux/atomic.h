#ifndef _LINUX_ATOMIC_H
#define	_LINUX_ATOMIC_H

/* 
 * BSD atomic functions have arguments in different order
 * compared to Linux atomic_add.
 * Do redefine hackery to set the right arguments.
 */ 
#define	atomic_add	bsd_atomic_add
#define	atomic_sub	bsd_atomic_sub
#include <sys/atomic.h>
#undef	atomic_add
#undef	atomic_sub

#define	atomic_add(i,v)	bsd_atomic_add(v,i)
#define	atomic_sub(i,v)	bsd_atomic_sub(v,i)

static inline int atomic_add_unless(atomic_t *v, int a, int u)
{
	int c, old;

	for (;;) {
		c = atomic_read(v);
		if (unlikely(c == (u)))
			break;
		if (atomic_cmpset(v, c, c + a))
			break;
	}
	return c != (u);
}


#endif
