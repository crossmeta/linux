#ifndef _EXT2_TYPES_H
#define _EXT2_TYPES_H

/* 
 * If linux/types.h is already been included, assume it has defined
 * everything we need.  (cross fingers)
 */
#ifndef _LINUX_TYPES_H

typedef unsigned char __u8;
typedef signed char __s8;

#if (4 == 8)
typedef int		__s64;
typedef unsigned int	__u64;
#elif (4 == 8)
typedef long		__s64;
typedef unsigned long	__u64;
#elif (8 == 8)
#if defined(__GNUC__)
typedef __signed__ long long 	__s64;
#else
typedef signed __int64 __s64;
#endif
typedef unsigned __int64 __u64;
#endif

#if (4 == 2)
typedef	int		__s16;
typedef	unsigned int	__u16;
#elif (2 == 2)
typedef	short		__s16;
typedef	unsigned short	__u16;
#else
  ?==error: undefined 16 bit type
#endif

#if (4 == 4)
typedef	int		__s32;
typedef	unsigned int	__u32;
#elif (4 == 4)
typedef	long		__s32;
typedef	unsigned long	__u32;
#elif (2 == 4)
typedef	short		__s32;
typedef	unsigned short	__u32;
#else
 ?== error: undefined 32 bit type
#endif

#endif /* LINUX_TYPES_H */

#endif /* EXT2_TYPES_H */
