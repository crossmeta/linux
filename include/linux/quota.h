
#ifndef  _LINUX_QUOTA_
#define _LINUX_QUOTA_

#include <sys/quota.h>

typedef __kernel_uid32_t qid_t; /* Type in which we store ids in memory */
typedef __u64 qsize_t;		/* Type in which we store size limitations */


/* This is in-memory copy of quota block. See meaning of entries above */
struct mem_dqblk {
	unsigned int dqb_ihardlimit;
	unsigned int dqb_isoftlimit;
	unsigned int dqb_curinodes;
	unsigned int dqb_bhardlimit;
	unsigned int dqb_bsoftlimit;
	qsize_t dqb_curspace;
	__kernel_time_t dqb_btime;
	__kernel_time_t dqb_itime;
};

/* Inmemory copy of version specific information */
struct mem_dqinfo {
	unsigned int dqi_bgrace;
	unsigned int dqi_igrace;
	unsigned int dqi_flags;
	unsigned int dqi_blocks;
	unsigned int dqi_free_blk;
	unsigned int dqi_free_entry;
};

#endif /* _LINUX_QUOTA_ */

