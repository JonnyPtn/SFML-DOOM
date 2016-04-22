#pragma once

// Endianess handling.
// WAD files are stored little endian.
#ifdef __BIG_ENDIAN__
short	SwapSHORT(short);
long	SwapLONG(long);
#define SHORT(x)	((short)SwapSHORT((unsigned short) (x)))
#define LONG(x)         ((long)SwapLONG((unsigned long) (x)))
#else
#define SHORT(x)	(x)
#define LONG(x)         (x)
#endif