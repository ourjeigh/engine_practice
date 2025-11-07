#ifndef __TYPES_H__
#define __TYPES_H__
#pragma once

typedef wchar_t wchar;
#define WIDE(x) L##x

typedef short int16;
typedef long int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef float real32;
typedef double real64;

const uint8 k_uint8_max = 0xFF;

#endif //__TYPES_H__