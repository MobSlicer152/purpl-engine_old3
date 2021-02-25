#include <stdbool.h>

#include <purpl/purpl.h>

int main(int argc, char *argv[])
{
	ubyte log_index;
	int err; /* Save errno before logging an error message */
	struct purpl_logger *logger;
	struct purpl_asset *test;

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

	/*
	 * Load an asset (this is an example, replace it
	 *  with something else to test it)
	 */
	test = purpl_load_asset_from_file(".local/etc:/usr/local/etc:/etc",
					  true, "hosts");
	if (!test) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, FATAL,
				"Failed to load asset: %s", strerror(err));
		purpl_end_logger(logger, true);
		return -err;
	}

	/* Write the file's contents into the log */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1,
			"Contents of \"%s\":\n%s", test->name, test->data);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
