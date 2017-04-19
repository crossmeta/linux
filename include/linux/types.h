#ifndef _LINUX_TYPES_H_
#define _LINUX_TYPES_H_

#include <ntifs.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/kern_svcs.h>
#include <sys/uio.h>
#include <sys/cred.h>
#include <sys/conf.h>
//#include <linux/kernel.h>

/*
 * Additional type declarations for XFS
 */

typedef signed char	__int8_t, __s8;
typedef unsigned char	__uint8_t, __u8;
typedef signed short int	__int16_t, __s16;
typedef unsigned short int	__uint16_t, __u16;
typedef signed int	__int32_t, __s32;
typedef unsigned int	__uint32_t, __u32;
typedef signed __int64 	__int64_t, __s64;
typedef unsigned __int64 	__uint64_t, __u64;

typedef unsigned char		unchar;                                         
typedef unsigned short		ushort;                                         
typedef unsigned int		uint;                                           
typedef unsigned long		ulong;                                          

#ifndef pgoff_t
#define	pgoff_t	unsigned long
#endif
#define __bitwise
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

#ifdef __KERNEL__
typedef unsigned __bitwise	gfp_t;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed __int64 s64;
typedef unsigned __int64 u64;

#define BITS_PER_LONG 32

/* Dma addresses are 32-bits wide.  */

typedef u32 dma_addr_t;

typedef u64 sector_t;
typedef u64 blkcnt_t;

#endif /* __KERNEL__ */


/* POSIX Extensions */
typedef unsigned char	uchar_t;
typedef unsigned short	ushort_t;
typedef unsigned int	uint_t;
typedef unsigned long	ulong_t;

typedef __int64_t	prid_t;		/* project ID */
typedef	__uint32_t	inst_t;		/* an instruction */

typedef __uint32_t	app32_ulong_t;
typedef __uint32_t	app32_ptr_t;

#if 0
#if (BITS_PER_LONG == 32)
#define XFS_64	0
typedef __int64_t	sysarg_t;
#elif (BITS_PER_LONG == 64)
#define XFS_64	1
typedef int		sysarg_t;
#else
#error BITS_PER_LONG must be 32 or 64
#endif
#endif

typedef struct timespec	timespec_t;

typedef off_t	linux_off_t;
typedef unsigned long	linux_ino_t;
typedef __u64	loff_t;

#ifdef __KERNEL__
typedef struct {
        unsigned char   __u_bits[16];
} uuid_t;
#endif

typedef unsigned short umode_t;

/* posix-types.h */
typedef unsigned short  __kernel_dev_t;
typedef unsigned long   __kernel_ino_t;
typedef unsigned short  __kernel_mode_t;
typedef unsigned short  __kernel_nlink_t;
typedef long            __kernel_off_t;
typedef int             __kernel_pid_t;
typedef unsigned short  __kernel_ipc_pid_t;
typedef unsigned short  __kernel_uid_t;
typedef unsigned short  __kernel_gid_t;
typedef unsigned int    __kernel_size_t;
typedef int             __kernel_ssize_t;
typedef int             __kernel_ptrdiff_t;
typedef long            __kernel_time_t;
typedef long            __kernel_suseconds_t;
typedef long            __kernel_clock_t;
typedef int             __kernel_daddr_t;
typedef char *          __kernel_caddr_t;
typedef unsigned short  __kernel_uid16_t;
typedef unsigned short  __kernel_gid16_t;
typedef unsigned int    __kernel_uid32_t;
typedef unsigned int    __kernel_gid32_t;

typedef unsigned short  __kernel_old_uid_t;
typedef unsigned short  __kernel_old_gid_t;

typedef struct vnode *	kdev_t;

typedef __kernel_uid16_t	uid16_t;
typedef __kernel_gid16_t	gid16_t;

#define __inline__	__inline
#define inline		__inline

#define	DEFINE_FUNC(s)	static const char *__func__ = s

#define offsetof(TYPE, MEMBER)	((size_t) &((TYPE *)0)->MEMBER)

#if DBG
#ifndef DEBUG
#define DEBUG 1
#endif
#endif

#endif	/* _LINUX_TYPES_H */
