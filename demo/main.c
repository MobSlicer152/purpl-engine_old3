#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <purpl/purpl.h>

/* App info name postfix */
#ifdef __linux__
#define APP_INFO_POSTFIX "_linux"
#elif _WIN32
#define APP_INFO_POSTFIX "_win32"
#endif

/* Symbols from embedded file */
extern char embed_start[];
extern char embed_end[];
extern size_t embed_size;

int main(int argc, char *argv[])
{
	char *test_name;
	struct purpl_asset *test;
	struct purpl_inst *inst;

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Create an instance */
	inst = purpl_create_inst(true, true, embed_start, embed_end, "app" APP_INFO_POSTFIX ".json");
	if (!inst) {
		fprintf(stderr, "Error: failed to create instance: %s\n", strerror(errno));
		return errno;
	}

	/* Log the details of the app info */
	purpl_write_log(
		inst->logger, FILENAME, __LINE__, -1, -1, /* -1 means the default index/level */
		"Contents of loaded app info:\nApp name: %s\nLog path: %s\n"
		"Version: %d.%d\nSearch paths: %s",
		inst->info->name, inst->info->log, inst->info->ver_maj, inst->info->ver_min,
		inst->info->search_paths);

	/* Load an asset */
	test_name = purpl_inst_load_asset_from_file(inst, true, "test.txt");
	if (!test_name) {
		purpl_write_log(inst->logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Error: failed to load asset: %s", strerror(errno));
		free(test_name);
		purpl_end_inst(inst);
		return errno;
	}

	/* Put the asset's contents in the log */
	test = stbds_shget(inst->assets, test_name);
	if (!test) {
		purpl_write_log(inst->logger, FILENAME, __LINE__, -1, PURPL_FATAL,
				"Error: failed to get asset from list: %s", strerror(errno));
		free(test_name);
		purpl_end_inst(inst);
		return errno;
	}
	purpl_write_log(inst->logger, FILENAME, __LINE__, -1, -1,
			"Contents of \"%s\":\n%s", test->name, test->data);

	/* Close the instance */
	purpl_end_inst(inst);

	return 0;
}
