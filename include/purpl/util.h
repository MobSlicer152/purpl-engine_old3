#pragma once

#ifndef PURPL_UTIL_H
#define PURPL_UTIL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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

/**
 * @brief Formats text as vsprintf would
 * 
 * @param len_ret will recieve the length of the buffer
 * @param fmt is the `printf`-style format string to be formatted
 * @param args is the variable argument structure that would be given to `vsprintf`
 * 
 * @returns a buffer large enough to hold the formatted string containing that string
 * 
 * This function returns a buffer of either the necessary length for the formatted string
 * or a large value that should be good enough as a fallback. The buffer can and should
 * be freed with `free`.
 */
char *purpl_fmt_text_va(const size_t *len, const char *fmt, va_list args);

#endif /* !PURPL_UTIL_H */
