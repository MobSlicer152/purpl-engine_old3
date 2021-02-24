#include <stdbool.h>

#include <purpl/purpl.h>

int main(int argc, char *argv[])
{
	ubyte log_index;
	FILE *fp;
	char *a_file;
	size_t len;
	int err; /* Save errno before logging an error message */
	struct purpl_mapping *map;
	struct purpl_logger *logger;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, INFO, DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error opening log file: %s\n",
			strerror(errno));
		return -1;
	}

	/* Log a message */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1, "a message");

	/* Open a file */
	fp = fopen("a file", PURPL_READ);
	if (!fp) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, FATAL,
				"Error opening file: %s", strerror(errno));
		return err;
	}

	/* Read a file (use the standard library for really big files) */
	a_file = purpl_read_file(&len, &map, true, "a file");
	if (!a_file) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, FATAL,
				"Error %s file: %s\n",
				(map == NULL) ? "reading" : "mapping",
				strerror(errno));
		purpl_end_logger(logger, true);
		return err;
	}

	/* Write a file's contents to the log */
	purpl_write_log(logger, FILENAME, __LINE__, -1, INFO,
			"Contents of file \"a file\":\n%s", a_file);

	/* Remove the file from memory */
	if (map != NULL)
		purpl_unmap_file(map);
	else
		free(a_file);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
