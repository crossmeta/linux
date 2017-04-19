/*
 * CROSSMETA Windows porting changes.
 * Copyright (c) 2001-2011 Supramani Sammandam.  suprasam _at_ crossmeta.org
 */
#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/kdev_t.h>
#include <sys/vfs.h>
#include <sys/vnode.h>
#include <sys/buf.h>
#include <sys/namei.h>
#include <linux/capability.h>
#include <linux/dcache.h>
#include <sys/stat.h>
#include <linux/reiserfs_fs_i.h>

#define BLOCK_SIZE_BITS 10
#define BLOCK_SIZE (1<<BLOCK_SIZE_BITS)

#define FMODE_READ	1
#define	FMODE_WRITE	2

/* Flags for ll_rw_block */
#define READ	UIO_READ
#define	WRITE	UIO_WRITE
#define READA	2
#define READ_META	READ

#ifdef __KERNEL__
#define S_IRWXUGO       (S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO       (S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO         (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO         (S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO         (S_IXUSR|S_IXGRP|S_IXOTH)
#endif

/*
 * These are the fs-independent mount-flags: up to 32 flags are supported
 */
#define MS_RDONLY	MNT_RDONLY	/* Mount read-only */
#define MS_NOSUID	MNT_NOSUID	/* Ignore suid and sgid bits */
#define MS_SYNCHRONOUS	MNT_SYNCHRONOUS
#define MS_MANDLOCK	0		/* XXX */
#define	MS_POSIXACL	0		/* XXX */
#define	MS_DIRSYNC	0		/* XXX */

/* Inode flags - they have nothing to superblock flags now */

#define S_SYNC		1	/* Writes are synced at once */
#define S_NOATIME	2	/* Do not update access times */
#define S_APPEND	4	/* Append-only file */
#define S_IMMUTABLE	8	/* Immutable file */
#define S_DEAD		16	/* removed, but still open directory */
#define	S_NOQUOTA	32	/* Inode is not counted to quota */
#define	S_DIRSYNC	64	/* Directory modifications are synchronous */
#define	S_SWAPFILE	256	/* Do not truncate: swapon got its bmaps */

/*
 * Note that nosuid etc flags are inode-specific: setting some file-system
 * flags just means all the inodes inherit those flags by default. It might be
 * possible to override it selectively if you really wanted to with some
 * ioctl() that is not currently implemented.
 *
 * Exception: MS_RDONLY is always applied to the entire file system.
 *
 * Unfortunately, it is possible to change a filesystems flags with it mounted
 * with files in use.  This means that all of the inodes will not have their
 * i_flags updated.  Hence, i_flags no longer inherit the superblock mount
 * flags, so these have to be checked separately. -- rmk@arm.uk.linux.org
 */
#define __IS_FLG(inode,flg) ((inode)->i_sb->s_flags & (flg))

#define IS_RDONLY(inode) ((inode)->i_sb->s_flags & MS_RDONLY)
#define IS_NOSUID(inode)	__IS_FLG(inode, MS_NOSUID)
#define IS_NODEV(inode)		__IS_FLG(inode, MS_NODEV)
#define IS_NOEXEC(inode)	__IS_FLG(inode, MS_NOEXEC)
#define IS_SYNC(inode)		(__IS_FLG(inode, MS_SYNCHRONOUS) || ((inode)->i_flags & S_SYNC))
#define	IS_DIRSYNC(inode)	(__IS_FLG(inode, MS_SYNCHRONOUS|MS_DIRSYNC) || \
					((inode)->i_flags & (S_SYNC|S_DIRSYNC)))
#define IS_MANDLOCK(inode)	__IS_FLG(inode, MS_MANDLOCK)

#define IS_QUOTAINIT(inode)	((inode)->i_flags & S_QUOTA)
#define IS_APPEND(inode)	((inode)->i_flags & S_APPEND)
#define IS_IMMUTABLE(inode)	((inode)->i_flags & S_IMMUTABLE)
#define IS_NOATIME(inode)	(__IS_FLG(inode, MS_NOATIME) || ((inode)->i_flags & S_NOATIME))
#define IS_NODIRATIME(inode)	__IS_FLG(inode, MS_NODIRATIME)

#define IS_DEADDIR(inode)	((inode)->i_flags & S_DEAD)
#define	IS_SWAPFILE(inode)	((inode)->i_flags & S_SWAPFILE)


#ifdef __KERNEL__

#define UPDATE_ATIME(inode)

/* bh state bits */
#define BH_Uptodate	0	/* 1 if the buffer contains valid data */
#define BH_Dirty	1	/* 1 if the buffer is dirty */
#define BH_Lock		2	/* 1 if the buffer is locked */
#define BH_Req		3	/* 0 if the buffer has been invalidated */
#define BH_Mapped	4	/* 1 if the buffer has a disk mapping */
#define BH_New		5	/* 1 if the buffer is new and not yet written out */
#define BH_Protected	6	/* 1 if the buffer is protected */
#define	BH_Boundary	9	/* Block is followed by a discontiguity */
#define	BH_Delay	10	/* Buffer is not yet allocated on disk */
#define	BH_Unwritten	11	/* Buffer is allocated on disk but not written*/
#define BH_DirtyQueue	12	/* 1 if the buffer i_dirty_buffers queue */
#define BH_PrivateStart	14	/* not a state bit, but the first bit available
				 * for private allocation by other entities
				 */


#define buffer_head	buf
#define	b_blocknr	b_lblkno
#define b_rsector	b_blkno
#define b_size		b_bcount
#define b_state		b_fspriv3
#define b_usecount	b_fspriv2
#define b_dirtyvp	b_fspriv1
#define b_page		b_data
#define b_count		b_usecount
#define	b_end_io	b_iodone
#define	b_private	b_priv1

extern struct buffer_head *linux_bread(kdev_t, daddr_t, int);
extern void linux_brelse(struct buffer_head *);
extern struct buffer_head *linux_getblk(kdev_t, daddr_t, int);
#if BUFTRACE
__inline struct buf *
trace_linux_getblk(kdev_t dev, daddr_t block, int size, char *f, int num)
{
	struct buffer_head *bh;
	bh = linux_getblk(dev,block,size);
	bp_log(bh, "getblk", num, f);
	return (bh);
}

__inline struct buf *
trace_linux_bread(kdev_t dev, daddr_t block, int size, char *f, int num)
{
	struct buffer_head *bh;
	bh = linux_bread(dev,block,size);
	bp_log(bh, "bread", num, f);
	return (bh);
}

__inline void
trace_linux_brelse(struct buffer_head *bh, char *f, int num)
{
	if (!bh)
		return;
	bp_log(bh, "brelse", num, f);
	linux_brelse(bh);
}
#define getblk(d,b,s)		trace_linux_getblk(d,b,s,__FILE__,__LINE__)
#define brelse(bp)		trace_linux_brelse(bp,__FILE__,__LINE__)
#define bread(d,b,s)		trace_linux_bread(d,b,s,__FILE__,__LINE__)
#else

#define getblk		linux_getblk
#define brelse		linux_brelse
#define bread		linux_bread
#define	__bread		linux_bread
#endif


extern struct buffer_head *linux_bread(kdev_t, daddr_t, int);
extern void linux_brelse(struct buffer_head *);
extern struct buffer_head *linux_getblk(kdev_t, daddr_t, int);

static inline void get_bh(struct buffer_head *bh)
{
	atomic_inc((atomic_t *)&bh->b_usecount);
}

static inline void put_bh(struct buffer_head *bh)
{
	if (atomic_read((atomic_t *)&bh->b_usecount) == 1)
		linux_brelse(bh);
	else
		atomic_dec((atomic_t *)&bh->b_usecount);
}

static inline void lock_buffer(struct buffer_head *bh)
{
	if (test_and_set_bit(BH_Lock, &bh->b_state)) {
		printf("lock_buffer: bh %p already locked\n");
		ASSERT(FALSE);
	}
	bp_lock(bh);
}

static inline void unlock_buffer(struct buffer_head *bh)
{
	if (!test_and_clear_bit(BH_Lock, &bh->b_state)) {
		printf("lock_buffer: bh %p not locked\n");
		ASSERT(FALSE);
	}
	bp_unlock(bh);
}

#define __buffer_state(bh, state)	((((long)(bh)->b_state) & (1UL << BH_##state)) != 0)

#define buffer_dirty(bh)	__buffer_state(bh,Dirty)
#define buffer_locked(bh)	__buffer_state(bh,Lock)
#define buffer_req(bh)		__buffer_state(bh,Req)
#define buffer_mapped(bh)	__buffer_state(bh,Mapped)
#define buffer_new(bh)		__buffer_state(bh,New)
#define buffer_protected(bh)	__buffer_state(bh,Protected)

#define bh_offset(bh)		((unsigned long)(bh)->b_data & ~PAGE_MASK)

#include <linux/ext2_fs_i.h>

struct buffer_head;
//typedef int (get_block_t)(struct inode *inode, sector_t iblock,
//			struct buffer_head *bh_result, int create);
typedef void (dio_iodone_t)(struct kiocb *iocb, loff_t offset,
			ssize_t bytes, void *private, int ret,
			bool_t is_async);
/*
 * Attribute flags.  These should be or-ed together to figure out what
 * has been changed!
 */
#define ATTR_MODE	1
#define ATTR_UID	2
#define ATTR_GID	4
#define ATTR_SIZE	8
#define ATTR_ATIME	16
#define ATTR_MTIME	32
#define ATTR_CTIME	64
#define ATTR_ATIME_SET	128
#define ATTR_MTIME_SET	256
#define ATTR_FORCE	512	/* Not a change, but a change it */
#define ATTR_ATTR_FLAG	1024

/*
 * This is the Inode Attributes structure, used for notify_change().  It
 * uses the above definitions as flags, to know which values have changed.
 * Also, in this manner, a Filesystem can look at only the values it cares
 * about.  Basically, these are the attributes that the VFS layer can
 * request to change from the FS layer.
 *
 * Derek Atkins <warlord@MIT.EDU> 94-10-20
 */
struct iattr {
	unsigned int	ia_valid;
	umode_t		ia_mode;
	uid_t		ia_uid;
	gid_t		ia_gid;
	loff_t		ia_size;
	timespec_t	ia_atime;
	timespec_t	ia_mtime;
	timespec_t	ia_ctime;
	unsigned int	ia_attr_flags;
};

/*
 * This is the inode attributes flag definitions
 */
#define ATTR_FLAG_SYNCRONOUS	1 	/* Syncronous write */
#define ATTR_FLAG_NOATIME	2 	/* Don't update atime */
#define ATTR_FLAG_APPEND	4 	/* Append-only file */
#define ATTR_FLAG_IMMUTABLE	8 	/* Immutable file */
#define ATTR_FLAG_NODIRATIME	16 	/* Don't update atime for directory */

/*
 * Request nofications on a directory.
 * See below for events that may be notified.
 */
#define F_NOTIFY	(F_LINUX_SPECIFIC_BASE+2)

/*
 * Types of directory notifications that may be requested.
 */
#define DN_ACCESS	0x00000001	/* File accessed */
#define DN_MODIFY	0x00000002	/* File modified */
#define DN_CREATE	0x00000004	/* File created */
#define DN_DELETE	0x00000008	/* File removed */
#define DN_RENAME	0x00000010	/* File renamed */
#define DN_ATTRIB	0x00000020	/* File changed attibutes */
#define DN_MULTISHOT	0x80000000	/* Don't remove notifier */


/*
 * Includes for diskquotas and mount structures.
 */
#include <linux/quota.h>

/*
 * oh the beauties of C type declarations.
 */
struct page;
struct address_space;

struct address_space_operations {
	int (*writepage)(struct page *);
	int (*readpage)(struct file *, struct page *);
	int (*sync_page)(struct page *);
	int (*prepare_write)(struct file *, struct page *, unsigned, unsigned);
	int (*commit_write)(struct file *, struct page *, loff_t, unsigned);
	void (*truncatepage)(struct page *); /* called from truncate_complete_page */
	/* Unfortunately this kludge is needed for FIBMAP. Don't use it */
	int (*bmap)(struct address_space *, long);
	int (*odirect_IO)(int, struct inode *, struct kiobuf *, unsigned long, int);
	int (*direct_IO)(int, struct inode *, struct kiocb *, struct iovec *,
			loff_t , unsigned long);
};

struct address_space {
	struct address_space_operations *a_ops;	/* methods */
	struct inode		*host;		/* owner: inode, block_device */
	spinlock_t		list_lock;
	struct list_head 	page_list;
	
};

#define	PG_uptodate	3

typedef struct page {
	struct address_space *mapping;	/* The inode (or ...) we belong to. */
	unsigned long index;		/* Our offset within mapping. */
	atomic_t count;			/* Usage count, ... */
	unsigned long flags;		/* atomic flags, some possibly
					   updated asynchronously */

	struct list_head list;
	struct buffer_head *buffers;
	void *virtual;
	int needfree;
} mem_map_t;

#define	INIT_PAGE_LIST(map)	do { 		\
	INIT_LIST_HEAD(&(map)->page_list); 	\
	spin_lock_init(&(map)->list_lock);	\
} while (0)
#define	page_has_buffers(pg)	((pg)->buffers != NULL)
#define	page_buffers(pg)	((pg)->buffers)
#define page_address(pg)	((pg)->buffers? (pg)->buffers->b_data : (caddr_t)(pg)->virtual)
#define	page_symlink(inode,name,len)	block_symlink(inode,name,len)
#define	__page_symlink(inode,name,len,fl)	block_symlink(inode,name,len)
#define	mapping_tagged(mapping,tag)	(0)
#define	mark_page_accessed(pg)
#define	PageUptodate(pg)	(page_buffers(pg)? \
	buffer_uptodate((pg)->buffers) : test_bit(PG_uptodate, &(pg)->flags))
#define	PageLocked(pg)	(page_buffers(pg)? \
					buffer_locked((pg)->buffers) : 0)
#define	find_get_page(mapp,off)		(NULL)

static inline struct page *
find_page_list(struct address_space *mapping, pgoff_t index)
{
	struct page *page;
	struct list_head *lp;

	list_for_each(lp, &mapping->page_list) {
		struct page *page = list_entry(lp, struct page, list);
		if (page->index == index) {
			return (page);
		}
	}
	return (NULL);
}

static inline void page_cache_release(struct page *pg)
{
	struct buffer_head *bh = page_buffers(pg);
	if (bh && bh->b_irp)	/* be careful about dummy bh */
		linux_brelse(bh);
	if (pg->needfree && atomic_dec_and_test(&pg->count)) {
		list_del(&pg->list);
		if (!bh)
			kfree(pg->virtual);
#if DBG
		printf("page_cache_rel: free page %p index %d\n", pg, pg->index);
#endif
		kfree(pg);
	}
}

/*
 * XXX: Only to be used by ext4 buddy inode
 */
static inline struct page *
find_or_create_page(struct address_space *mapping, pgoff_t index,
				gfp_t gfp_mask)
{
	struct page *page;
	struct page *old;
	
	spin_lock(&mapping->list_lock);
	page = find_page_list(mapping, index);
	spin_unlock(&mapping->list_lock);
	if (page)
		return (page);

	page = kzalloc(sizeof (*page),gfp_mask);
	page->mapping = mapping;
	page->index = index;
	page->needfree = 0;	/* don't free now on release */
	page->virtual = kmalloc(PAGE_SIZE, GFP_KERNEL);
	atomic_set(&page->count, 1);

	spin_lock(&mapping->list_lock);
	old = find_page_list(mapping, index);
	if (!old)
		list_add_tail(&page->list, &mapping->page_list);
	spin_unlock(&mapping->list_lock);
	if (old) {
		page->needfree = 1;
		INIT_LIST_HEAD(&page->list);
		page_cache_release(page);
		return (old);
	}
	return (page);
}

static inline struct page *
destroy_page_list(struct address_space *mapping)
{
	struct page *page;

	while (!list_empty(&mapping->page_list)) {
		struct page *page = list_entry(mapping->page_list.next,
						struct page, list);
//		ASSERT(atomic_read(&page->count) == 0);
		ASSERT(page->needfree == 0);
		page->needfree = 1;
		atomic_set(&page->count, 1);
		page_cache_release(page);
	}
	return (NULL);
}

#define	get_page(pg)		atomic_inc(&(pg)->count)
#define	page_cache_get(pg)	get_page(pg)

static inline void lock_page(struct page *page)
{
	struct buffer_head *bh = page_buffers(page);
	if (bh && bh->b_irp)
		lock_buffer(bh);
}

static inline void unlock_page(struct page *page)
{
	struct buffer_head *bh = page_buffers(page);
	if (bh && bh->b_irp)
		unlock_buffer(page_buffers(page));
}


/* Convert between inode pointers and vnode pointers. */
#define VTOI(vp)		((struct inode *)(vp)->v_data)
#define ITOV(ip)		((struct vnode *)(ip)->i_fspriv)

struct inode {
	LIST_ENTRY(inode)	i_hash;
	LIST_ENTRY(inode)	i_list;
	struct buflists		i_dirty_buffers;
	unsigned long		i_ino;
	atomic_t		i_count;
	kdev_t			i_dev;
	umode_t			i_mode;
	nlink_t			i_nlink;
	uid_t			i_uid;
	gid_t			i_gid;
	udev_t			i_rdev;
	loff_t			i_size;
	timespec_t		i_atime;
	timespec_t		i_mtime;
	timespec_t		i_ctime;
	unsigned int		i_blkbits;
	unsigned long		i_blksize;
	blkcnt_t 		i_blocks;
	unsigned long		i_version;
	unsigned short		i_bytes;
	spinlock_t		i_lock;	/* i_blocks, i_bytes, maybe i_size */
	kmutex_t		i_mutex;
	struct inode_operations	*i_op;
	struct file_operations	*i_fop;	/* former ->i_op->default_file_ops */
	struct super_block	*i_sb;
	wait_queue_head_t	i_wait;
	struct file_lock	*i_flock;
	struct address_space	*i_mapping;
	struct dquot		*i_dquot[MAXQUOTAS];

	unsigned long		i_dnotify_mask; /* Directory notify events */
	struct dnotify_struct	*i_dnotify; /* for directory notifications */

	unsigned long		i_state;

	unsigned int		i_flags;
	unsigned char		i_sock;

	atomic_t		i_writecount;
	unsigned int		i_attr_flags;
	__u32			i_generation;
	rwsleep_t		i_rwlock;	/* I/O lock */
	void 			*i_fspriv;
	union {
		struct ext2_inode_info		ext2_i;
		struct reiserfs_inode_info	reiserfs_i;
		void				*generic_ip;
	} u;
	struct address_space	i_mapdat;
};

static __inline loff_t i_size_read(const struct inode *inode)
{
	return inode->i_size;
}

static __inline int inode_newsize_ok(const struct inode *inode, loff_t offset)
{
	return (0);
}

static inline i_size_write(struct inode *inode, loff_t i_size)
{
	inode->i_size = i_size;
}

static inline void truncate_inode_pages(struct inode *inode, loff_t from)
{
	vnode_pager_zero_eof(ITOV(inode), from);
}

struct file {
	struct dentry		*f_dentry;
	struct vfsmount         *f_vfsmnt;
	struct file_operations	*f_op;
	atomic_t		f_count;
	unsigned int 		f_flags;
	mode_t			f_mode;
	loff_t			f_pos;
	int			f_error;
	unsigned long		f_version;
	void			*private_data;
};

#define	MAX_LFS_FILESIZE	(__uint64_t)0x7fffffffffffffff

#define DQUOT_USR_ENABLED       0x01            /* User diskquotas enabled */
#define DQUOT_GRP_ENABLED       0x02            /* Group diskquotas enabled */

struct quota_info
{
	unsigned int flags;	/* Flags for diskquotas on this device */
        rwsleep_t dqio_sem;	/* lock device while I/O in progress */
        rwsleep_t dqoff_sem;	/* serialize quota_off() and quota_on() on device */
        struct file *files[MAXQUOTAS];          /* fp's to quotafiles */
        struct mem_dqinfo info[MAXQUOTAS];      /* Information for each quota type */
};

#define VFSTOSB(vfsp)	((struct super_block *)(vfsp)->vfs_data)

struct ext2fid {
	uint16_t	e2fid_len;	/* length of remaining bytes */
	uint16_t	e2fid_pad;	/* for 32bit alignment */
	int32_t		e2fid_gen;	/* generation number */
	ino_t		e2fid_ino;	/* inode number */
};


#include <linux/ext2_fs_sb.h>
#include <linux/reiserfs_fs_sb.h>

struct super_block {
	LIST_ENTRY(super_block) s_list;
	kdev_t 			s_dev;
	unsigned long		s_blocksize;
	unsigned char		s_blocksize_bits;
	unsigned char 		s_lock;
	unsigned char		s_dirt;
	fsleep_t		s_sblock;
	unsigned __int64	s_maxbytes;	/* Max file size */
	struct file_system_type	*s_type;
	struct super_operations	*s_op;
	struct dquot_operations	*dq_op;
	struct export_operations *s_export_op;
	unsigned long		s_flags;
	unsigned long		s_magic;
	void			*s_root;	/* root vnode ptr */
	LIST_HEAD(inodelists, inode) s_dirty;
	struct vfs		*s_vfsp;
	struct quota_info	s_dquot;	/* Diskquota specific options */
	char 			s_id[32];	/* Informational name */

	/* Granularity of c/m/atime in ns.
	   Cannot be worse than a second */
	u32			s_time_gran;
	union {
		struct ext2_sb_info	ext2_sb;
		struct reiserfs_sb_info reiserfs_sb;
		void			*generic_sbp;
	} u;
};
#define	s_fs_info	u.generic_sbp

/**
 * current_fs_time - Return FS time
 * @sb: Superblock.
 *
 * Return the current time truncated to the time granularity supported by
 * the fs.
 */
static __inline struct timespec current_fs_time(struct super_block *sb)
{
        struct timespec now;
	u32 gran = sb->s_time_gran;

	systime(&now);
	if (!gran) {
		/* nothing */
	} else if (gran == 1000000000) {
		now.tv_nsec = 0;
	} else {
		now.tv_nsec -= now.tv_nsec % gran;
	}
        return now;
}

/*
 * VFS helper functions..
 */
extern int vfs_create(struct inode *, struct dentry *, int);
extern int vfs_mkdir(struct inode *, struct dentry *, int);
extern int vfs_mknod(struct inode *, struct dentry *, int, dev_t);
extern int vfs_symlink(struct inode *, struct dentry *, const char *);
extern int vfs_link(struct dentry *, struct inode *, struct dentry *);
extern int vfs_rmdir(struct inode *, struct dentry *);
extern int vfs_unlink(struct inode *, struct dentry *);
extern int vfs_rename(struct inode *, struct dentry *, struct inode *, struct dentry *);

/*
 * File types
 */
#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14

/*
 * This is the "filldir" function type, used by readdir() to let
 * the kernel specify what kind of dirent layout it wants to have.
 * This allows the kernel to read directories into kernel space or
 * to have different dirent layouts depending on the binary type.
 */
typedef int (*filldir_t)(void *, const char *, int, off_t, ino_t, unsigned);

struct block_device_operations {
	int (*open) (struct inode *, struct file *);
	int (*release) (struct inode *, struct file *);
	int (*ioctl) (struct inode *, struct file *, unsigned, unsigned long);
	int (*check_media_change) (kdev_t);
	int (*revalidate) (kdev_t);
};

/*
 * NOTE:
 * read, write, poll, fsync, readv, writev can be called
 *   without the big kernel lock held in all filesystems.
 */
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, struct dentry *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*readv) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*writepage) (struct file *, struct page *, int, size_t, loff_t *, int);
};

struct inode_operations {
	int (*create) (struct inode *,struct dentry *,int);
	struct dentry * (*lookup) (struct inode *,struct dentry *);
	int (*link) (struct dentry *,struct inode *,struct dentry *);
	int (*unlink) (struct inode *,struct dentry *);
	int (*symlink) (struct inode *,struct dentry *,const char *);
	int (*mkdir) (struct inode *,struct dentry *,int);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct inode *,struct dentry *,int,int);
	int (*rename) (struct inode *, struct dentry *,
			struct inode *, struct dentry *);
	int (*readlink) (struct dentry *, char *,int);
	int (*follow_link) (struct dentry *, struct nameidata *);
	void (*put_link) (struct dentry *, struct nameidata *, void *);
	void (*truncate) (struct inode *);
	int (*permission) (struct inode *, int);
	int (*revalidate) (struct dentry *);
	int (*setattr) (struct dentry *, struct iattr *);
	int (*getattr) (struct dentry *, struct iattr *);
};

/*
 * NOTE: write_inode, delete_inode, clear_inode, put_inode can be called
 * without the big kernel lock held in all filesystems.
 */
struct super_operations {
	struct inode *(*alloc_inode)(struct super_block *sb);
	void (*destroy_inode)(struct inode *);

	void (*read_inode) (struct inode *);
  
  	/* reiserfs kludge.  reiserfs needs 64 bits of information to
    	** find an inode.  We are using the read_inode2 call to get
   	** that information.  We don't like this, and are waiting on some
   	** VFS changes for the real solution.
   	** iget4 calls read_inode2, iff it is defined
   	*/
    	void (*read_inode2) (struct inode *, void *) ;
   	void (*dirty_inode) (struct inode *);
	void (*write_inode) (struct inode *, int);
	void (*put_inode) (struct inode *);
	void (*delete_inode) (struct inode *);
	void (*put_super) (struct super_block *);
	void (*write_super) (struct super_block *);
	void (*write_super_lockfs) (struct super_block *);
	void (*unlockfs) (struct super_block *);
	int (*statfs) (struct super_block *, struct statfs *);
	int (*remount_fs) (struct super_block *, int *, char *);
	void (*clear_inode) (struct inode *);
	void (*umount_begin) (struct super_block *);
};

/* Inode state bits.. */
#define I_DIRTY_SYNC		1 /* Not dirty enough for O_DATASYNC */
#define I_DIRTY_DATASYNC	2 /* Data-related inode changes pending */
#define I_DIRTY_PAGES		4 /* Data-related inode changes pending */
#define I_LOCK			8
#define I_FREEING		16
#define I_CLEAR			32
#define	I_NEW			64

#define I_DIRTY (I_DIRTY_SYNC | I_DIRTY_DATASYNC | I_DIRTY_PAGES)

extern void __mark_inode_dirty(struct inode *, int);
static inline void mark_inode_dirty(struct inode *inode)
{
	__mark_inode_dirty(inode, I_DIRTY);
}

static inline void mark_inode_dirty_sync(struct inode *inode)
{
	__mark_inode_dirty(inode, I_DIRTY_SYNC);
}

static inline void inode_add_bytes(struct inode *inode, loff_t bytes)
{
	inode->i_blocks += bytes >> 9;
	bytes &= 511;
	inode->i_bytes += bytes;
	if (inode->i_bytes >= 512) {
		inode->i_blocks++;
		inode->i_bytes -= 512;
	}
}

static inline void inode_sub_bytes(struct inode *inode, loff_t bytes)
{
	inode->i_blocks -= bytes >> 9;
	bytes &= 511;
	if (inode->i_bytes < bytes) {
		inode->i_blocks--;
		inode->i_bytes += 512;
	}
	inode->i_bytes -= bytes;
}

static inline loff_t inode_get_bytes(struct inode *inode)
{
	return (((loff_t)inode->i_blocks) << 9) + inode->i_bytes;
}

static inline void inode_set_bytes(struct inode *inode, loff_t bytes)
{
	inode->i_blocks = bytes >> 9;
	inode->i_bytes = bytes & 511;
}

static inline void mark_inode_dirty_pages(struct inode *inode)
{
	__mark_inode_dirty(inode, I_DIRTY_PAGES);
}

static inline void inc_nlink(struct inode *inode)
{
	inode->i_nlink++;
}

static inline void drop_nlink(struct inode *inode)
{
	inode->i_nlink--;
}

static inline void clear_nlink(struct inode *inode)
{
	inode->i_nlink = 0;
}

static inline void inode_inc_iversion(struct inode *inode)
{
	spin_lock(&inode->i_lock);
	inode->i_version++;
	spin_unlock(&inode->i_lock);
}


struct dquot_operations {
	void (*initialize) (struct inode *, short);
	void (*drop) (struct inode *);
	int (*alloc_space) (struct inode *, qsize_t, char);
	int (*alloc_inode) (const struct inode *, unsigned long);
	void (*free_space) (struct inode *, qsize_t);
	void (*free_inode) (const struct inode *, unsigned long);
	int (*transfer) (struct inode *, struct iattr *);
};

struct file_system_type {
	const char *name;
	struct super_block *(*read_super) (struct super_block *, void *, int);
	void (*unload)(void *);
	int vtype;
	struct vnodeops *vops;
	int fs_flags;
	int fs_type;
	int refcnt;
	struct module *owner;
	struct vfsmount *kern_mnt; /* For kernel mount, if it's FS_SINGLE fs */
	struct file_system_type * next;
};

#define DECLARE_FSTYPE(var,type,read,flags) \
struct file_system_type var = { \
	name:		type, \
	read_super:	read, \
	fs_flags:	flags, \
	owner:		THIS_MODULE, \
}

#define MAX_NON_LFS	((1UL<<31) - 1)

#define DECLARE_FSTYPE_DEV(var,type,read) \
	DECLARE_FSTYPE(var,type,read,FS_REQUIRES_DEV)

/* Alas, no aliases. Too much hassle with bringing module.h everywhere */
#define fops_get(fops) \
	(((fops) && (fops)->owner)	\
		? ( try_inc_mod_count((fops)->owner) ? (fops) : NULL ) \
		: (fops))

#define fops_put(fops) \
do {	\
	if ((fops) && (fops)->owner) \
		__MOD_DEC_USE_COUNT((fops)->owner);	\
} while(0)

extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);
extern struct file_system_type *get_filesystem(int);
void reference_filesystem(struct file_system_type *);
void dereference_filesystem(struct file_system_type *);
extern struct vfsmount *kern_mount(struct file_system_type *);
extern void kern_umount(struct vfsmount *);
extern int may_umount(struct vfsmount *);
extern long do_mount(char *, char *, char *, unsigned long, void *);


extern int vfs_statfs(struct super_block *, struct statfs *);

/* Return value for VFS lock functions - tells locks.c to lock conventionally
 * REALLY kosha for root NFS and nfs_lock
 */ 
#define LOCK_USE_CLNT 1

#define FLOCK_VERIFY_READ  1
#define FLOCK_VERIFY_WRITE 2

extern int locks_mandatory_locked(struct inode *);
extern int locks_mandatory_area(int, struct inode *, struct file *, loff_t, size_t);

#if 0
/*
 * Candidates for mandatory locking have the setgid bit set
 * but no group execute bit -  an otherwise meaningless combination.
 */
#define MANDATORY_LOCK(inode) \
	(IS_MANDLOCK(inode) && ((inode)->i_mode & (S_ISGID | S_IXGRP)) == S_ISGID)

static inline int locks_verify_locked(struct inode *inode)
{
	if (MANDATORY_LOCK(inode))
		return locks_mandatory_locked(inode);
	return 0;
}

static inline int locks_verify_area(int read_write, struct inode *inode,
				    struct file *filp, loff_t offset,
				    size_t count)
{
	if (inode->i_flock && MANDATORY_LOCK(inode))
		return locks_mandatory_area(read_write, inode, filp, offset, count);
	return 0;
}

static inline int locks_verify_truncate(struct inode *inode,
				    struct file *filp,
				    loff_t size)
{
	if (inode->i_flock && MANDATORY_LOCK(inode))
		return locks_mandatory_area(
			FLOCK_VERIFY_WRITE, inode, filp,
			size < inode->i_size ? size : inode->i_size,
			(size < inode->i_size ? inode->i_size - size
			 : size - inode->i_size)
		);
	return 0;
}

extern inline int get_lease(struct inode *inode, unsigned int mode)
{
	if (inode->i_flock && (inode->i_flock->fl_flags & FL_LEASE))
		return __get_lease(inode, mode);
	return 0;
}
#endif

/* fs/open.c */


/* fs/dcache.c */
extern void vfs_caches_init(unsigned long);

#define	block_device	spec_info
#define __getname()	kmem_cache_alloc(names_cachep, SLAB_KERNEL)
#define putname(name)	kmem_cache_free(names_cachep, (void *)(name))

static char *__bdevname(dev_t dev, char *buffer)
{
	_snprintf(buffer, 16, "unknown-block");
	return buffer;
}
enum {BDEV_FILE, BDEV_SWAP, BDEV_FS, BDEV_RAW};
extern int register_blkdev(unsigned int, const char *, struct block_device_operations *);
extern int unregister_blkdev(unsigned int, const char *);
static int inline bdev_read_only(struct block_device *bd) { return (0); }
extern struct block_device *bdget(dev_t);
extern void bdput(struct block_device *);
static inline struct block_device *open_by_devnum(dev_t d, mode_t m) { return NULL; }
extern int blkdev_open(struct inode *, struct file *);
extern struct file_operations def_blk_fops;
extern struct file_operations def_fifo_fops;
extern int ioctl_by_bdev(struct block_device *, unsigned, unsigned long);
extern int blkdev_get(struct block_device *, mode_t, unsigned, int);
extern int blkdev_put(struct block_device *, int);
static inline  int bd_claim(struct block_device *bd, void *holder) { return 0; }
extern void invalidate_bdev(struct block_device *);
extern int sync_blockdev(kdev_t);
extern int blkdev_issue_zeroout(kdev_t devvp, sector_t sector, sector_t nr_sects);
static inline int sb_issue_zeroout(struct super_block *sb, sector_t block,
			sector_t nr_blocks, gfp_t gfp_mask)
{
	return blkdev_issue_zeroout(sb->s_dev,
		 block << (sb->s_blocksize_bits - 9),
			nr_blocks << (sb->s_blocksize_bits - 9));
}

static inline int sb_issue_discard(struct super_block *sb, sector_t block,
		sector_t nr_blocks, gfp_t gfp_mask, int flags)
{
}

/* assumes size > 256 */
static inline unsigned int blksize_bits(unsigned int size)
{
	unsigned int bits = 8;
	do {
		bits++;
		size >>= 1;
	} while (size > 256);
	return bits;
}

/* fs/devices.c */
extern const struct block_device_operations *get_blkfops(unsigned int);
extern int register_chrdev(unsigned int, const char *, struct file_operations *);
extern int unregister_chrdev(unsigned int, const char *);
extern int chrdev_open(struct inode *, struct file *);
#define	BDEVNAME_SIZE	32	/* Largest string for a blockdev identifier */
extern const char * bdevname(kdev_t);
extern const char * cdevname(kdev_t);
extern const char * kdevname(kdev_t);

#define kdevname(d)	devtoname(((struct vnode *)d)->v_rdev)
#define bdevname(d)	devtoname(((struct vnode *)d)->v_rdev)
#define kdev_t_to_nr(d)	(((struct vnode *)d)->v_rdev->si_udev)

extern void refile_buffer(struct buffer_head * buf);

#define BUF_CLEAN	0
#define BUF_LOCKED	1	/* Buffers scheduled for write */
#define BUF_DIRTY	2	/* Dirty buffers, not yet scheduled for write */
#define BUF_PROTECTED	3	/* Ramdisk persistent storage */
#define NR_LIST		4

static inline int buffer_uptodate(struct buffer_head *bh)
{
	if (test_bit(BH_Uptodate, &bh->b_state))
		return 1;
	/* This can happen on baread buffers */
	if (bh->b_flags & B_DONE) {
		set_bit(BH_Uptodate, &bh->b_state);
		return 1;
	}
	return 0;
}

/*
 * This is called by bh->b_end_io() handlers when I/O has completed.
 */
static inline void mark_buffer_uptodate(struct buffer_head * bh, int on)
{

	if (on) {
		set_bit(BH_Uptodate, &bh->b_state);
		bh->b_flags |= B_DONE;
	} else {
		clear_bit(BH_Uptodate, &bh->b_state);
		bh->b_flags &= ~B_DONE;
	}
}

#define atomic_set_buffer_clean(bh) test_and_clear_bit(BH_Dirty, &(bh)->b_state)

static inline void mark_buffer_clean(struct buffer_head * bh)
{
	bh->b_flags &= ~B_DELWRI;
	if (atomic_set_buffer_clean(bh)) {
		//XXX move it to clean queue
		refile_buffer(bh);
	}
}

#define atomic_set_buffer_dirty(bh) test_and_set_bit(BH_Dirty, &(bh)->b_state)

extern void buffer_insert_list(struct buffer_head *, struct list_head *);
#if 0
static inline void __mark_buffer_clean(struct buffer_head *bh)
{
	refile_buffer(bh);
}

static inline void mark_buffer_clean(struct buffer_head * bh)
{
	if (atomic_set_buffer_clean(bh))
		__mark_buffer_clean(bh);
}

#define atomic_set_buffer_protected(bh) test_and_set_bit(BH_Protected, &(bh)->b_state)

static inline void __mark_buffer_protected(struct buffer_head *bh)
{
	refile_buffer(bh);
}

static inline void mark_buffer_protected(struct buffer_head * bh)
{
	if (!atomic_set_buffer_protected(bh))
		__mark_buffer_protected(bh);
}


/*
 * If an error happens during the make_request, this function
 * has to be recalled. It marks the buffer as clean and not
 * uptodate, and it notifys the upper layer about the end
 * of the I/O.
 */
static inline void buffer_IO_error(struct buffer_head * bh)
{
	mark_buffer_clean(bh);
	/*
	 * b_end_io has to clear the BH_Uptodate bitflag in the error case!
	 */
	bh->b_end_io(bh, 0);
}
#endif

extern void buffer_insert_inode_queue(struct buffer_head *, struct inode *);
extern int fsync_buffers_list(struct list_head *);

extern int writeback_inodes_sb_if_idle(struct super_block *sb);

extern void balance_dirty(void);
extern void write_inode_buffers(kdev_t);
extern void write_old_buffers(kdev_t);
extern int check_disk_change(kdev_t);
extern int invalidate_inodes(struct super_block *);
extern void invalidate_inode_pages(struct inode *);
extern void invalidate_inode_buffers(struct inode *);
#define invalidate_buffers(dev)	__invalidate_buffers((dev), 0)
#define destroy_buffers(dev)	__invalidate_buffers((dev), 1)
extern void __invalidate_buffers(kdev_t dev, int);
extern void sync_inodes(kdev_t);
extern void write_inode_now(struct inode *, int);
extern void sync_dev(kdev_t, int);
extern int fsync_dev(kdev_t);
extern int osync_inode_buffers(struct inode *);
extern int inode_has_buffers(struct inode *);
extern void filemap_fdatasync(struct address_space *);
extern void filemap_fdatawait(struct address_space *);
#define	filemap_flush(mapp)	(printf("filemap_flush unimplemented\n"),0)
extern void sync_supers(kdev_t);
extern int bmap(struct inode *, int);
extern int notify_change(struct dentry *, struct iattr *);
extern int permission(struct inode *, int);
extern int vfs_permission(struct inode *, int);
extern int get_write_access(struct inode *);
extern int deny_write_access(struct file *);

extern void init_special_inode(struct inode *, umode_t, int);
/* Invalid inode operations -- fs/bad_inode.c */
extern void make_bad_inode(struct inode *);
extern int is_bad_inode(struct inode *);
extern void destroy_inode(struct inode *);
void add_super(struct super_block *);
void remove_super(struct super_block *);
void inode_hash_init(void);
int inode_hash_deinit(void);
#if 0
static inline void put_write_access(struct inode * inode)
{
	atomic_dec(&inode->i_writecount);
}
static inline void allow_write_access(struct file *file)
{
	if (file)
		atomic_inc(&file->f_dentry->d_inode->i_writecount);
}
extern int do_pipe(int *);

extern int open_namei(const char *, int, int, struct nameidata *);

extern int kernel_read(struct file *, unsigned long, char *, unsigned long);
extern struct file * open_exec(const char *);
 
/* fs/dcache.c -- generic fs support functions */
extern int is_subdir(struct dentry *, struct dentry *);
extern ino_t find_inode_number(struct dentry *, struct qstr *);

#endif

int fsync_inode_buffers(struct inode *);
int fsync_inode_data_buffers(struct inode *);

void mark_buffer_dirty_inode(struct buffer_head *bh, struct inode *inode);

/*
 * Kernel pointers have redundant information, so we can use a
 * scheme where we can return either an error code or a dentry
 * pointer with the same return value.
 *
 * This should be a per-architecture thing, to allow different
 * error and pointer decisions.
 */
static inline void *ERR_PTR(long error)
{
	return (void *) error;
}

static inline long PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static inline long IS_ERR(const void *ptr)
{
	return (unsigned long)ptr > (unsigned long)-1000L;
}

static inline void *ERR_CAST(const void *ptr)
{
	return (void *) ptr;
}

/*
 * The bitmask for a lookup event:
 *  - follow links at the end
 *  - require a directory
 *  - ending slashes ok even for nonexistent files
 *  - internal "there are more path compnents" flag
 */
#define LOOKUP_FOLLOW		(1)
#define LOOKUP_DIRECTORY	(2)
#define LOOKUP_CONTINUE		(4)
#define LOOKUP_POSITIVE		(8)
#define LOOKUP_PARENT		(16)
#define LOOKUP_NOALT		(32)
#define LOOKUP_ATOMIC		(64)
/*
 * Type of the last component on LOOKUP_PARENT
 */
enum {LAST_NORM, LAST_ROOT, LAST_DOT, LAST_DOTDOT, LAST_BIND};

#define DEFAULT_FS_OVERFLOWUID  65534
#define DEFAULT_FS_OVERFLOWGID  65534

/*
 * Since these macros are used in architectures that only need limited
 * 16-bit UID back compatibility, we won't use old_uid_t and old_gid_t
 */
#define fs_high2lowuid(uid) ((uid) > 65535 ? (uid16_t)fs_overflowuid : (uid16_t)(uid))
#define fs_high2lowgid(gid) ((gid) > 65535 ? (gid16_t)fs_overflowgid : (gid16_t)(gid))

#define low_16_bits(x)  ((x) & 0xFFFF)
#define high_16_bits(x) (((x) & 0xFFFF0000) >> 16)

extern int fs_overflowuid;
extern int fs_overflowgid;
extern void inode_init_once(struct inode *);
extern void iput(struct inode *);
static inline struct inode *igrab(struct inode *inode) { return inode; }
extern int inode_needs_sync(struct inode *inode);
typedef int (*find_inode_t)(struct inode *, unsigned long, void *);
extern struct inode *iget4(struct super_block *, unsigned long, find_inode_t, void *);
static inline struct inode *iget(struct super_block *sb, unsigned long ino)
{
	return iget4(sb, ino, NULL, NULL);
}
static inline struct inode *
iget_locked(struct super_block *sb, unsigned long ino)
{
#define	IGET_LOCKED	(void *)0xDEDEDE
	 return iget4(sb, ino, NULL, IGET_LOCKED);
}
extern int insert_inode_locked(struct inode *);
extern void unlock_new_inode(struct inode *);

extern void iget_failed(struct inode *);
extern void clear_inode(struct inode *);
extern void insert_inode_hash(struct inode *);
extern struct inode *new_inode(struct super_block *);
extern int inode_setattr(struct inode *, struct iattr *);

typedef int (get_block_t)(struct inode*,sector_t,struct buffer_head*,int);

extern void ll_rw_block(int, int, struct buffer_head * bh[]);

void inline submit_bh(int rw, struct buffer_head *bh)
{
	ASSERT(rw == READ);
	ASSERT(!(bh->b_flags & B_DONE));
	bh->b_flags |= B_READ;
	if (bh->b_end_io)
		bh->b_flags |= B_CALL;
	bufstrategy(bh);
}

kdev_t inline
to_kdev_t(udev_t num)
{
	kdev_t dvp;
	dev_t d;

#if 0
	d = udev2dev(num, 0);
	if (d) {
		vfinddev(d, VCHR, &dvp);
		return (dvp);
	}
#endif
	return (NULL);
}

static int inline is_read_only(kdev_t dev) { return (0); }
#if BIG_ZERO
extern void remove_suid(struct inode *inode);

extern void remove_inode_hash(struct inode *);
extern struct file * get_empty_filp(void);
extern void file_move(struct file *f, struct list_head *list);
extern void file_moveto(struct file *new, struct file *old);

extern struct buffer_head * getblk(kdev_t, int, int);
extern void __brelse(struct buffer_head *);
static inline void brelse(struct buffer_head *buf)
{
	if (buf)
		__brelse(buf);
}
extern void __bforget(struct buffer_head *);
static inline void bforget(struct buffer_head *buf)
{
	if (buf)
		__bforget(buf);
}
#endif
static inline struct buffer_head *
get_hash_table(kdev_t dev, int block, int blocksize)
{
	return gbincore(dev, block);
}
extern void set_blocksize(kdev_t, int);


static inline unsigned short bdev_logical_block_size(struct block_device *bdev)
{
	return 512;
}

static inline int sb_set_blocksize(struct super_block *sb, int size)
{
	set_blocksize(sb->s_dev, size);
	sb->s_blocksize = size;
	sb->s_blocksize_bits = blksize_bits(size);
	return sb->s_blocksize;
}

static inline int sb_min_blocksize(struct super_block *sb, int size)
{
	int minsize = bdev_logical_block_size(sb->s_dev);
	if (size < minsize)
		size = minsize;
	return sb_set_blocksize(sb, size);
}

static inline struct buffer_head * sb_bread(struct super_block *sb, int block)
{
	return linux_bread(sb->s_dev, block, sb->s_blocksize);
}

static inline void
sb_breadahead(struct super_block *sb, sector_t block)
{
	baread(sb->s_dev, (long)block, sb->s_blocksize);
}

static inline struct buffer_head * sb_getblk(struct super_block *sb, int block)
{
	return linux_getblk(sb->s_dev, block, sb->s_blocksize);
}

static inline struct buffer_head * sb_get_hash_table(struct super_block *sb, int block)
{
	return get_hash_table(sb->s_dev, block, sb->s_blocksize);
}

extern unsigned int get_hardblocksize(kdev_t);
//extern struct buffer_head * bread(kdev_t, int, int);
extern void wakeup_bdflush(int wait);
extern void bdflush_init(void);
extern void bdflush_exit(void);

extern int brw_page(int, struct page *, kdev_t, int [], int);


#define	block_write_begin(f,map,pos,len,flgs,pgp,fsd,getb)	\
	block_prepare_write(page,pos,len,getb)

#define	generic_write_end(f,map,pos,len,copied,pg,fsd)		\
	generic_commit_write(f,pg,pos,copied)

void page_zero_new_buffers(struct page *page, unsigned from, unsigned to);

/* Generic buffer handling for block filesystems.. */
extern int block_flushpage(struct page *, unsigned long);
extern int block_symlink(struct inode *, const char *, int);
extern int block_write_full_page(struct page*, get_block_t*);
extern int block_read_full_page(struct page*, get_block_t*);
extern int block_prepare_write(struct page*, unsigned, unsigned, get_block_t*);
extern int cont_prepare_write(struct page*, unsigned, unsigned, get_block_t*,
				unsigned long *);
extern int generic_cont_expand(struct inode *inode, loff_t size);
extern int block_sync_page(struct page *);
extern void flush_inode_pages (struct inode * inode);

int generic_block_bmap(struct address_space *, long, get_block_t *);
int generic_commit_write(struct file *, struct page *, unsigned, unsigned);
int block_truncate_page(struct address_space *, loff_t, get_block_t *);

extern int generic_file_mmap(struct file *, struct vm_area_struct *);
//extern int file_read_actor(read_descriptor_t * desc, struct page *page, unsigned long offset, unsigned long size);
extern ssize_t generic_file_read(struct file *, char *, size_t, loff_t *);
extern ssize_t generic_file_write(struct file *, const char *, size_t, loff_t *);
//extern void do_generic_file_read(struct file *, loff_t *, read_descriptor_t *, read_actor_t, int);

extern ssize_t generic_read_dir(struct file *, char *, size_t, loff_t *);
extern int generic_file_open(struct inode *, struct file *);
extern int generic_direct_IO(int, struct inode *, struct kiobuf *, daddr_t, int, get_block_t);
typedef void (dio_submit_t)(int rw, struct buffer_head*bio, struct inode *inode,
		loff_t file_offset);
extern int generic_file_direct_IO(struct inode *, struct uio *, int);
ssize_t __blockdev_direct_IO(int rw, struct kiocb *iocb, struct inode *inode,
	struct block_device *bdev, const struct iovec *iov, loff_t offset,
	unsigned long nr_segs, get_block_t get_block, dio_iodone_t end_io,
	dio_submit_t submit_io, int lock_type);

enum {
	/* need locking between buffered and direct access */
	DIO_LOCKING	= 0x01,

	/* filesystem does not support filling holes */
	DIO_SKIP_HOLES	= 0x02,
};


static inline ssize_t blockdev_direct_IO(int rw, struct kiocb *iocb,
	struct inode *inode, struct block_device *bdev, const struct iovec *iov,
	loff_t offset, unsigned long nr_segs, get_block_t get_block,
	dio_iodone_t end_io)
{
	return __blockdev_direct_IO(rw, iocb, inode, bdev, iov, offset,
				nr_segs, get_block, end_io, NULL,
				DIO_LOCKING | DIO_SKIP_HOLES);
}


extern struct file_operations generic_ro_fops;

static inline void nd_set_link(struct nameidata *nd, char *path)
{
        //nd->saved_names[nd->depth] = path;
	nd->ni_dirp = path; /* XXX */
}

static inline char *nd_get_link(struct nameidata *nd)
{
        //return nd->saved_names[nd->depth];
	return nd->ni_dirp; /* XXX */
}
extern int vfs_readlink(struct dentry *, char *, int, const char *);
extern int vfs_follow_link(struct nameidata *, const char *);
extern int page_readlink(struct dentry *, char *, int);
extern int page_follow_link(struct dentry *, struct nameidata *);
extern void *page_follow_link_light(struct dentry *, struct nameidata *);
extern void page_put_link(struct dentry *, struct nameidata *, void *);
extern struct inode_operations page_symlink_inode_operations;
extern int generic_readlink(struct dentry *, char __user *, int);

extern int vfs_readdir(struct file *, filldir_t, void *);
extern int dcache_readdir(struct file *, void *, filldir_t);

extern int get_sb_bdev(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data,
	int (*fill_super)(struct super_block *, void *, int),
	struct vfsmount *mnt);
extern struct file_system_type *get_fs_type(const char *name);
extern struct super_block *get_super(kdev_t);
struct super_block *get_empty_super(void);
extern void put_super(kdev_t);
unsigned long generate_cluster(kdev_t, int b[], int);
unsigned long generate_cluster_swab32(kdev_t, int b[], int);
extern kdev_t ROOT_DEV;
extern char root_device_name[];


extern void show_buffers(void);
extern void mount_root(void);

#ifdef CONFIG_BLK_DEV_INITRD
extern kdev_t real_root_dev;
extern int change_root(kdev_t, const char *);
#endif

extern ssize_t char_read(struct file *, char *, size_t, loff_t *);
extern ssize_t block_read(struct file *, char *, size_t, loff_t *);
extern int read_ahead[];

extern ssize_t char_write(struct file *, const char *, size_t, loff_t *);
extern ssize_t block_write(struct file *, const char *, size_t, loff_t *);

extern int file_fsync(struct file *, struct dentry *, int);
extern int generic_buffer_fdatasync(struct inode *inode, unsigned long start_idx, unsigned long end_idx);
extern int generic_osync_inode(struct inode *, int);

static inline int simple_fsync(struct file *fp, struct dentry *dentry, int dsync)
{
	write_inode_now(dentry->d_inode, dsync);
}

extern int inode_change_ok(struct inode *, struct iattr *);
static inline void nd_terminate_link(void *name, size_t len, size_t maxlen)
{
	((char *) name)[min(len, maxlen)] = '\0';
}

#if 0

/*
 * Common dentry functions for inclusion in the VFS
 * or in other stackable file systems.  Some of these
 * functions were in linux/fs/ C (VFS) files.
 *
 */

/*
 * Locking the parent is needed to:
 *  - serialize directory operations
 *  - make sure the parent doesn't change from
 *    under us in the middle of an operation.
 *
 * NOTE! Right now we'd rather use a "struct inode"
 * for this, but as I expect things to move toward
 * using dentries instead for most things it is
 * probably better to start with the conceptually
 * better interface of relying on a path of dentries.
 */
static inline struct dentry *lock_parent(struct dentry *dentry)
{
	struct dentry *dir = dget(dentry->d_parent);

	down(&dir->d_inode->i_sem);
	return dir;
}

static inline struct dentry *get_parent(struct dentry *dentry)
{
	return dget(dentry->d_parent);
}

static inline void unlock_dir(struct dentry *dir)
{
	up(&dir->d_inode->i_sem);
	dput(dir);
}

/*
 * Whee.. Deadlock country. Happily there are only two VFS
 * operations that does this..
 */
static inline void double_down(struct semaphore *s1, struct semaphore *s2)
{
	if (s1 != s2) {
		if ((unsigned long) s1 < (unsigned long) s2) {
			struct semaphore *tmp = s2;
			s2 = s1; s1 = tmp;
		}
		down(s1);
	}
	down(s2);
}

/*
 * Ewwwwwwww... _triple_ lock. We are guaranteed that the 3rd argument is
 * not equal to 1st and not equal to 2nd - the first case (target is parent of
 * source) would be already caught, the second is plain impossible (target is
 * its own parent and that case would be caught even earlier). Very messy.
 * I _think_ that it works, but no warranties - please, look it through.
 * Pox on bloody lusers who mandated overwriting rename() for directories...
 */

static inline void triple_down(struct semaphore *s1,
			       struct semaphore *s2,
			       struct semaphore *s3)
{
	if (s1 != s2) {
		if ((unsigned long) s1 < (unsigned long) s2) {
			if ((unsigned long) s1 < (unsigned long) s3) {
				struct semaphore *tmp = s3;
				s3 = s1; s1 = tmp;
			}
			if ((unsigned long) s1 < (unsigned long) s2) {
				struct semaphore *tmp = s2;
				s2 = s1; s1 = tmp;
			}
		} else {
			if ((unsigned long) s1 < (unsigned long) s3) {
				struct semaphore *tmp = s3;
				s3 = s1; s1 = tmp;
			}
			if ((unsigned long) s2 < (unsigned long) s3) {
				struct semaphore *tmp = s3;
				s3 = s2; s2 = tmp;
			}
		}
		down(s1);
	} else if ((unsigned long) s2 < (unsigned long) s3) {
		struct semaphore *tmp = s3;
		s3 = s2; s2 = tmp;
	}
	down(s2);
	down(s3);
}

static inline void double_up(struct semaphore *s1, struct semaphore *s2)
{
	up(s1);
	if (s1 != s2)
		up(s2);
}

static inline void triple_up(struct semaphore *s1,
			     struct semaphore *s2,
			     struct semaphore *s3)
{
	up(s1);
	if (s1 != s2)
		up(s2);
	up(s3);
}

static inline void double_lock(struct dentry *d1, struct dentry *d2)
{
	double_down(&d1->d_inode->i_sem, &d2->d_inode->i_sem);
}

static inline void double_unlock(struct dentry *d1, struct dentry *d2)
{
	double_up(&d1->d_inode->i_sem,&d2->d_inode->i_sem);
	dput(d1);
	dput(d2);
}
#endif /* BIG_ZERO */

#define lock_super(s)		FSLEEP_LOCK(&(s)->s_sblock)
#define unlock_super(s)		FSLEEP_UNLOCK(&(s)->s_sblock)


extern void *d_alloc_root(struct inode *ip);
extern void d_instantiate(struct dentry *, struct inode *);

static __inline void d_add(struct dentry *entry, struct inode *inode)
{
	d_instantiate(entry, inode);
	//ncache_entrer();
}

static __inline struct dentry *
d_splice_alias(struct inode *inode, struct dentry *dentry)
{
	d_add(dentry,inode);
	return dentry;
}

#define dput(root)
#define __mark_buffer_dirty	mark_buffer_dirty

extern void bforget(struct buffer_head *);
extern void __mark_dirty(struct buffer_head *bh);
extern void mark_buffer_dirty(struct buffer_head *bh);

#define __wait_on_buffer(bh)	biowait(bh)
extern inline void 
wait_on_buffer(struct buffer_head *bh)
{
// ext2_getblk lock_buffer needed during memset
	if (test_bit(BH_Lock, &bh->b_state))
		biowait(bh);
}

int generic_fs_mount(vfs_t *, vnode_t *, struct mount_args *, cred_t *);
int generic_fs_unmount(vfs_t *, int, cred_t *);
int generic_fs_root(vfs_t *, vnode_t **);
int generic_fs_statfs(vfs_t *, struct statfs *);
int generic_fs_sync(vfs_t *, int, cred_t *);
int generic_fs_vget(vfs_t *, vnode_t **, struct fid *);
int generic_fs_mountroot(vfs_t *, enum whymountroot);

int	generic_vop_open(vnode_t *, int, cred_t *);
int 	generic_vop_close(vnode_t *, int, cred_t *, struct file *);
int 	generic_vop_read(vnode_t *, struct uio *, int, cred_t *);
int 	generic_vop_write(vnode_t *, struct uio *, int, cred_t *);
int 	generic_vop_ioctl(vnode_t *, int, caddr_t, int, cred_t *);
int 	generic_vop_getattr(vnode_t *, int, vattr_t *, cred_t *);
int 	generic_vop_setattr(vnode_t *, int, vattr_t *, cred_t *);
int 	generic_vop_access(vnode_t *, int, cred_t *);
int 	generic_vop_lookup(vnode_t *, vnode_t **, struct componentname *);
int 	generic_vop_create(vnode_t *, vnode_t **, struct componentname *,
				vattr_t *);
int 	generic_vop_remove(vnode_t *, vnode_t *, struct componentname *);
int 	generic_vop_link(vnode_t *, vnode_t *, struct componentname *);
int 	generic_vop_rename(vnode_t *, vnode_t *, struct componentname *,
				vnode_t *, vnode_t *, struct componentname *);
int 	generic_vop_mkdir(vnode_t *, vnode_t **, struct componentname *,
				vattr_t *);
int 	generic_vop_rmdir(vnode_t *, vnode_t *, struct componentname *);
int 	generic_vop_readdir(vnode_t *, struct uio *, cred_t *, int *);
int 	generic_vop_symlink(vnode_t *, vnode_t **, struct componentname *,
				struct vattr *, char *);
int 	generic_vop_readlink(vnode_t *, struct uio *, cred_t *);
int 	generic_vop_fid(vnode_t *, fid_t *);
int 	generic_vop_fsync(vnode_t *, int, cred_t *);
void	generic_vop_inactive(vnode_t *);
void 	generic_vop_reclaim(vnode_t *);
int 	generic_vop_rwlock(vnode_t *, int);
void 	generic_vop_rwunlock(vnode_t *);
int 	generic_vop_bmap(vnode_t *, daddr_t, vnode_t **, daddr_t *,
			 int *, int *);
int 	generic_vop_strategy(vnode_t *, struct buf *);
int 	generic_vop_seek(vnode_t *, off_t, off_t *);
int	generic_vop_prewrite(vnode_t *, int, off64_t, uint_t, cred_t *);
int	generic_vop_commit_write(vnode_t *, int, off64_t, uint_t, cred_t *);
int	generic_vinit(struct vfs *vfsp, struct inode *ip, vnode_t **vpp);

#endif /* __KERNEL__ */

#endif /* _LINUX_FS_H */
