/*
 * CROSSMETA Windows porting changes.
 * Copyright (c) 2001-2011 Supramani Sammandam.  suprasam _at_ crossmeta.org
 */
#ifndef _AMD64_BITOPS_H
#define _AMD64_BITOPS_H

#define LOCKMP	lock
#define	cmpxchg(ptr,old,new)	(atomic_cmpset(ptr,old,new)? old : new)

static __inline void
set_bit(int nr, volatile void *addr)
{
	(void)_interlockedbittestandset(addr, nr);
}


static __inline void
clear_bit(int nr, volatile void *addr)
{
	(void)_interlockedbittestandreset(addr, nr);
}

static __inline void
__clear_bit(int nr, volatile void *addr)
{
	(void)_bittestandreset(addr, nr);
}

static __inline int test_and_set_bit(int nr, volatile void *addr)
{
	return _interlockedbittestandset(addr, nr);
}

/* non-atomic and it can be reordered */
static __inline int __test_and_set_bit(int nr, volatile void *addr)
{
	return _bittestandset(addr, nr);
}

static __inline int test_and_clear_bit(int nr, volatile void *addr)
{
	return _interlockedbittestandreset(addr, nr);
}

static __inline int __test_and_clear_bit(int nr, volatile void *addr)
{
	return _bittestandreset(addr, nr);
}

#if 0
static __inline int test_and_change_bit(int nr, LONG volatile *addr)
{
	LONG tBit;

	tBit = 1 << (nr & (sizeof (*addr)*8-1));
	return (BOOLEAN)
	  ((InterlockedXor(&addr[nr/(sizeof (*addr)*8)], tBit)&tBit) != 0);

}
#endif

static __inline int
test_bit(int nr, volatile void *addr)
{
	return _bittest(addr, nr);
}



/*
 * fls - find last bit (MSB) set
 */
#pragma intrinsic(_BitScanReverse)
static __inline int fls(int x)
{
	int pos=0;
	return 	_BitScanReverse(&pos, x)? (pos+1) : 0;
}

#pragma intrinsic(_BitScanForward)
static inline int ffs(int x)
{
	int pos = 0;
	return 	_BitScanForward(&pos, x)? (pos+1) : 0;
}

/*
 * __ffs - find first set bit in word
 * Undefined if no bit exists, so code should check against 0 first
 */
static __inline unsigned int __ffs(unsigned long x)
{
	int pos=0;
	return 	_BitScanForward(&pos, x)? (pos) : -1;
}
#define	BITS_PER_LONG	32
#define	BITOP_WORD(nr)	((nr) / BITS_PER_LONG)

#define	ffz(x)	__ffs(~(x))

/*
 * Find the first cleared bit in a memory region.
 */
static inline unsigned long
find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	const unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	while (size & ~(BITS_PER_LONG-1)) {
		if (~(tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	if (!size)
		return result;

	tmp = (*p) | (~0UL << size);
	if (tmp == ~0UL)	/* Are any bits zero? */
		return result + size;	/* Nope. */
found:
	return result + ffz(tmp);
}

static inline unsigned long 
find_next_zero_bit(const unsigned long *addr, unsigned long size,
				 unsigned long offset)
{
	const unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset %= BITS_PER_LONG;
	if (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		if (size < BITS_PER_LONG)
			goto found_first;
		if (~tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	while (size & ~(BITS_PER_LONG-1)) {
		if (~(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	if (tmp == ~0UL)	/* Are any bits zero? */
		return result + size;	/* Nope. */
found_middle:
	return result + ffz(tmp);
}


/*
 * Find the next set bit in a memory region.
 */
static __inline unsigned long
find_next_bit(const unsigned long *addr, unsigned long size,
			    unsigned long offset)
{
#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)
	const unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset %= BITS_PER_LONG;
	if (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		if (size < BITS_PER_LONG)
			goto found_first;
		if (tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	while (size & ~(BITS_PER_LONG-1)) {
		if ((tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	if (tmp == 0UL)		/* Are any bits set? */
		return result + size;	/* Nope. */
found_middle:
	return result + __ffs(tmp);
}

#ifdef __KERNEL__

#define ext2_set_bit                 __test_and_set_bit
#define ext2_clear_bit               __test_and_clear_bit
#define ext2_test_bit                test_bit
#define ext2_find_first_zero_bit     find_first_zero_bit
#define ext2_find_next_zero_bit      find_next_zero_bit
#define	ext2_find_next_bit(addr, size, off) \
		find_next_bit((unsigned long *)(addr), (size), (off))


#endif /* __KERNEL__ */

#endif /* _AMD64_BITOPS_H */
