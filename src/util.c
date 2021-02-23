#define STB_SPRINTF_IMPLEMENTATION
#include "purpl/util.h"

char *PURPL_EXPORT purpl_fmt_text_va(size_t *len_ret, const char *fmt,
				     va_list args)
{
	size_t len;
	char *buf;
	va_list ap;

	PURPL_RESET_ERRNO;

	/* Check our parameters */
	if (!len_ret || !fmt || !args) {
		errno = EINVAL;
		return fmt;
	}

	/* Copy the arglist */
	va_copy(ap, args);

	/* Determine how large the buffer has to be */
	len = stbsp_vsnprintf(NULL, 0, fmt, args) + 1;
	if (!len) {
		errno = E2BIG;
		len = PURPL_LARGE_BUF;
	}

	/* Now we know how big the buffer will be */
	buf = PURPL_CALLOC(len, char);
	if (!buf) {
		errno = ENOMEM;
		len = -1;
		return fmt; /* This function is used a lot and if it can't "fail" that's good */
	}

	/* Now put the string in the buffer and return */
	stbsp_vsnprintf(buf, len, fmt, ap);
	if (!buf) {
		errno = E2BIG;
		len = -1;
		return fmt;
	}
	*len_ret = len;

	PURPL_RESET_ERRNO;
	return buf;
}

char *PURPL_EXPORT purpl_fmt_text(size_t *len_ret, const char *fmt, ...)
{
	va_list args;
	char *fmt_ptr;

	PURPL_RESET_ERRNO;

	/* Check everything */
	if (!len_ret || !fmt) {
		errno = EINVAL;
		return fmt;
	}

	va_start(args, fmt);
	fmt_ptr = purpl_fmt_text_va(len_ret, fmt, args);
	va_end(args);

	PURPL_RESET_ERRNO;

	return fmt_ptr;
}

char *PURPL_EXPORT purpl_map_file(size_t *len_ret, FILE *fp)
{
	char *map;
	size_t len;
	size_t fpos;
	int fd;

	PURPL_RESET_ERRNO;

	/* Check arguments */
	if (!len_ret || !fp) {
		errno = EINVAL;
		return NULL;
	}

	/* Figure out how long the file is */
	fpos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	/*
	 * Why in K&R's great creation does Windows
	 *  even _have_ file descriptors _and_ handles?!
	 */
	fd = fileno(fp);

#ifdef _WIN32
	
#else /* Everything else is POSIX and therefore sane and logical */
#endif /* _WIN32 */
}
