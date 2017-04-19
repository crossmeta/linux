#ifndef __LINUX__AIO_H
#define	__LINUX__AIO_H
#include <linux/iobuf.h>

/* Linux kernel 2.6 kiocb */
struct kiocb {
	struct kiobuf 	ki_iobuf;
	long		ki_flags;
	struct file	*ki_filp;
	loff_t		ki_pos;
	size_t		ki_left;
	void		*private;
};

#define	is_sync_kiocb(iocb)	(1)

static inline int aio_complete(struct kiocb *iocb, long res, long res2)
{
	printf("aio_complete: ignored\n");
	return 0;
}

#endif
