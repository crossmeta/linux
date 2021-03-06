/*
 * CROSSMETA Windows porting changes.
 * Copyright (c) 2001-2011 Supramani Sammandam.  suprasam _at_ crossmeta.org
 */
#ifndef _I386_BITOPS_H
#define _I386_BITOPS_H

#define LOCKMP	lock
#define	cmpxchg(ptr,old,new)	(atomic_cmpset(ptr,old,new)? old : new)

static __inline void
set_bit(int nr, volatile void *addr)
{
	__asm {
		mov	ecx, nr
		mov	edx, addr
		LOCKMP bts [edx], ecx
	}
}


static __inline void
clear_bit(int nr, volatile void *addr)
{
	__asm {
		mov	ecx, nr
		mov	edx, addr
		LOCKMP btr [edx], ecx
	}
}

static __inline void
__clear_bit(int nr, volatile void *addr)
{
	__asm {
		mov	ecx, nr
		mov	edx, addr
		btr [edx], ecx
	}
}

static __inline int test_and_set_bit(int nr, volatile void *addr)
{
	_asm {
		mov	eax, nr
		mov	edx, addr
		LOCKMP bts [edx], eax
		sbb	eax, eax
	}
}

/* non-atomic and it can be reordered */
static __inline int __test_and_set_bit(int nr, volatile void *addr)
{
	_asm {
		mov	eax, nr
		mov	edx, addr
		bts 	[edx], eax
		sbb	eax, eax
	}
}

static __inline int test_and_clear_bit(int nr, volatile void *addr)
{
	_asm {
		mov	eax, nr
		mov	edx, addr
		LOCKMP btr [edx], eax
		sbb	eax, eax
	}
}

static __inline int __test_and_clear_bit(int nr, volatile void *addr)
{
	_asm {
		mov	eax, nr
		mov	edx, addr
		btr 	[edx], eax
		sbb	eax, eax
	}

}

static __inline int test_and_change_bit(int nr, volatile void *addr)
{
	_asm {
		mov	eax, nr
		mov	edx, addr
		LOCKMP btc [edx], eax
		sbb	eax, eax
	}
}


static __inline int
test_bit(int nr, volatile void *addr)
{
	__asm {
		mov	eax, nr
		mov	edx, addr
		bt	[edx], eax
		sbb	eax, eax
	}
}

/*
 * Find-bit routines..
 */
static __inline int
find_first_zero_bit(void *addr, unsigned count)
{
	__asm {
		mov	edi, addr
		mov	eax, count
		test	eax, eax
		je	short ffzb01
		lea	ecx,[31 + eax]
		shr	ecx, 5
		mov	ebx, edi
		mov	eax, -1
		xor	edx, edx
		repe scasd
		je ffzb02
		xor	eax, [edi - 4]
		sub	edi, 4
		bsf	edx, eax
	 ffzb02:
		sub	edi, ebx
		shl	edi, 3
		add	edx, edi
		mov	eax, edx
	 ffzb01:

	}
}

static __inline int
find_next_zero_bit (void *addr, int nbytes, int offset)
{
	unsigned long * p = ((unsigned long *) addr) + (offset >> 5);
	int set = 0, bit = (offset & 31), res;
	
	if (bit) {
		/*
		 * Look for zero in first byte
		 */
		__asm {
			mov	cl, byte ptr bit
			mov	eax, [p]
			mov	eax, [eax]
			shr	eax, cl
			not	eax
			bsf	eax, eax
			jne	fnzb01
			mov	eax, 32
		 fnzb01:
			mov	set, eax

		}
		if (set < (32 - bit))
			return (set + offset);
		set = 32 - bit;
		p++;
	}
	/*
	 * No zero yet, search remaining full bytes for a zero
	 */
	res = find_first_zero_bit(p, nbytes - 32 * (p - (unsigned long *)addr));
	return (offset + set + res);
}

/*
 * ffz = Find First Zero in word. Undefined if no zero exists,
 * so code should check against ~0UL first..
 */
static __inline unsigned long
ffz(unsigned long arg)
{
	__asm {
		mov	eax, arg
		not	eax
		bsf	eax, eax
	}
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

#endif /* _I386_BITOPS_H */
