/**
 * @file types.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Typedefs for convenience
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
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
