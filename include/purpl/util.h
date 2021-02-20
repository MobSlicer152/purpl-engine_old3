/**
 * @file util.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Assorted utility functions
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 */

#pragma once

#ifndef PURPL_UTIL_H
#define PURPL_UTIL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* These are necessary for mapping files */
#if __linux__ || __APPLE__
#include <sys/mman.h>
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
#include <windows.h>
#endif

#include <stb_sprintf.h>

#include "types.h"

/**
 * @brief Gets the size of an array
 */
#define PURPL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief Concatenates two values together (and generates a warning 
 *  sometimes so ignore it if you get the right value)
 */
#define PURPL_CONCAT(hi, lo, type, target) \
	((target)((hi) << (sizeof(type) << 3) | (type)(lo)))

/**
 * @brief Gets the higher half of a value
 */
#define PURPL_HIGH(val, target) ((target)((val) >> (sizeof(val) << 3)))

/**
 * @brief Gets the lower half of a value
 */
#define PURPL_LOW(val, target) \
	((target)((val) & ((1 << (sizeof(val) << 3)) - 1)))

/**
 * @brief 10% more convenient `calloc` for arrays
 */
#define PURPL_CALLOC(count, type) calloc(count, sizeof(type));

/**
 * @brief Resets `errno`
 */
#define PURPL_RESET_ERRNO (errno = 0)

/**
 * @brief The base name and extension of the current file
 */
#if _WIN32 && _MSC_VER /*
	          * MSVC is the only (supported) Windows compiler that
		  * uses backslashes in __FILE__
		  */
#define FILENAME \
	(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define FILENAME \
	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/**
 * @brief Exports a function
 */
#if _WIN32 && _MSC_VER
#define PURPL_EXPORT __declspec(dllexport)
#else
#define PURPL_EXPORT
#endif

/**
 * @brief The specified parameter is unused
 */
#define NOPE(param) (void)(param)

/**
 * @brief To be used for large buffers
 */
#define PURPL_LARGE_BUF 1024

#define PURPL_READ "rb"
#define PURPL_WRITE "rb+"
#define PURPL_OVERWRITE "wb+"
#define PURPL_APPEND "ab+"

/**
 * @brief Formats text as `vsprintf` would
 * 
 * @param len_ret will receive the length of the buffer (if it's -1, returns `fmt`)
 * @param fmt is the `printf`-style format string to be formatted
 * @param args is the variable argument structure that would be given to 
 *  vsprintf`
 * 
 * @return Returns either a buffer containing the formatted string, or, in the
 *  event that that doesn't work, the value of `fmt`
 * 
 * This function can be used as a simpler way of formatting text. It returns
 *  a buffer of either the necessary length for the formatted string or a
 *  large value that should be good enough as a fallback. The buffer can and
 *  should be freed with `free`.
 */
extern char *purpl_fmt_text_va(size_t *len_ret, const char *fmt, va_list args);

/**
 * @brief Formats text as `sprintf` would
 * 
 * @param len_ret will receive the length of the buffer (if it's -1, returns `fmt`)
 * @param fmt is the `printf`-style format string to be formatted
 * 
 * @return Returns either a buffer containing the formatted string, or, in the
 *  event that that doesn't work, the value of `fmt`
 * 
 * This function is a convenient way of formatting text. It gives you a buffer
 *  large enough for the formatted message, which is more convenient than
 *  `sprintf`. Always `free` the buffer.
 */
extern char *purpl_fmt_text(size_t *len_ret, const char *fmt, ...);

#endif /* !PURPL_UTIL_H */
