/*
 * Definitions for diskquota-operations. When diskquota is configured these
 * macros expand to the right source-code.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 * Version: $Id: quotaops.h,v 1.2 1998/01/15 16:22:26 ecd Exp $
 *
 */
#ifndef _LINUX_QUOTAOPS_
#define _LINUX_QUOTAOPS_

#if defined(CONFIG_QUOTA)

/*
 * declaration of quota_function calls in kernel.
 */
extern void dquot_initialize(struct inode *inode, short type);
extern void dquot_drop(struct inode *inode);
extern void invalidate_dquots(kdev_t dev, short type);
extern int  quota_off(struct super_block *sb, short type);
extern int  sync_dquots(kdev_t dev, short type);

extern int  dquot_alloc_space(struct inode *inode, qsize_t number, char prealloc);
extern int  dquot_alloc_inode(const struct inode *inode, unsigned long number);

extern void dquot_free_space(struct inode *inode, qsize_t number);
extern void dquot_free_inode(const struct inode *inode, unsigned long number);

extern int  dquot_transfer(struct inode *inode, struct iattr *iattr);

/*
 * Operations supported for diskquotas.
 */
#define sb_any_quota_enabled(sb) ((sb)->s_dquot.flags & (DQUOT_USR_ENABLED | DQUOT_GRP_ENABLED))

extern __inline__ void DQUOT_INIT(struct inode *inode)
{
	if (!inode->i_sb)
		BUG();
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb))
		inode->i_sb->dq_op->initialize(inode, -1);
	unlock_kernel();
}

extern __inline__ void DQUOT_DROP(struct inode *inode)
{
	if (!inode->i_sb)
		BUG();
	lock_kernel();
	if (IS_QUOTAINIT(inode))
		inode->i_sb->dq_op->drop(inode);	/* Ops must be set when there's any quota... */
	unlock_kernel();
}

extern __inline__ int DQUOT_PREALLOC_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb)) {
		/* Number of used blocks is updated in alloc_block() */
		if (inode->i_sb->dq_op->alloc_space(inode, nr, 1) == NO_QUOTA) {
			unlock_kernel();
			return 1;
		}
	}
	else {
		inode_add_bytes(inode, nr);
		mark_inode_dirty(inode);
	}
	unlock_kernel();
	return 0;
}

extern __inline__ int DQUOT_ALLOC_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb)) {
		/* Number of used blocks is updated in alloc_block() */
		if (inode->i_sb->dq_op->alloc_space(inode, nr, 0) == NO_QUOTA) {
			unlock_kernel();
			return 1;
		}
	}
	else {
		inode_add_bytes(inode, nr);
		mark_inode_dirty(inode);
	}
	unlock_kernel();
	return 0;
}

extern __inline__ int DQUOT_ALLOC_INODE(struct inode *inode)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb)) {
		inode->i_sb->dq_op->initialize(inode, -1);
		if (inode->i_sb->dq_op->alloc_inode(inode, 1) == NO_QUOTA) {
			unlock_kernel();
			return 1;
		}
	}
	unlock_kernel();
	return 0;
}

extern __inline__ void DQUOT_FREE_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb))
		inode->i_sb->dq_op->free_space(inode, nr);
	else {
		inode_sub_bytes(inode, nr);
		mark_inode_dirty(inode);
	}
	unlock_kernel();
}

extern __inline__ void DQUOT_FREE_INODE(struct inode *inode)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb))
		inode->i_sb->dq_op->free_inode(inode, 1);
	unlock_kernel();
}

extern __inline__ int DQUOT_TRANSFER(struct inode *inode, struct iattr *iattr)
{
	lock_kernel();
	if (sb_any_quota_enabled(inode->i_sb)) {
		inode->i_sb->dq_op->initialize(inode, -1);
		if (inode->i_sb->dq_op->transfer(inode, iattr) == NO_QUOTA) {
			unlock_kernel();
			return 1;
		}
	}
	unlock_kernel();
	return 0;
}

#define DQUOT_SYNC(dev)	sync_dquots(dev, -1)
#define DQUOT_OFF(sb)	quota_off(sb, -1)

#else

/*
 * NO-OP when quota not configured.
 */
#define DQUOT_INIT(inode)			do { } while(0)
#define DQUOT_DROP(inode)			do { } while(0)
#define DQUOT_ALLOC_INODE(inode)		(0)
#define DQUOT_FREE_INODE(inode)			do { } while(0)
#define DQUOT_SYNC(dev)				do { } while(0)
#define DQUOT_OFF(sb)				do { } while(0)
#define DQUOT_TRANSFER(inode, iattr)		(0)
extern __inline__ int DQUOT_PREALLOC_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	inode_add_bytes(inode, nr);
	unlock_kernel();
	mark_inode_dirty(inode);
	return 0;
}

extern __inline__ int DQUOT_ALLOC_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	inode_add_bytes(inode, nr);
	unlock_kernel();
	mark_inode_dirty(inode);
	return 0;
}

extern __inline__ void DQUOT_FREE_SPACE(struct inode *inode, qsize_t nr)
{
	lock_kernel();
	inode_sub_bytes(inode, nr);
	unlock_kernel();
	mark_inode_dirty(inode);
}

#endif /* CONFIG_QUOTA */

#define DQUOT_ALLOC_BLOCK(inode, nr) DQUOT_ALLOC_SPACE((inode), ((qsize_t)(nr)) << (inode)->i_sb->s_blocksize_bits)
#define DQUOT_PREALLOC_BLOCK(inode, nr) DQUOT_ALLOC_SPACE((inode), ((qsize_t)(nr)) << (inode)->i_sb->s_blocksize_bits)
#define DQUOT_FREE_BLOCK(inode, nr) DQUOT_FREE_SPACE((inode), ((qsize_t)(nr)) << (inode)->i_sb->s_blocksize_bits)

static inline void vfs_dq_init(struct inode *inode)
{
}

static inline void vfs_dq_drop(struct inode *inode)
{
}

static inline int vfs_dq_alloc_space_nodirty(struct inode *inode, qsize_t nr)
{
	inode_add_bytes(inode, nr);
	return 0;
}

static inline void vfs_dq_alloc_space_nofail(struct inode *inode, qsize_t nr)
{
	inode_add_bytes(inode, nr);
	mark_inode_dirty(inode);
}

static inline int vfs_dq_alloc_space(struct inode *inode, qsize_t nr)
{
	vfs_dq_alloc_space_nodirty(inode, nr);
	mark_inode_dirty(inode);
	return 0;
}

static inline int vfs_dq_reserve_space(struct inode *inode, qsize_t nr)
{
	return 0;
}

static inline int vfs_dq_claim_space(struct inode *inode, qsize_t nr)
{
	return vfs_dq_alloc_space(inode, nr);
}

static inline
int vfs_dq_release_reservation_space(struct inode *inode, qsize_t nr)
{
	return 0;
}

static inline void vfs_dq_free_space_nodirty(struct inode *inode, qsize_t nr)
{
	inode_sub_bytes(inode, nr);
}

static inline void vfs_dq_free_space(struct inode *inode, qsize_t nr)
{
	vfs_dq_free_space_nodirty(inode, nr);
	mark_inode_dirty(inode);
}


static inline int vfs_dq_alloc_inode(struct inode *inode)
{
	return 0;
}

static inline void vfs_dq_free_inode(struct inode *inode)
{
}


static inline int vfs_dq_alloc_block(struct inode *inode, qsize_t nr)
{
	return vfs_dq_alloc_space(inode, nr << inode->i_blkbits);
}

static inline int vfs_dq_reserve_block(struct inode *inode, qsize_t nr)
{
	return vfs_dq_reserve_space(inode, nr << inode->i_blkbits);
}

static inline void vfs_dq_alloc_block_nofail(struct inode *inode, qsize_t nr)
{
	vfs_dq_alloc_space_nofail(inode, nr << inode->i_blkbits);
}

static inline int vfs_dq_claim_block(struct inode *inode, qsize_t nr)
{
	return vfs_dq_claim_space(inode, nr << inode->i_blkbits);
}

static inline
void vfs_dq_release_reservation_block(struct inode *inode, qsize_t nr)
{
	vfs_dq_release_reservation_space(inode, nr << inode->i_blkbits);
}

static inline void vfs_dq_free_block(struct inode *inode, qsize_t nr)
{
	vfs_dq_free_space(inode, nr << inode->i_blkbits);
}

static inline int vfs_dq_transfer(struct inode *inode, struct iattr *iattr)
{
	return 0;
}


#endif /* _LINUX_QUOTAOPS_ */
