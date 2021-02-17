#include "purpl/log.h"

struct purpl_logger *PURPL_EXPORT
purpl_init_logger(const ubyte *first_index_ret, ubyte default_level,
		  ubyte first_max_level, const char *first_log_path, ...)
{
	uint i;
	struct purpl_logger *logger;
	char *first;
	va_list args;
	size_t len;
	ubyte first_index;

	PURPL_RESET_ERRNO;

	/* First, check our parameters */
	if (!first_index_ret || !first_log_path) {
		errno = EINVAL;
		return NULL;
	}

	/* Allocate the structure */
	logger = PURPL_CALLOC(1, struct purpl_logger);
	if (!logger) {
		errno = ENOMEM;
		return NULL;
	}

	/* Format the path to the first log */
	va_start(args, first_log_path);
	first = purpl_fmt_text_va(&len, first_log_path, args);
	va_end(args);

	/* Clear the file pointers */
	memset(logger->logs, 0, PURPL_MAX_LOGS * sizeof(FILE *));

	/* Open the first log and fill out the structure */
	logger->default_index = purpl_open_log(logger, first_max_level, first) &
				0xFFFFFF;
	if (logger->default_index < 0) {
		free(logger);
		return NULL;
	}

	logger->default_level = ((default_level < 0) ? INFO : default_level) &
				0xFFF;

	/* Return the index of the first log\ */
	first_index = logger->default_index;
	memcpy(first_index_ret, &first_index, 1);

	PURPL_RESET_ERRNO;

	return logger;
}

int PURPL_EXPORT purpl_open_log(struct purpl_logger *logger, ubyte max_level,
				const char *path, ...)
{
	ubyte index;
	char *fmt_path;
	size_t len;
	va_list args;

	PURPL_RESET_ERRNO;

	/* Check the parameters */
	if (!logger || !path) {
		errno = EINVAL;
		return -1;
	}

	/* Format the path to the log */
	va_start(args, path);
	fmt_path = purpl_fmt_text_va(&len, path, args);
	va_end(args);

	/* Determine the index */
	index = logger->nlogs & 0xFFFFFF;

	/* Open the log */
	if (strcmp(fmt_path, "stdout") == 0) {
		logger->logs[index] = stdout;
	} else if (strcmp(fmt_path, "stderr") == 0) {
		logger->logs[index] = stderr;
	} else {
		logger->logs[index] = fopen(fmt_path, PURPL_OVERWRITE);
		if (logger->logs[index] == NULL) {
			errno = EIO;
			return -1;
		}
	}

	/* Set the max level for the log */
	logger->max_level[index] = (max_level < 0) ? DEBUG : max_level;

	PURPL_RESET_ERRNO;

	/* Increment the number of logs and return */
	logger->nlogs++;
	return index &
	       0xFFFFFF; /* Gotta mask off any extra for the bit field */
}

/* Some macros for the text of different message levels */
#define PRE_WTF "[???] "
#define PRE_FATAL "[fatal] "
#define PRE_ERROR "[error] "
#define PRE_WARNING "[warning] "
#define PRE_INFO "[info] "
#define PRE_DEBUG "[debug] "

size_t PURPL_EXPORT purpl_write_log(struct purpl_logger *logger, ubyte index,
				    ubyte level, const char *fmt, ...)
{
	char *fmt_ptr;
	char *msg;
	char *lvl_pre;
	ubyte idx;
	ubyte lvl;
	size_t orig_len;
	size_t len;
	size_t written;
	FILE *fp;
	va_list args;

	PURPL_RESET_ERRNO;

	/* Check our args */
	if (!logger || !fmt) {
		errno = EINVAL;
		return -1;
	}

	/* Format fmt */
	va_start(args, fmt);
	fmt_ptr = purpl_fmt_text_va(&orig_len, fmt, args);
	va_end(args);
	len = ((orig_len < 0) ? strlen(fmt_ptr) : orig_len) + strlen(FILENAME) +
	      2; /* For calloc and fwrite */

	/* Evaluate what level to use */
	lvl = (level < 0) ? logger->default_level : level;
	switch (lvl) {
	case WTF:
		lvl_pre = PURPL_CALLOC(strlen(PRE_WTF) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_WTF);
		break;
	case FATAL:
		lvl_pre = PURPL_CALLOC(strlen(PRE_FATAL) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_FATAL);
		break;
	case ERROR:
		lvl_pre = PURPL_CALLOC(strlen(PRE_ERROR) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_ERROR);
		break;
	case WARNING:
		lvl_pre = PURPL_CALLOC(strlen(PRE_WARNING) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_WARNING);
		break;
	case INFO:
		lvl_pre = PURPL_CALLOC(strlen(PRE_INFO) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_INFO);
		break;
	case DEBUG:
		lvl_pre = PURPL_CALLOC(strlen(PRE_DEBUG) + 1, char);
		if (!lvl_pre) {
			(orig_len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_DEBUG);
		break;
	}

	/* Figure out our index */
	idx = index & 0xFFFFFF;
	idx = (idx < 0) ? logger->default_index : idx;
	fp = logger->logs[idx];

	/* Put together the message */
	msg = PURPL_CALLOC(len, char);
	if (!msg) {
		free(lvl_pre);
		(orig_len < 0) ?: free(fmt_ptr);
		errno = ENOMEM;
		return -1;
	}

	stbsp_sprintf(msg, "%s %s %s\n", lvl_pre, FILENAME, fmt_ptr);

	/* Free everything except msg as they're no longer needed */
	free(fmt_ptr);
	free(lvl_pre);

	/* Write the message */
	written = fwrite(msg, len, len, fp);
	if (!written) {
		free(msg);
		errno = EIO;
		return -1;
	}

	/* Free message too */
	free(msg);

	PURPL_RESET_ERRNO;

	return written;
}
