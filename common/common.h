#ifndef PMD_COMMON_H
#define PMD_COMMON_H

#include "chip_defs.h"

typedef unsigned int	uint;


#ifdef _WIN32
	#include <tchar.h>
	
	#if _MSC_VER >= 1600 || __GNUC__
	
		#include <stdint.h>
	
	#else
	
		typedef unsigned char	uint8_t;
		typedef unsigned short	uint16_t;
		typedef unsigned int	uint32_t;
		typedef unsigned long	uint64_t;
		typedef signed char		int8_t;
		typedef signed short	int16_t;
		typedef signed int		int32_t;
		typedef signed long		int64_t;
	
	#endif
	

#else
	#include <stdio.h>
	#include <stdint.h>
	#include <limits.h>
	
	#define _T(x)			x
	typedef char			TCHAR;
	
	#define	_MAX_PATH	((PATH_MAX) > ((FILENAME_MAX)*4) ? (PATH_MAX) : ((FILENAME_MAX)*4))
	#define	_MAX_DIR	FILENAME_MAX
	#define	_MAX_FNAME	FILENAME_MAX
	#define	_MAX_EXT	FILENAME_MAX
	
#endif

typedef struct stereosampletag
{
	FM::Sample left;
	FM::Sample right;
} StereoSample;

#pragma pack( push, enter_include1 )
#pragma pack(2)

typedef struct stereo16bittag
{
	short left;
	short right;
} Stereo16bit;

#pragma pack( pop, enter_include1 )

#endif	// PMD_COMMON_H
