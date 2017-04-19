#ifndef _LINUX_TIME_H
#define	_LINUX_TIME_H

#include <linux/types.h>
#include <linux/kernel.h>

typedef timespec_t	ktime_t;

#define MSEC_PER_SEC    1000L
#define	NSEC_PER_SEC	1000000000L
#define CURRENT_TIME	time_second()
#define	CURRENT_TIME_SEC	curr_time_sec()
#define	get_seconds()	time_second()

static __inline struct timespec curr_time_sec()
{
	struct timespec __ts = { time_second(), 0 };
	return __ts;
}

/*
 * Change timeval to jiffies, trying to avoid the
 * most obvious overflows..
 *
 * And some not so obvious.
 *
 * Note that we don't want to return MAX_LONG, because
 * for various timeout reasons we often end up having
 * to wait "jiffies+1" in order to guarantee that we wait
 * at _least_ "jiffies" - so "jiffies+1" had better still
 * be positive.
 */
#define MAX_JIFFY_OFFSET ((~0UL >> 1)-1)

#endif
