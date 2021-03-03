/**
 * @file types.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Typedefs for convenience
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 */

#pragma once

#ifndef PURPL_TYPES_H
#define PURPL_TYPES_H 1

typedef char byte;
typedef unsigned char ubyte;

typedef unsigned char uchar;
typedef unsigned int uint;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long s64;
#ifdef __GNUC__
#define PURPL_U128_SUPPORTED
typedef __int128 s128;
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
#ifdef __GNUC__
#define PURPL_U128_SUPPORTED
typedef unsigned __int128 u128;
#endif

#endif /* !PURPL_TYPES_H */
