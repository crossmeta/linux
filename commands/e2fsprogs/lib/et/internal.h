/*
 * internal include file for com_err package
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose is hereby granted, provided that
 * the names of M.I.T. and the M.I.T. S.I.P.B. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  M.I.T. and the
 * M.I.T. S.I.P.B. make no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */
#ifndef __STDC__
#undef const
#define const
#endif

#include <errno.h>

#ifdef NEED_SYS_ERRLIST
extern char const * const sys_errlist[];
extern const int sys_nerr;
#endif

#if 0
/* AIX and Ultrix have standard conforming header files. */
#if !defined(ultrix) && !defined(_AIX)
#ifdef __STDC__
void perror (const char *);
#endif
#endif
#endif
