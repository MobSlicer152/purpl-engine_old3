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
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defined when not Windows, to be expanded  
 */
#if !defined _WIN32 && !defined _MSC_VER && !defined __CYGWIN__
#define PROBABLY_POSIX 1
#endif

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
 * @brief Get the base name of `path`
 */
#if _WIN32 && _MSC_VER /*
	          * MSVC is the only (supported) Windows compiler that
		  * uses backslashes in __FILE__
		  */
#define PURPL_GET_BASENAME(path) \
	(strrchr(path, '\\') ? strrchr(path, '\\') + 1 : path)
#else
#define PURPL_GET_BASENAME(path) \
	(strrchr(path, '/') ? strrchr(path, '/') + 1 : path)
#endif

/**
 * @brief The base name and extension of the current file
 */
#define FILENAME PURPL_GET_BASENAME(__FILE__)

/**
 * @brief The specified parameter is unused
 */
#define NOPE(param) (void)(param)

/**
 * @brief To be used for large buffers
 */
#define PURPL_LARGE_BUF 1024

#ifdef PROBABLY_POSIX
/**
 * @brief glibc's macro for retrying interrupted syscalls (because some people
 *  don't have glibc or something, and this macro is useful)
 */
#define PURPL_RETRY_INTR(expression)                       \
	(__extension__({                                   \
		long int __result;                         \
		do                                         \
			__result = (long int)(expression); \
		while (__result == -1L && errno == EINTR); \
		__result;                                  \
	}))
#endif
#define PURPL_READ "rb" /**< Read only */
#define PURPL_WRITE "rb+" /**< Read _and_ write */
#define PURPL_OVERWRITE "wb+" /**< Overwrite (read and write, but truncated) */
#define PURPL_APPEND "ab+" /**< Append */

/**
 * @brief Holds information about a mapped file
 * 
 * DO NOT USE `data` or `len` DIRECTLY IF THEY MIGHT GET MODIFIED!
 */
struct purpl_mapping {
	void *data; /**< The actual mapped file */
	size_t len; /**< The length of the file */
	u8 prot; /**< The protection of the pages. See `purpl_map_file` for more */
#ifdef _WIN32
	HANDLE handle; /**< Ewwy Win32 handle to the "mapping object" */
#endif
};

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
 *  large value that should be good enough as a fallback.  Always `free` the buffer,
 *  unless len_ret is -1 and `fmt` can't be freed in that way.
 */
extern char *purpl_fmt_text_va(size_t *len_ret, const char *fmt,
					    va_list args);

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
 *  `sprintf`. Always `free` the buffer, unless len_ret is -1 and `fmt`
 *  can't be freed in that way.
 */
extern char *purpl_fmt_text(size_t *len_ret, const char *fmt, ...);

/**
 * @brief Maps a file into the process's virtual memory using the
 *  appropriate system function
 * 
 * @param protection is 0, 1, or 2 to indicate read-only, writable, or
 *  executable.
 * @param fp is the file to map
 * 
 * @return Returns either information about the mapped file (use the `data` member)
 *  or `NULL`. Check `errno` if `NULL` is returned.
 * 
 * This function provides a convenient way to map a file into memory which is
 *  significantly more efficient than reading the file into memory. Seeing as
 *  this process requires either a system call or a Win32 function, it's easier
 *  to use this function instead because it's consistent (or at least tries to
 *  be). DO NOT FREE THE POINTER RETURNED, THAT'S NOT HOW THAT WORKS.
 *  Use `purpl_unmap_file` with THE ORIGINAL ADDRESS RETURNED FROM THIS
 *  FUNCTION OR BAD SHIT (!) WILL HAPPEN. On Linux, you'll get a segfault if
 *  you try writing to read-only mappings,  and as far as I remember, Windows
 *  will get mad too. Check the returned structure's `prot` member for the
 *  actual protection of the pages.
 */
extern struct purpl_mapping *purpl_map_file(u8 protection,
							 FILE *fp);

/**
 * @brief Unmap a file mapped with `purpl_map_file`.
 * 
 * @param mapping is the `purpl_mapping` structure containing information
 *  about the mapping to be unmapped
 * 
 * This function does no error checking because it's assumed that `mapping`
 *  will be unused after this (i.e. will be reassigned or something).
 */
extern void purpl_unmap_file(struct purpl_mapping *mapping);

/**
 * @brief Read a file (either map it into memory or read it into a buffer)
 *
 * @param len_ret is the length of the file (you'll want to keep track of this)
 * @param mapping is optional if `map` is false, otherwise you need it (if `NULL`,
 *  `free` the buffer instead of unmapping)
 * @param map is whether or not to map the file into memory instead of reading
 *  it into a buffer (if this is true _AND_ `mapping` isn't `NULL`, don't free the
 *  buffer, use `purpl_unmap_file`). Also, if this is true, writing to the
 *  buffer _is_ safe, but _only_ if `mapping->prot` is greater than 0
 * @param fp is the file stream to read from/map
 *
 * @return Returns the contents of the file (either a buffer or a pointer to
 *  the pages containing the mapped contents of the file, see `mapping` and `map`)
 */
extern char *purpl_read_file_fp(size_t *len_ret,
					     struct purpl_mapping **mapping,
				bool map, FILE *fp);

/**
 * @brief Read a file from a path
 * 
 * @param len_ret is the length of the file (keep track of it)
 * @param mapping is optional if `map` is false. If it's `NULL`, mapping failed and
 *  you can use `free` on the buffer instead of `purpl_unmap_file`
 * @param map is whether to map the file or read it into a buffer (see the 
 *  description for `mapping`)
 * @param path is the path to the file to read/map
 *
 * @return Returns the contents of the file (either a buffer or a pointer to
 *  the pages containing the mapped contents of the file, see `mapping` and `map`)
 */
extern char *purpl_read_file(size_t *len_ret,
					  struct purpl_mapping **mapping,
			     bool map, const char *path, ...);

#ifdef __cplusplus
}
#endif

#endif /* !PURPL_UTIL_H */
