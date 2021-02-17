#include "purpl/log.h"

struct purpl_logger *PURPL_EXPORT
purpl_init_logger(const ubyte *first_index_ret, byte default_level,
		  byte first_max_level, const char *first_log_path, ...)
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

	/* Allocate the file stream pointers */
	logger->logs = PURPL_CALLOC(64, FILE *);

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

int PURPL_EXPORT purpl_open_log(struct purpl_logger *logger, byte max_level,
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

size_t PURPL_EXPORT purpl_write_log(struct purpl_logger *logger,
				    const char *file, const int line,
				    byte index, byte level, const char *fmt,
				    ...)
{
	char *fmt_ptr;
	char *lvl_pre;
	ubyte idx;
	ubyte lvl;
	size_t len;
	size_t written;
	FILE *fp;
	va_list args;

	PURPL_RESET_ERRNO;

	/* Check our args */
	if (!logger || !file || !line || !fmt) {
		errno = EINVAL;
		return -1;
	}

	/* Format fmt */
	va_start(args, fmt);
	fmt_ptr = purpl_fmt_text_va(&len, fmt, args);
	va_end(args);

	/* Evaluate what level to use */
	lvl = (level < 0) ? logger->default_level : level;
	switch (lvl) {
	case WTF:
		lvl_pre = PURPL_CALLOC(strlen(PRE_WTF) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_WTF);
		break;
	case FATAL:
		lvl_pre = PURPL_CALLOC(strlen(PRE_FATAL) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_FATAL);
		break;
	case ERROR:
		lvl_pre = PURPL_CALLOC(strlen(PRE_ERROR) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_ERROR);
		break;
	case WARNING:
		lvl_pre = PURPL_CALLOC(strlen(PRE_WARNING) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_WARNING);
		break;
	case INFO:
		lvl_pre = PURPL_CALLOC(strlen(PRE_INFO) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_INFO);
		break;
	case DEBUG:
		lvl_pre = PURPL_CALLOC(strlen(PRE_DEBUG) + 1, char);
		if (!lvl_pre) {
			(len < 0) ?: free(fmt_ptr);
			errno = EIO;
			return -1;
		}

		strcpy(lvl_pre, PRE_DEBUG);
		break;
	}

	/* Figure out our index */
	idx = (index < 0) ? logger->default_index : index;
	fp = logger->logs[idx];

	/* Write the message */
	written = fprintf(fp, "%s%s:%d: %s\n", lvl_pre, file, line, fmt_ptr);
	if (!written) {
		free(fmt_ptr);
		free(lvl_pre);
		errno = EIO;
		return -1;
	}

	/* Just in case */
	fflush(fp);

	/* Free msg too */
	free(fmt_ptr);
	free(lvl_pre);

	PURPL_RESET_ERRNO;

	return written;
}

void PURPL_EXPORT purpl_close_log(struct purpl_logger *logger, ubyte index)
{
	PURPL_RESET_ERRNO;

	/* Check our args */
	if (!logger || !logger->logs[index]) {
		errno = EINVAL;
		return;
	}

	/* Close the log and clear its information */
	fclose(logger->logs[index]);
	logger->nlogs--;
	logger->max_level[index] = 0;

	PURPL_RESET_ERRNO;
}

void PURPL_EXPORT purpl_end_logger(struct purpl_logger *logger)
{
	uint i;

	PURPL_RESET_ERRNO;

	/* Check args */
	if (!logger) {
		errno = EINVAL;
		return;
	}

	/* Close every log */
	for (i = 0; i < logger->nlogs; i++)
		purpl_close_log(logger, i);

	/* Free the logger */
	free(logger);

	PURPL_RESET_ERRNO;
}
