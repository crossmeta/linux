#ifndef _LINUX_BLOCKGROUP_LOCK_H
#define _LINUX_BLOCKGROUP_LOCK_H
/*
 * Per-blockgroup locking for ext2 and ext3.
 *
 * Simple hashed spinlocking.
 */

#include <linux/kernel.h>

#define NR_BG_LOCKS	16

struct bgl_lock {
	spinlock_t lock;
} ____cacheline_aligned_in_smp;

struct blockgroup_lock {
	struct bgl_lock locks[NR_BG_LOCKS];
};

static inline void bgl_lock_init(struct blockgroup_lock *bgl)
{
	int i;

	for (i = 0; i < NR_BG_LOCKS; i++)
		spin_lock_init(&bgl->locks[i].lock);
}

/*
 * The accessor is a macro so we can embed a blockgroup_lock into different
 * superblock types
 */
static inline spinlock_t *
bgl_lock_ptr(struct blockgroup_lock *bgl, unsigned int block_group)
{
	return &bgl->locks[(block_group) & (NR_BG_LOCKS-1)].lock;
}

#endif
