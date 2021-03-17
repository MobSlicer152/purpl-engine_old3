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
	struct purpl_logger *logger;
	struct purpl_embed *embed;
	struct purpl_app_info *info;
	struct purpl_asset *test;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Initialize a logger */
	logger = purpl_init_logger(&log_index, PURPL_INFO, PURPL_DEBUG,
				   "purpl.log");
	if (!logger) {
		fprintf(stderr, "Error opening log file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Log a message */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1, "a message");

	/* Open up the embedded archive */
	embed = purpl_load_embed(embed_start, embed_end);
	if (!embed) {
		purpl_write_log(logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Failed to load embedded archive: %s",
				strerror(errno));
		purpl_end_logger(logger, true);
		return errno;
	}

	/* Load an app info file */
	info = purpl_load_app_info(embed, true, "app.json");
	if (!info) {
		purpl_write_log(logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Failed to load app info: %s", strerror(errno));
		purpl_end_logger(logger, true);
		return errno;
	}

	/* Log the details of the app info */
	purpl_write_log(
		logger, FILENAME, __LINE__, -1, -1,
		"Contents of loaded app info:\nApp name: %s\nLog path: %s\nVersion: %d.%d\n"
		"Search paths: %s",
		info->name, info->log, info->ver_maj, info->ver_min,
		info->search_paths);

	/* Load an asset using our newly loaded search paths */
	test = purpl_load_asset_from_file(info->search_paths, false,
					  "no u microsoft.txt");
	if (!test) {
		purpl_write_log(logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Error: failed to load asset: %s",
				strerror(errno));
		purpl_end_logger(logger, true);
		return errno;
	}

	/* Write the file contents */
	purpl_write_log(logger, FILENAME, __LINE__, -1, -1,
			"Contents of \"%s\":\n%s", test->name, test->data);

	/* Free all the file structures */
	purpl_free_asset(test);
	purpl_free_app_info(info);
	purpl_free_embed(embed);

	/* Close the logger */
	purpl_end_logger(logger, true);

	return 0;
}
