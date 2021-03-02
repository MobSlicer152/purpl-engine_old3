#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <purpl/purpl.h>

/* Symbols from embedded file */
extern char embed_start[];
extern char embed_end[];
extern size_t embed_size;

int main(int argc, char *argv[])
{
	ubyte log_index;
	int err; /* Save errno before logging an error message */
	struct purpl_logger *logger;
	struct purpl_embed *embed;
	struct purpl_asset *test;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, PURPL_INFO, PURPL_DEBUG, "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error opening log file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Log a message */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1, "a message");

	/* Open up the embedded archive */
	embed = purpl_load_embed(embed_start,
				 embed_end);
	if (!embed) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Failed to load embedded archive: %s",
				strerror(err));
		purpl_end_logger(logger, true);
		return err;
	}

	/* Load an asset from the archive embedded in the executable */
	test = purpl_load_asset_from_archive(embed->z, "test.txt");
	if (!test) {
		err = errno;
		purpl_write_log(logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Failed to load asset: %s", strerror(err));
		purpl_end_logger(logger, true);
		return err;
	}

	/* Write the file's contents into the log */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1,
			"Contents of \"(embedded zip file)/%s\":\n%s", test->name, test->data);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
