/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
/* truncate64 syscall.  Copes with 64 bit and 32 bit machines
 * and on 32 bit machines this sends things into the kernel as
 * two 32-bit arguments (high and low 32 bits of length) that
 * are ordered based on endianess.  It turns out endian.h has
 * just the macro we need to order things, __LONG_LONG_PAIR.
 */

#include <_lfs_64.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef __NR_truncate64
# include <bits/wordsize.h>

# if __WORDSIZE == 64
_syscall2(int, truncate64, const char *, path, __off64_t, length)
# elif __WORDSIZE == 32
#  include <endian.h>
#  include <stdint.h>
int truncate64(const char * path, __off64_t length)
{
	uint32_t low = length & 0xffffffff;
	uint32_t high = length >> 32;
#  if defined(__UCLIBC_TRUNCATE64_HAS_4_ARGS__)
	return INLINE_SYSCALL(truncate64, 4, path, 0,
			__LONG_LONG_PAIR(high, low));
#  else
	return INLINE_SYSCALL(truncate64, 3, path,
			__LONG_LONG_PAIR(high, low));
#  endif
}
# else
#  error Your machine is not 64 bit nor 32 bit, I am dazed and confused.
# endif

#else
# include <errno.h>
int truncate64(const char * path, __off64_t length)
{
	__off_t x = (__off_t) length;

	if (x == length) {
		return truncate(path, x);
	}

	__set_errno((x < 0) ? EINVAL : EFBIG);

	return -1;
}
#endif
