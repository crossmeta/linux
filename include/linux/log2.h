#ifndef _LINUX_LOG2_H
#define _LINUX_LOG2_H

#include <linux/types.h>

/*
 *  Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 */

static inline __attribute__((const))
bool_t is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}


static inline int ilog2(unsigned int n)
{
	return fls(n) - 1;
}

static inline unsigned long __roundup_pow_of_two(unsigned long n)
{
	return 1UL << fls(n - 1);
}

#define	order_base_2(n)	ilog2(__roundup_pow_of_two(n))

#endif /* _LINUX_LOG2_H */
