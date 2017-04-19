
#ifndef _LINUX_EXPORTFS_H
#define _LINUX_EXPORTFS_H

struct export_operations {
	int (*encode_fh)(struct dentry *de, __u32 *fh, int *max_len,
			int connectable);
	struct inode * (*fh_to_inode)(struct super_block *sb,
				u64 ino, u32 generation);
};

extern struct dentry *generic_fh_to_dentry(struct super_block *sb,
	struct fid *fid, int fh_len, int fh_type,
	struct inode *(*get_inode) (struct super_block *sb, u64 ino, u32 gen));

#endif
	
