#include <stdbool.h>

#include <purpl/purpl.h>

int main(int argc, char *argv[])
{
	ubyte log_index;
	FILE *fp;
	struct purpl_mapping *map;
	struct purpl_logger *logger;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, INFO, DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}

	/* Write a message */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1, "test");

	/* Open a file */
	fp = fopen("a file", PURPL_READ);
	if (!fp)
		purpl_write_log(logger, FILENAME, __LINE__, -1, ERROR,
				"Error opening file: %s", strerror(errno));

	/* Map the file */
	map = purpl_map_file(0, fp);
	if (!map)
		purpl_write_log(logger, FILENAME, __LINE__, -1, ERROR,
				"Error mapping file: %s\n", strerror(errno));

	/* Write the mapped file to the log */
	purpl_write_log(logger, FILENAME, __LINE__, -1, INFO,
			"Mapped contents of file \"a file\":\n%s", map->data);

	/* Unmap a file */
	purpl_unmap_file(map);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
