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

/* This is called every frame (think a Win32 window procedure of sorts) */
void frame(struct purpl_inst *inst, SDL_Event e, uint delta, void *user);

int main(int argc, char *argv[])
{
	int err;
	char *test_name;
	struct purpl_asset *test;
	struct purpl_inst *inst;
	bool have_ast = false;
	double runtime;
	char runtime_s[8];
#ifndef NDEBUG
	char *log_path;
	char *log_cont;
	size_t log_len;
	struct purpl_mapping *log_map;
	bool log_mapped = true;
#endif

	/* Unused parameters */
	NOPE(argc);
	NOPE(argv);

	/* Create an instance */
	inst = purpl_create_inst(true, true, embed_start, embed_end,
				 "app" APP_INFO_POSTFIX ".json");
	if (!inst) {
		fprintf(stderr, "Error: failed to create instance: %s\n",
			strerror(errno));
		return errno;
	}

	/* Log the details of the app info */
	purpl_write_log(
		inst->logger, __FILENAME__, __LINE__, -1,
		-1, /* -1 means the default index/level */
		"Contents of loaded app info:\nApp name: %s\nLog path: %s\n"
		"Version: %d.%d\nSearch paths: %s",
		inst->info->name, inst->info->log, inst->info->ver_maj,
		inst->info->ver_min, inst->info->search_paths);

	/* Load an asset */
	test_name = purpl_inst_load_asset_from_file(inst, true, "test.txt");
	if (!test_name) {
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1,
				PURPL_ERROR, "Error: failed to load asset: %s",
				strerror(errno));
		have_ast = false;
	} else {
		have_ast = true;
	}

	if (have_ast) {
		/* Put the asset's contents in the log */
		test = stbds_shget(inst->assets, test_name);
		if (!test) {
			purpl_write_log(
				inst->logger, __FILENAME__, __LINE__, -1,
				PURPL_FATAL,
				"Error: failed to get asset from list: %s",
				strerror(errno));
			free(test_name);
			purpl_end_inst(inst);
			return errno;
		}
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1, -1,
				"Contents of \"%s\":\n%s", test->name,
				test->data);
	}

	/* Create a window (yay it took so long to get here) */
	err = purpl_inst_create_window(inst, false, 800, 600, "%s, version %d.%d",
				       inst->info->name, inst->info->ver_maj,
				       inst->info->ver_min);
	if (err) {
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1,
				PURPL_FATAL,
				"Error: failed to create window: %s\n",
				strerror(errno));
		free(test_name);
		purpl_end_inst(inst);
		return errno;
	}

	/* Initialize graphics */
	err = purpl_inst_init_graphics(inst);
	if (err) {
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1,
				PURPL_FATAL,
				"Error: failed to initialize graphics: %s\n",
				strerror(errno));
		free(test_name);
		purpl_end_inst(inst);
		return errno;
	}

	/* Run the main game loop */
	runtime = purpl_inst_run(inst, NULL, frame) / 1000.0;
	strcpy(runtime_s, "seconds");
	if (runtime >= 60) {
		strcpy(runtime_s, "minutes");
		runtime /= 60;
	}
	if (runtime >= 60) {
		strcpy(runtime_s, "hours");
		runtime /= 60;
	}

	/* Log the amount of time we ran for */
	purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1, -1,
			"Total runtime: %0.3lf %s", runtime, runtime_s);

#ifndef NDEBUG
	/* Save our log's filename */
	log_path = PURPL_CALLOC(strlen(inst->info->log) + 1, char);
	if (!log_path) {
		fprintf(stderr, "Error: failed to save log path: %s\n",
			strerror(errno));
		return errno;
	}
	strcpy(log_path, inst->info->log);
#endif

	/* Close the instance */
	purpl_end_inst(inst);

#ifndef NDEBUG
	/* Read the contents of the log */
	log_cont = purpl_read_file(&log_len, &log_map, &log_mapped, "%s",
				   log_path);
	if (!log_cont) {
		fprintf(stderr, "Error: failed to read file: %s\n",
			strerror(errno));
		return errno;
	}

	/* Print the log's contents */
	printf("Log contents:\n%s", log_cont);

	/* Free stuff */
	free(log_path);
	(log_mapped) ? purpl_unmap_file(log_map) : free(log_cont);

#ifdef _WIN32
	/* Pause */
	printf("\n(Press Enter to exit)");
	NOPE(getc(stdin));
#endif
#endif

	return 0;
}

void frame(struct purpl_inst *inst, SDL_Event e, uint delta, void *user)
{
	SDL_Rect rect;
	static bool yellow = true;
	static uint total = 0;

	NOPE(e);
	NOPE(user);

	/* Fill out a rectangle */
	SDL_GetWindowSize(inst->wnd, &rect.w, &rect.h);
	rect.w /= 2;
	rect.h /= 2;
	SDL_GetWindowSize(inst->wnd, &rect.x, &rect.y);
	rect.x /= 2;
	rect.y /= 2;
	rect.x -= (rect.w / 2);
	rect.y -= (rect.h / 2);

	/* Determine the rectangle's color */
	total += delta;
	if (total >= 1000) {
		total = 0;
		yellow = !yellow;
	}
}
