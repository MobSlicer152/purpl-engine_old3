#include "purpl/util.h"

/* stb's versions of the sprintf functions are better and don't use fmemopen */
#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf.h>

char *purpl_fmt_text_va(const size_t *len_ret, const char *fmt, va_list args)
{
	size_t len;
	char *buf;

	/* Check our parameters */
	if (!len || !fmt || !args) {
		errno = EINVAL;
		return NULL;
	}
}
