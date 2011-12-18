/*
 * CROSSMETA Windows porting changes.
 * Copyright (c) 2001-2011 Supramani Sammandam.  suprasam _at_ crossmeta.org
 */
#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */

#ifdef __KERNEL__

#include <stdarg.h>
#ifdef i386
#include <linux/i386-bitops.h>
#else
#error "bitops.h needed for your platform"
#endif 

/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ __volatile__("": : :"memory")

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

#define STACK_MAGIC	0xdeadbeef

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define	KERN_EMERG	"<0>"	/* system is unusable			*/
#define	KERN_ALERT	"<1>"	/* action must be taken immediately	*/
#define	KERN_CRIT	"<2>"	/* critical conditions			*/
#define	KERN_ERR	"<3>"	/* error conditions			*/
#define	KERN_WARNING	"<4>"	/* warning conditions			*/
#define	KERN_NOTICE	"<5>"	/* normal but significant condition	*/
#define	KERN_INFO	"<6>"	/* informational			*/
#define	KERN_DEBUG	"<7>"	/* debug-level messages			*/

#define NORET_TYPE    /**/
#define ATTRIB_NORET  __attribute__((noreturn))
#define NORET_AND     noreturn,

#define PAGE_CACHE_SIZE		PAGE_SIZE
#define PAGE_CACHE_SHIFT	PAGE_SHIFT

#define ENOTSUPP	EOPNOTSUPP

#define __attribute__(x)

#define printk	printf
#define memcmp	bcmp

#define CURRENT_TIME	time_second()

#define current		curproc()
#define fsuid		p_ucred->cr_uid
#define fsgid		p_ucred->cr_gid

#define GFP_KERNEL	0

#define kmalloc(s,f)	kmem_alloc(s, M_TEMP, 0)
#define kfree(p)	kmem_free(p)
#define vmalloc(n)	kmem_alloc_paged(n, M_TEMP, 0)
#define vfree(p)	kmem_free(p)
#define kmap(p)		(p)
#define kunmap(p)
#define Page_Uptodate(p)	(printf("Page_Uptodate"),0)

#define SPIN_LOCK_UNLOCKED	0
extern rwsleep_t kernel_iolock;
#define lock_kernel() 	RWSLEEP_WRLOCK(&kernel_iolock)
#define unlock_kernel() RWSLEEP_UNLOCK(&kernel_iolock)

#define spinlock_t	lock_t
#define spin_lock(l)	LOCK(l)
#define spin_unlock(l,f) UNLOCK(l, f)
#define wait_queue_head_t event_t
#define DECLARE_WAIT_QUEUE_HEAD(x)	event_t x
#define init_waitqueue_head(q) EVENT_INIT(q)
#define sleep_on(q)		EVENT_WAIT(q, Executive)
#define wake_up(q)		EVENT_BROADCAST(q)
#define read_proc_t	void *
#define BUG()		panic("Kernel bug")

#define kernel_thread(fn, a, flags)  \
do {	\
	HANDLE __h;	\
	PsCreateSystemThread(&__h, THREAD_ALL_ACCESS, NULL, \
				NULL, NULL, fn, a);	\
} while (0)

NTSYSAPI NTSTATUS NTAPI ZwYieldExecution(VOID);
#define schedule()	ZwYieldExecution()
#define yield()		ZwYieldExecution()

#ifdef i386
#define  __LITTLE_ENDIAN	1
#define flush_dcache_page
#define get_unaligned(ptr)	(*(ptr))
#define put_unaligned(val, ptr)	((void)( *(ptr) = (val) ))

#define cpu_to_le64(x)	((__u64)(x))
#define cpu_to_le32(x)	((__u32)(x))
#define cpu_to_le16(x)	((__u16)(x))
#define le64_to_cpu(x)	((__u64)(x))
#define le32_to_cpu(x)	((__u32)(x))
#define le16_to_cpu(x)	((__u16)(x))
#endif

#endif /* __KERNEL__ */

/* find the first occurrence of byte 'c', or 1 past the area if none */
static __inline void *
memscan(void *addr, int c, size_t n)
{
	if (!n)
		return (addr);
	__asm {
		mov	edi, addr
		mov	ecx, n
		mov	eax, c
		repnz scasb
		jnz 	short msc01
		dec 	edi
	msc01:
		mov	eax, edi
	}
}

#if 0
static __inline void *
memscan(void *addr, int c, size_t n)
{
	unsigned char *p, *e;

	if (!n)
		return (addr);
	p = addr;
	e = p + n;
	while (p != e) {
		if (*p == (unsigned char)c)
			return (p);
		p++;
	}
	return (p);
}
#endif


#define vsprintf(b, s, va)	_vsnprintf(b, 4096, s, va) 
_CRTIMP int __cdecl _vsnprintf(char *, size_t, const char *, va_list);


#endif
