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
			purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1,
					PURPL_FATAL,
					"Error: failed to get asset from list: %s",
					strerror(errno));
			free(test_name);
			purpl_end_inst(inst);
			return errno;
		}
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1, -1,
				"Contents of \"%s\":\n%s", test->name, test->data);
	}

	/* Create a window (yay it took so long to get here) */
	err = purpl_inst_create_window(inst, true, -1, -1, "%s, version %d.%d",
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

	/* Close the instance */
	purpl_end_inst(inst);

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
		if (yellow)
			yellow = false;
		else
			yellow = true;
	}
	if (yellow)
		SDL_SetRenderDrawColor(inst->renderer, 0xFF, 0xFF, 0x0, 0xFF);
	else
		SDL_SetRenderDrawColor(inst->renderer, 0xCF, 0x0, 0xFF, 0xFF);

	/* Fill the rectangle */
	SDL_RenderFillRect(inst->renderer, &rect);
}
