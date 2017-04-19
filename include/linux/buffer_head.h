#ifndef	_LINUX_BUFFER_HEAD_H
#define	_LINUX_BUFFER_HEAD_H

#include <linux/fs.h>
#include <linux/aio.h>

#define	set_buffer_uptodate(bh)	mark_buffer_uptodate(bh,1)
#define	set_buffer_mapped(bh)	bh->b_state |= (1UL << BH_Mapped)
#define	set_buffer_new(bh)	bh->b_state |= (1UL << BH_New)

static inline void set_buffer_delay(struct buffer_head *bh)
{
	set_bit(BH_Delay, &bh->b_state);
}

static inline int buffer_delay(struct buffer_head *bh)
{
	return test_bit(BH_Delay, &bh->b_state);
}

static inline void set_buffer_boundary(struct buffer_head *bh)
{
	set_bit(BH_Boundary, &bh->b_state);
}

static inline void set_buffer_unwritten(struct buffer_head *bh)
{
	set_bit(BH_Unwritten, &bh->b_state);
}

static inline int buffer_unwritten(struct buffer_head *bh)
{
	return test_bit(BH_Unwritten, &bh->b_state);
}

#define	buffer_write_io_error(bh)	geterror(bh)
static inline void clear_buffer_write_io_error(struct buffer_head *bh)
{
	if (geterror(bh)) {
		bh->b_flags &= B_ERROR;
		bh->b_error = 0;
	}
}

static inline void clear_buffer_mapped(struct buffer_head *bh)
{
	clear_bit(BH_Mapped, &bh->b_state);
}

static inline void clear_buffer_new(struct buffer_head *bh)
{
	clear_bit(BH_New, &bh->b_state);
}

static inline void clear_buffer_unwritten(struct buffer_head *bh)
{
	clear_bit(BH_Unwritten, &bh->b_state);
}

static inline void SetPageUptodate(struct page *pg)
{
	if (page_buffers(pg))
		mark_buffer_uptodate(page_buffers(pg),1);
	set_bit(PG_uptodate, &pg->flags);
}

#define	sb_find_get_block(sb,block)	sb_getblk(sb,block)

static inline void
map_bh(struct buffer_head *bh, struct super_block *sb, sector_t block)
{
	set_buffer_mapped(bh);
	bh->b_dev = sb->s_dev;
	bh->b_blocknr = block;
	bh->b_size = sb->s_blocksize;
}

static inline int bd_release(struct block_device *bdev)
{
	printk("bd_release: dev %p\n", bdev);
	return 0;
}

static inline int blkdev_put(void *bdev, mode_t mode)
{
	vrele((kdev_t)bdev);
}

static inline int blkdev_issue_flush(struct block_device *bdev,
			sector_t *error_sector)
{
}
static inline int __blkdev_issue_flush(struct block_device *bdev, gfp_t flags,
			sector_t *error_sector)
{
}

void end_buffer_read_sync(struct buffer_head *bh);
int bh_uptodate_or_lock(struct buffer_head *bh);
int bh_submit_read(struct buffer_head *bh);
int sync_dirty_buffer(struct buffer_head *bh);

void unmap_underlying_metadata(struct superblock *sb, sector_t block);

static inline int block_write_end(struct file *file,
	struct address_space *mapping, loff_t pos, unsigned len,
	unsigned copied, struct page *page, void *fsdata)
{
	return generic_commit_write(file, page, pos, len);
}

#endif

