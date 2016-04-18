/* Copyright (C) 1991,92,93,95,96,97,98,99,2000,2001 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with the GNU C Library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.  */

#ifndef	_FEATURES_H
#define	_FEATURES_H	1

/* For uClibc, always optimize for size -- this should disable
* a lot of expensive inlining... */
#define __OPTIMIZE_SIZE__   1

/* These are defined by the user (or the compiler)
to specify the desired environment:

__STRICT_ANSI__	ISO Standard C.
_ISOC99_SOURCE	Extensions to ISO C89 from ISO C99.
_POSIX_SOURCE	IEEE Std 1003.1.
_POSIX_C_SOURCE	If ==1, like _POSIX_SOURCE; if >=2 add IEEE Std 1003.2;
if >=199309L, add IEEE Std 1003.1b-1993;
if >=199506L, add IEEE Std 1003.1c-1995
_XOPEN_SOURCE	Includes POSIX and XPG things.  Set to 500 if
Single Unix conformance is wanted, to 600 for the
upcoming sixth revision.
_XOPEN_SOURCE_EXTENDED XPG things and X/Open Unix extensions.
_LARGEFILE_SOURCE	Some more functions for correct standard I/O.
_LARGEFILE64_SOURCE	Additional functionality from LFS for large files.
_FILE_OFFSET_BITS=N	Select default filesystem interface.
_BSD_SOURCE		ISO C, POSIX, and 4.3BSD things.
_SVID_SOURCE		ISO C, POSIX, and SVID things.
_GNU_SOURCE		All of the above, plus GNU extensions.
_REENTRANT		Select additionally reentrant object.
_THREAD_SAFE		Same as _REENTRANT, often used by other systems.

The `-ansi' switch to the GNU C compiler defines __STRICT_ANSI__.
If none of these are defined, the default is to have _SVID_SOURCE,
_BSD_SOURCE, and _POSIX_SOURCE set to one and _POSIX_C_SOURCE set to
199506L.  If more than one of these are defined, they accumulate.
For example __STRICT_ANSI__, _POSIX_SOURCE and _POSIX_C_SOURCE
together give you ISO C, 1003.1, and 1003.2, but nothing else.

These are defined by this file and are used by the
header files to decide what to declare or define:

__USE_ISOC99		Define ISO C99 things.
__USE_POSIX		Define IEEE Std 1003.1 things.
__USE_POSIX2		Define IEEE Std 1003.2 things.
__USE_POSIX199309	Define IEEE Std 1003.1, and .1b things.
__USE_POSIX199506	Define IEEE Std 1003.1, .1b, .1c and .1i things.
__USE_XOPEN		Define XPG things.
__USE_XOPEN_EXTENDED	Define X/Open Unix things.
__USE_UNIX98		Define Single Unix V2 things.
__USE_XOPEN2K        Define XPG6 things.
__USE_LARGEFILE	Define correct standard I/O things.
__USE_LARGEFILE64	Define LFS things with separate names.
__USE_FILE_OFFSET64	Define 64bit interface as default.
__USE_BSD		Define 4.3BSD things.
__USE_SVID		Define SVID things.
__USE_MISC		Define things common to BSD and System V Unix.
__USE_GNU		Define GNU extensions.
__USE_REENTRANT	Define reentrant/thread-safe *_r functions.
__FAVOR_BSD		Favor 4.3BSD things in cases of conflict.

The macros `__GNU_LIBRARY__', `__GLIBC__', and `__GLIBC_MINOR__' are
defined by this file unconditionally.  `__GNU_LIBRARY__' is provided
only for compatibility.  All new code should use the other symbols
to test for features.

All macros listed above as possibly being defined by this file are
explicitly undefined if they are not explicitly defined.
Feature-test macros that are not defined by the user or compiler
but are implied by the other feature-test macros defined (or by the
lack of any definitions) are defined by the file.  */

/* Undefine everything, so we get a clean slate.  */
#undef	__USE_ISOC99
#undef	__USE_POSIX
#undef	__USE_POSIX2
#undef	__USE_POSIX199309
#undef	__USE_POSIX199506
#undef	__USE_XOPEN
#undef	__USE_XOPEN_EXTENDED
#undef	__USE_UNIX98
#undef	__USE_XOPEN2K
#undef	__USE_LARGEFILE
#undef	__USE_LARGEFILE64
#undef	__USE_FILE_OFFSET64
#undef	__USE_BSD
#undef	__USE_SVID
#undef	__USE_MISC
#undef	__USE_GNU
#undef	__USE_REENTRANT
#undef	__FAVOR_BSD
#undef	__KERNEL_STRICT_NAMES

/* Suppress kernel-name space pollution unless user expressedly asks
for it.  */
#ifndef _LOOSE_KERNEL_NAMES
# define __KERNEL_STRICT_NAMES
#endif

/* Always use ISO C things.  */
#define	__USE_ANSI	1


/* If _BSD_SOURCE was defined by the user, favor BSD over POSIX.  */
#if defined _BSD_SOURCE && \
    !(defined _POSIX_SOURCE || defined _POSIX_C_SOURCE || \
      defined _XOPEN_SOURCE || defined _XOPEN_SOURCE_EXTENDED || \
      defined _GNU_SOURCE || defined _SVID_SOURCE)
# define __FAVOR_BSD	1
#endif

/* If _GNU_SOURCE was defined by the user, turn on all the other features.  */
#ifdef _GNU_SOURCE
# undef  _ISOC99_SOURCE
# define _ISOC99_SOURCE	1
# undef  _POSIX_SOURCE
# define _POSIX_SOURCE	1
# undef  _POSIX_C_SOURCE
# define _POSIX_C_SOURCE	199506L
# undef  _XOPEN_SOURCE
# define _XOPEN_SOURCE	600
# undef  _XOPEN_SOURCE_EXTENDED
# define _XOPEN_SOURCE_EXTENDED	1
# undef	 _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE	1
# undef  _BSD_SOURCE
# define _BSD_SOURCE	1
# undef  _SVID_SOURCE
# define _SVID_SOURCE	1
#endif

/* If nothing (other than _GNU_SOURCE) is defined,
define _BSD_SOURCE and _SVID_SOURCE.  */
#if (!defined __STRICT_ANSI__ && !defined _ISOC99_SOURCE && \
     !defined _POSIX_SOURCE && !defined _POSIX_C_SOURCE && \
     !defined _XOPEN_SOURCE && !defined _XOPEN_SOURCE_EXTENDED && \
     !defined _BSD_SOURCE && !defined _SVID_SOURCE)
# define _BSD_SOURCE	1
# define _SVID_SOURCE	1
#endif

/* This is to enable the ISO C99 extension.  Also recognize the old macro
which was used prior to the standard acceptance.  This macro will
eventually go away and the features enabled by default once the ISO C99
standard is widely adopted.  */
#if (defined _ISOC99_SOURCE || defined _ISOC9X_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
# define __USE_ISOC99	1
#endif

/* If none of the ANSI/POSIX macros are defined, use POSIX.1 and POSIX.2
(and IEEE Std 1003.1b-1993 unless _XOPEN_SOURCE is defined).  */
#if (!defined __STRICT_ANSI__ && !defined _POSIX_SOURCE && \
     !defined _POSIX_C_SOURCE)
# define _POSIX_SOURCE	1
# if defined _XOPEN_SOURCE && (_XOPEN_SOURCE - 0) < 500
#  define _POSIX_C_SOURCE	2
# else
#  define _POSIX_C_SOURCE	199506L
# endif
#endif

#if defined _POSIX_SOURCE || _POSIX_C_SOURCE >= 1 || defined _XOPEN_SOURCE
# define __USE_POSIX	1
#endif

#if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 2 || defined _XOPEN_SOURCE
# define __USE_POSIX2	1
#endif

#if (_POSIX_C_SOURCE - 0) >= 199309L
# define __USE_POSIX199309	1
#endif

#if (_POSIX_C_SOURCE - 0) >= 199506L
# define __USE_POSIX199506	1
#endif

#ifdef	_XOPEN_SOURCE
# define __USE_XOPEN	1
# if (_XOPEN_SOURCE - 0) >= 500
#  define __USE_XOPEN_EXTENDED	1
#  define __USE_UNIX98	1
#  undef _LARGEFILE_SOURCE
#  define _LARGEFILE_SOURCE	1
#  if (_XOPEN_SOURCE - 0) >= 600
#   define __USE_XOPEN2K	1
#   undef __USE_ISOC99
#   define __USE_ISOC99		1
#  endif
# else
#  ifdef _XOPEN_SOURCE_EXTENDED
#   define __USE_XOPEN_EXTENDED	1
#  endif
# endif
#endif

#ifdef _LARGEFILE_SOURCE
# define __USE_LARGEFILE	1
#endif

#ifdef _LARGEFILE64_SOURCE
# define __USE_LARGEFILE64	1
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
# define __USE_FILE_OFFSET64	1
#endif

#if defined _BSD_SOURCE || defined _SVID_SOURCE
# define __USE_MISC	1
#endif

#ifdef	_BSD_SOURCE
# define __USE_BSD	1
#endif

#ifdef	_SVID_SOURCE
# define __USE_SVID	1
#endif

#ifdef	_GNU_SOURCE
# define __USE_GNU	1
#endif

#if defined _REENTRANT || defined _THREAD_SAFE
# define __USE_REENTRANT	1
#endif

/* We do support the IEC 559 math functionality, real and complex.  */
#define __STDC_IEC_559__		1
#define __STDC_IEC_559_COMPLEX__	1

/* This macro indicates that the installed library is uClibc.  Use
* __UCLIBC_MAJOR__ and __UCLIBC_MINOR__ to test for the features in
* specific releases.  */
#define	__UCLIBC__		1

/* Load up the current set of uClibc supported features along
* with the current uClibc major and minor version numbers.
* For uClibc release 0.9.26, these numbers would be:
*	#define	__UCLIBC_MAJOR__	0
*	#define	__UCLIBC_MINOR__	9
*	#define	__UCLIBC_SUBLEVEL__	26
*/
#define __need_uClibc_config_h
//JONNY//#include <bits/uClibc_config.h>
#undef __need_uClibc_config_h

#ifdef __UCLIBC_HAS_WCHAR__
/* wchar_t uses ISO 10646-1 (2nd ed., published 2000-09-15) / Unicode 3.0.  */
# define __STDC_ISO_10646__		200009L
#endif

/*  There is an unwholesomely huge amount of code out there that depends on the
*  presence of GNU libc header files.  We have GNU libc header files.  So here
*  we commit a horrible sin.  At this point, we _lie_ and claim to be GNU libc
*  to make things like /usr/include/linux/socket.h and lots of apps work as
*  their developers intended.  This is IMHO, pardonable, since these defines
*  are not really intended to check for the presence of a particular library,
*  but rather are used to define an _interface_.  */
#if !defined __FORCE_NOGLIBC && (!defined _LIBC || defined __FORCE_GLIBC)
#   define __GNU_LIBRARY__ 6
#   define __GLIBC__       2
#   define __GLIBC_MINOR__ 2
#endif

/* Convenience macros to test the versions of glibc and gcc.
Use them like this:
#if __GNUC_PREREQ (2,8)
... code requiring gcc 2.8 or later ...
#endif
Note - they won't work for gcc1 or glibc1, since the _MINOR macros
were not defined then.  */
#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif

#define __GLIBC_PREREQ(maj, min) \
	((__GLIBC__ << 16) + __GLIBC_MINOR__ >= ((maj) << 16) + (min))

/* This is here only because every header file already includes this one.  */
#ifndef __ASSEMBLER__
#ifndef _SYS_CDEFS_H
//JONNY//# include <sys/cdefs.h>
#endif

/* If we don't have __REDIRECT, prototypes will be missing if
__USE_FILE_OFFSET64 but not __USE_LARGEFILE[64]. */
# if defined __USE_FILE_OFFSET64 && !defined __REDIRECT
#  define __USE_LARGEFILE	1
#  define __USE_LARGEFILE64	1
# endif

#endif	/* !ASSEMBLER */

/* Decide whether we can define 'extern inline' functions in headers.  */
#if __GNUC_PREREQ (2, 7) && defined __OPTIMIZE__ \
    && !defined __OPTIMIZE_SIZE__ && !defined __NO_INLINE__
# define __USE_EXTERN_INLINES	1
#endif


/* Make sure users large file options agree with uClibc's configuration. */
#ifndef __UCLIBC_HAS_LFS__

/* If uClibc was built without large file support, output an error if
* and 64-bit file offsets were requested, output an error.
* NOTE: This is probably incorrect on a 64-bit arch... */
#ifdef __USE_FILE_OFFSET64
#error It appears you have defined _FILE_OFFSET_BITS=64.  Unfortunately, \
uClibc was built without large file support enabled.
#endif

/* If uClibc was built without large file support and _LARGEFILE64_SOURCE
* is defined, undefine it. */
#if defined(_LARGEFILE64_SOURCE)
#undef _LARGEFILE64_SOURCE
#undef __USE_LARGEFILE64
#endif

/* If we're actually building uClibc with large file support,
* define __USE_LARGEFILE64 and __USE_LARGEFILE. */
#elif defined(_LIBC)
#undef _LARGEFILE_SOURCE
#undef _LARGEFILE64_SOURCE
#undef _FILE_OFFSET_BITS
#undef __USE_LARGEFILE
#undef __USE_LARGEFILE64
#undef __USE_FILE_OFFSET64
#define _LARGEFILE_SOURCE       1
#define _LARGEFILE64_SOURCE     1
#define __USE_LARGEFILE         1
#define __USE_LARGEFILE64       1
#endif

/* Some nice features only work properly with ELF */
#if defined _LIBC 
#if defined __HAVE_ELF__
/* Define ALIASNAME as a weak alias for NAME. */
#  define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#  define _weak_alias(name, aliasname) \
      extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));
/* Define ALIASNAME as a strong alias for NAME.  */
# define strong_alias(name, aliasname) _strong_alias(name, aliasname)
# define _strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));
/* This comes between the return type and function name in
*    a function definition to make that definition weak.  */
# define weak_function __attribute__ ((weak))
# define weak_const_function __attribute__ ((weak, __const__))
/* Tacking on "\n\t#" to the section name makes gcc put it's bogus
* section attributes on what looks like a comment to the assembler. */
#  if defined(__cris__) 
#    define link_warning(symbol, msg)
#  else
#    define link_warning(symbol, msg)					      \
	asm (".section "  ".gnu.warning." #symbol  "\n\t.previous");	      \
	    static const char __evoke_link_warning_##symbol[]		      \
	    __attribute__ ((unused, section (".gnu.warning." #symbol "\n\t#"))) = msg;
#endif
#else /* !defined __HAVE_ELF__ */
#  define strong_alias(name, aliasname) _strong_alias (name, aliasname)
#  define weak_alias(name, aliasname) _strong_alias (name, aliasname)
#  define _strong_alias(name, aliasname) \
	__asm__(".global " __C_SYMBOL_PREFIX__ #aliasname "\n" \
                ".set " __C_SYMBOL_PREFIX__ #aliasname "," __C_SYMBOL_PREFIX__ #name);
#  define link_warning(symbol, msg) \
	asm (".stabs \"" msg "\",30,0,0,0\n\t" \
	      ".stabs \"" #symbol "\",1,0,0,0\n");
#endif

#ifndef weak_function
/* If we do not have the __attribute__ ((weak)) syntax, there is no way we
can define functions as weak symbols.  The compiler will emit a `.globl'
directive for the function symbol, and a `.weak' directive in addition
will produce an error from the assembler.  */
# define weak_function          /* empty */
# define weak_const_function    /* empty */
#endif

/* On some platforms we can make internal function calls (i.e., calls of
functions not exported) a bit faster by using a different calling
convention.  */
#ifndef internal_function
# define internal_function      /* empty */
#endif

/* Prepare for the case that `__builtin_expect' is not available.  */
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif
#ifndef likely
# define likely(x)	__builtin_expect((!!(x)),1)
#endif
#ifndef unlikely
# define unlikely(x)	__builtin_expect((!!(x)),0)
#endif
#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H
#endif
#ifndef __cast__
#define __cast__(_to)
#endif

/* Arrange to hide uClibc internals */
#if __GNUC_PREREQ (3, 3)
# define attribute_hidden __attribute__ ((visibility ("hidden")))
#else
# define attribute_hidden
#endif

/* Pull in things like __attribute_used__ */
#include <sys/cdefs.h>

/* --- this is added to integrate linuxthreads */
#define __USE_UNIX98            1

#endif /* _LIBC only stuff */

#ifndef __linux__
# define __linux__ 1
#endif

/* Disable __user, which shows up in 2.6.x include asm headers
* that get pulled in by signal.h */
#define __user

#endif	/* features.h  */