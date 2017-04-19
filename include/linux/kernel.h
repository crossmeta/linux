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
#include <linux/types.h>
#include <linux/time.h>
#include <sys/byteorder.h>
#include <linux/bitops.h>

/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ __volatile__("": : :"memory")
#define	smp_wmb()
#define	smp_rmb()

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

#define STACK_MAGIC	0xdeadbeef

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define	DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

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
#define	likely(x)	x
#define	unlikely(x)	x
#define	noinline_for_stack	__declspec(noinline)
#define	__init
#define	__exit
#define	__user
#define	__releases(x)
#define	__acquires(x)
#define	container_of(ptr,type,member)	CONTAINING_RECORD(ptr,type,member)

#define	min_t(type,x,y) ((type)x < (type)y ? x: y)

#define printk	printf
#define	vprintk	vprintf
#define memcmp	bcmp

#define current		curproc()
#define fsuid		p_ucred->cr_uid
#define fsgid		p_ucred->cr_gid

#define GFP_KERNEL	KM_SLEEP
#define GFP_ATOMIC	KM_NOSLEEP
#define	GFP_NOFS	0

#define kmalloc(s,f)	kmem_alloc(s, M_TEMP, f)
#define	kzalloc(s,f)	kmem_zalloc(s, M_TEMP, f)
#define kfree(p)	kmem_free(p)
#define vmalloc(n)	kmem_alloc_paged(n, M_TEMP, 0)
#define vfree(p)	kmem_free(p)
#define	is_vmalloc_addr(p)	(0)
#define	kmem_cache	kmem_zone_s
#define	kmem_cache_create(s,sz,a,f,ctr)	kmem_zone_init(sz,s)
#define	kmem_cache_destroy(p)	kmem_zone_destroy(p)
#define	kmem_cache_alloc(p,f)	kmem_zone_alloc(p,f)
#define	kmem_cache_free(p,o)	kmem_zone_free(p,o)
#define	KMEM_CACHE(type,flags)	kmem_zone_init(sizeof (type), #type)
#define kmap(p)		(p)
#define kunmap(p)
#define Page_Uptodate(p)	(printf("Page_Uptodate"),0)

#define SPIN_LOCK_UNLOCKED	{0,0}
extern rwsleep_t kernel_iolock;
#define lock_kernel() 	RWSLEEP_WRLOCK(&kernel_iolock)
#define unlock_kernel() RWSLEEP_UNLOCK(&kernel_iolock)

//#define spinlock_t	lock_t
typedef struct spinlock_s {
	lock_t lock;
	pl_t irql;
} spinlock_t;

#define	spin_lock_init(l)	do { LOCK_INIT(&(l)->lock); (l)->irql = 0; } while (0)
#define spin_lock(l)		(l)->irql = LOCK(&(l)->lock)
#define spin_unlock(l)		do { 	\
	ASSERT((l)->irql <= DISPATCH_LEVEL);	\
	UNLOCK(&(l)->lock, (l)->irql);		\
} while (0)
#if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_WS03)
#define	LOCK_AVAIL(lock)	KeTestSpinLock(lock)
#else
#define	LOCK_AVAIL(lock)	(*(lock) == 0)
#endif
static __inline int spin_trylock(spinlock_t *p)
{
	if (LOCK_AVAIL(&p->lock)) {
		p->irql = LOCK(&p->lock);
		return (1);
	}
	return (0);
}
#define	spin_lock_irqsave(l,f)	do {(f) = LOCK(l);} while (0)
#define	spin_unlock_irqrestore(l,f)	UNLOCK(l, f)
#define	assert_spin_locked(x)	ASSERT(LOCK_OWNED(&x->lock))

#define	kmutex_t	fsleep_t
#define	mutex_init(lock, type, name)	FSLEEP_INIT(lock)
#define	mutex_lock(lock)	FSLEEP_LOCK(lock);
#define	mutex_unlock(lock)	FSLEEP_UNLOCK(lock);
#define	rw_semaphore_t	rwsleep_t
#define	init_rwsem(s)	RWSLEEP_INIT(s)
#define	destroy_rwsem(s)RWSLEEP_DEINIT(s)
#define	down_read(s)	RWSLEEP_RDLOCK(s)
#define	up_read(s)	RWSLEEP_UNLOCK(s)
#define	down_write(s)	RWSLEEP_WRLOCK(s)
#define	up_write(s)	RWSLEEP_UNLOCK(s)
#define	down_write_nested(s,class)	RWSLEEP_WRLOCK(s)
#define wait_queue_head_t event_t
#define DECLARE_WAIT_QUEUE_HEAD(x)	event_t x
#define init_waitqueue_head(q) EVENT_INIT(q)
#define sleep_on(q)		EVENT_WAIT(q, Executive)
#define wake_up(q)		EVENT_BROADCAST(q)
#define wake_up_all(q)		EVENT_BROADCAST(q)
#define	waitqueue_active(q)	(1)
#define	wait_event(q,cond)				\
do {							\
	if (cond)					\
		break;					\
	EVENT_WAIT(&q, Executive);			\
} while (1)
#define read_proc_t	void *
#define BUG()		panic("Kernel bug")
#define	WARN(a)		printf("WARNING: at %s:%d (%s)\n", __FILE__, __LINE__,a)
#define BUG_ON(cond)	do { if ((cond)) BUG(); } while (0)
#define	WARN_ON(cond)	((cond)? __warn_on(cond) : 0)
#define	dump_stack()	ASSERT(0)
#define	might_sleep()	PAGED_CODE()

static __inline int __warn_on(int cond)
{
	if (cond)
		printf("WARNING: at %s:%d (%s)\n", __FILE__, __LINE__);
	return (cond);
}

#define kernel_thread(fn, a, flags)  \
do {	\
	HANDLE __h;	\
	PsCreateSystemThread(&__h, THREAD_ALL_ACCESS, NULL, \
				NULL, NULL, fn, a);	\
} while (0)

#define	raw_smp_processor_id()	(0)
#ifndef HZ
#define	HZ	100
#endif
#define	USEC_PER_SEC	1000000L

#define set_task_ioprio(tsk,pri)	printf("set_task_ioprio unimplemented\n")
	
NTSYSAPI NTSTATUS NTAPI ZwYieldExecution(VOID);
#define schedule()	ZwYieldExecution()
#define yield()		ZwYieldExecution()
#define	cond_resched()	ZwYieldExecution()
#define	need_resched()	(0)

static signed long schedule_timeout_uninterruptible(long ticks)
{
	LARGE_INTEGER expire;

	expire.QuadPart = -((USEC_PER_SEC / HZ) * ticks * 10);
	printf("schedule_timeout: ticks %d -> %I64d\n", ticks, expire.QuadPart);
	KeDelayExecutionThread(KernelMode, FALSE, &expire);
	return 0;
}

#if defined(_M_IX86) || defined(_M_AMD64)
#define flush_dcache_page
#define get_unaligned(ptr)	(*(ptr))
#define put_unaligned(val, ptr)	((void)( *(ptr) = (val) ))

#define	le64_to_cpup(x)	(*(__u64 *)(x))
#define cpu_to_le64(x)	((__u64)(x))
#define cpu_to_le32(x)	((__u32)(x))
#define cpu_to_le16(x)	((__u16)(x))
#define le64_to_cpu(x)	((__u64)(x))
#define le32_to_cpu(x)	((__u32)(x))
#define le16_to_cpu(x)	((__u16)(x))
#define	be32_to_cpu	__be32_to_cpu
#define	__be32_to_cpu(x)	__swab32(( __u32)(__be32)(x))
#endif

static __inline void le16_add_cpu(__u16 *var, __u16 val)
{
	*var = cpu_to_le16(le16_to_cpu(*var) + val);
}

static inline void le32_add_cpu(__le32 *var, u32 val)
{
	*var = cpu_to_le32(le32_to_cpu(*var) + val);
}


#define do_div(a, b)	xfs_do_div(&(a), (b), sizeof(a))

/* Move the kernel do_div definition off to one side */
static inline __u32 xfs_do_div(void *a, __u32 b, int n)
{
	__u32	mod;

	switch (n) {
		case 4:
			mod = *(__u32 *)a % b;
			*(__u32 *)a = *(__u32 *)a / b;
			return mod;
		case 8:
			mod = *(__u64 *)a % b;
			*(__u64 *)a = *(__u64 *)a /  b;
			return mod;
	}

	/* NOTREACHED */
	return 0;
}

#endif /* __KERNEL__ */

#ifdef i386
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
#endif

#ifdef _WIN64
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
_CRTIMP int __cdecl _snprintf(char *, size_t, const char *, ...);


#endif
