#ifndef __MAMEDEFS_H__
#define __MAMEDEFS_H__

#include <stddef.h>	// for NULL
#include <stdint.h>

/*
 * Copied from mame/src/osd/osdcomm.h
 * license:BSD-3-Clause
 * copyright-holders:Aaron Giles
 */
/* 8-bit values */
typedef unsigned char                       UINT8;
typedef signed char                         INT8;

/* 16-bit values */
typedef unsigned short                      UINT16;
typedef signed short                        INT16;

/* 32-bit values */
#ifndef _WINDOWS_H
typedef unsigned int                        UINT32;
typedef signed int                          INT32;
#endif

/* 64-bit values */
#ifndef _WINDOWS_H
#ifdef _MSC_VER
typedef signed __int64                      INT64;
typedef unsigned __int64                    UINT64;
#else
__extension__ typedef unsigned long long    UINT64;
__extension__ typedef signed long long      INT64;
#endif
#endif

/*
 * Copied from mame/src/emu/memory.h
 * license:BSD-3-Clause
 * copyright-holders:Aaron Giles
 */
/* offsets and addresses are 32-bit (for now...) */
typedef UINT32	offs_t;

/*
 * Copied from mame/src/emu/emucore.h
 * license:BSD-3-Clause
 * copyright-holders:Aaron Giles
 */
/* stream_sample_t is used to represent a single sample in a sound stream */
typedef INT32 stream_sample_t;

// M_PI is not part of the C/C++ standards and is not present on
// strict ANSI compilers or when compiling under GCC with -ansi
#ifndef M_PI
#define M_PI                            3.14159265358979323846
#endif

/*************************************************/

typedef void (*DEVCB_SRATE_CHG)(void* info, UINT32 newSRate);

#ifndef INLINE
#if defined(_MSC_VER)
#define INLINE static __inline
#elif defined(__GNUC__)
#define INLINE static __inline__
#else
#define INLINE static inline
#endif
#endif

#endif	/* __MAMEDEFS_H__ */
