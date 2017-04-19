#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#include <linux/list.h>
#include <linux/time.h>

#define	jiffies	get_ticks()

#ifdef _M_IX86
static inline __int64 __declspec(naked) get_cycles(void)
{
	__asm rdtsc;
	__asm ret;
}
#elif defined(_M_AMD64)
#pragma intrinsic(__rdtsc)
static inline __int64 get_cycles(void)
{
	return __rdtsc();
}
#endif

struct timer_list {
	struct list_head entry;
	unsigned long expires;

	void (*function)(unsigned long);
	unsigned long data;
};


#endif

