#include <stdbool.h>

#include <purpl/purpl.h>

int main(int argc, char *argv[])
{
	ubyte log_index;
	FILE *fp;
	int err; /* Save errno before logging an error message */
	struct purpl_mapping *map;
	struct purpl_logger *logger;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, INFO, DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error opening log file: %s\n", strerror(errno));
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

	/* Map a file as read-only */
	map = purpl_map_file(0, fp);
	if (!map) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, FATAL,
				"Error mapping file: %s\n", strerror(errno));
		purpl_end_logger(logger, true);
		return err;
	}

	/* Write the mapped file to the log */
	purpl_write_log(logger, FILENAME, __LINE__, -1, INFO,
			"Mapped contents of file \"a file\":\n%s", map->data);

	/* Unmap a file */
	purpl_unmap_file(map);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
