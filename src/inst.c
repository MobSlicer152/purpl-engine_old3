#include "purpl/purpl.h"

struct purpl_inst *purpl_create_inst(bool allow_external_app_info,
				     bool start_log, char *embed_start,
				     char *embed_end, char *app_info_path, ...)
{
	struct purpl_inst *inst;
	bool external;
	bool have_embed;
	va_list args;
	char *path;
	s64 path_len;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Determine what has to happen */
	have_embed = embed_start;
	external = (allow_external_app_info && have_embed);
	if (!have_embed)
		NOPE(embed_end);

	/* Format the path to the app info */
	if (!app_info_path) {
		/* Allocate a buffer */
		path_len = strlen("app.json");
		path = calloc(path_len, sizeof(char));
		if (!path)
			return NULL;

		/* Copy in the default app info path */
		strcpy(path, "app.json");
	} else {
		/* Format the path normally */
		va_start(args, app_info_path);
		path = purpl_fmt_text_va(&path_len, app_info_path, args);
		va_end(args);
	}

	/* Allocate the structure */
	inst = PURPL_CALLOC(1, struct purpl_inst);
	if (!inst) {
		(path_len > 0) ? (void)0 : free(path);
		return NULL;
	}

	/* If we have one, load the embed */
	if (have_embed) {
		inst->embed = purpl_load_embed(embed_start, embed_end);
		if (!inst->embed) {
			(path_len > 0) ? (void)0 : free(path);
			free(inst);
			return NULL;
		}
	}

	/* Load the app info */
	inst->info = purpl_load_app_info(inst->embed, external, "%s", path);
	if (!inst->info) {
		(path_len > 0) ? (void)0 : free(path);
		purpl_free_embed(inst->embed);
		free(inst);
		return NULL;
	}

	/* Append the app info's asset to our list */
	stbds_shput(inst->assets, inst->info->json->name, inst->info->json);

	/* Start the logger if requested */
	if (start_log) {
		inst->logger = purpl_init_logger(&inst->logindex, PURPL_INFO,
						 PURPL_WARNING, "%s",
						 inst->info->log);
		if (!inst->logger) {
			(path_len > 0) ? (void)0 : free(path);
			purpl_free_app_info(inst->info);
			purpl_free_embed(inst->embed);
			free(inst);
			return NULL;
		}

		/* State that the logger has started */
		purpl_write_log(inst->logger, __FILENAME__, __LINE__, -1, -1,
				"Logger started");
	}

	/* Properly initialize SDL */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

	PURPL_RESTORE_ERRNO(___errno);

	/* Return the instance */
	return inst;
}

int purpl_inst_create_window(struct purpl_inst *inst, bool fullscreen,
			     int width, int height, const char *title, ...)
{
	va_list args;
	int w;
	int h;
	char *title_fmt;
	s64 title_len;
	int err;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check parameters */
	if (!inst || width < -1 || height < -1 || !title) {
		errno = EINVAL;
		return errno;
	}

	/* Avoid orphaning a window's memory */
	if (inst->wnd || inst->renderer) {
		errno = EEXIST;
		return errno;
	}

	/* Format the title */
	va_start(args, title);
	title_fmt = purpl_fmt_text_va(&title_len, title, args);
	va_end(args);

	/* If necessary, get default values for width/height */
	w = (width == -1) ? 1024 : width;
	h = (width == -1) ? 600 : height;

	/* Create a window through SDL */
	err = SDL_CreateWindowAndRenderer(
		w, h,
		SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
			/* This is really simple but feels really big brain */
			((fullscreen) ? SDL_WINDOW_FULLSCREEN : 0),
		&inst->wnd, &inst->renderer);
	if (err < 0) {
		errno = ENOMEM; /* This is typically the cause */
		return errno;
	}

	/* Set our window title */
	SDL_SetWindowTitle(inst->wnd, title_fmt);

	/* Get the window surface */
	inst->surf = SDL_GetWindowSurface(inst->wnd);

	/* Free our title format pointer */
	(title_len < 0) ? (void)0 : free(title_fmt);

	PURPL_RESTORE_ERRNO(___errno);

	return 0;
}

uint purpl_inst_run(struct purpl_inst *inst, void *user,
		    void(frame)(struct purpl_inst *inst, SDL_Event e, uint delta,
				void *user))
{
	uint delta;
	uint beginning;
	uint last;
	uint now;
	SDL_Event e;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check arguments */
	if (!inst || !frame) {
		errno = EINVAL;
		return -1;
	}

	/* Get the time */
	beginning = SDL_GetTicks();

	/* Start the loop */
	inst->running = true;
	last = beginning;
	while (inst->running) {
		/* Process events */
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT)
				inst->running = false;
		}

		/* Clear before drawing anything */
		SDL_SetRenderDrawColor(inst->renderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(inst->renderer);

		/* Get the time */
		now = SDL_GetTicks();

		/* Call the frame function */
		delta = now - last;
		frame(inst, e, delta, user);

		/* Get the time again */
		last = now;
		now = SDL_GetTicks();

		/* Update the window surface */
		SDL_RenderFlush(inst->renderer);
		SDL_RenderPresent(inst->renderer);
	}

	PURPL_RESTORE_ERRNO(___errno);
	
	/* We're done now */
	return now - beginning;
}

const char *purpl_inst_load_asset_from_file(struct purpl_inst *inst, bool map,
					    const char *name, ...)
{
	va_list args;
	char *path;
	s64 path_len;
	struct purpl_asset *ast;
	char *tmp;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check arguments */
	if (!inst || !name) {
		errno = EINVAL;
		return NULL;
	}

	/* Format the path to the asset */
	va_start(args, name);
	path = purpl_fmt_text_va(&path_len, name, args);
	va_end(args);

	/* Get the asset */
	ast = purpl_load_asset_from_file(inst->info->search_paths, map, "%s",
					 path);
	if (!ast) {
		(path_len > 0) ? (void)0 : free(path);
		return NULL;
	}

	/* Make a new buffer and copy in the name */
	tmp = PURPL_CALLOC(strlen(ast->name), char);
	if (!tmp) {
		purpl_free_asset(ast);
		return NULL;
	}
	strcpy(tmp, ast->name);

	/* Append the asset to the lis4t */
	stbds_shput(inst->assets, tmp, ast);

	PURPL_RESTORE_ERRNO(___errno);

	return tmp;
}

void purpl_inst_free_asset(struct purpl_inst *inst, const char *name)
{
	struct purpl_asset *ast;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check the instance */
	if (!inst) {
		errno = EINVAL;
		return;
	}

	/* Get a pointer to the asset */
	ast = stbds_shget(inst->assets, name);

	/* Remove the asset from the list */
	stbds_shdel(inst->assets, name);

	/* Free the asset and its name */
	free(name);
	purpl_free_asset(ast);

	PURPL_RESTORE_ERRNO(___errno);
}

void purpl_inst_destroy_window(struct purpl_inst *inst)
{
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Avoid a segfault */
	if (!inst) {
		errno = EINVAL;
		return;
	}

	/* Destroy the renderer and then the window */
	SDL_DestroyRenderer(inst->renderer);
	SDL_DestroyWindow(inst->wnd);

	PURPL_RESTORE_ERRNO(___errno);
}

void purpl_end_inst(struct purpl_inst *inst)
{
	size_t i;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Avoid messing with a NULL pointer */
	if (!inst) {
		errno = EINVAL;
		return;
	}

	/* Free the structures for the instance */
	purpl_free_app_info(inst->info);
	purpl_free_embed(inst->embed);
	purpl_end_logger(inst->logger, true);

	/* Free all the assets */
	for (i = 0; i < stbds_shlenu(inst->assets); i++)
		purpl_free_asset(inst->assets[i].value);

	/* Get rid of the string hash map */
	stbds_shfree(inst->assets);

	/* Make sure the window is closed */
	purpl_inst_destroy_window(inst);

	/* Free the structure */
	free(inst);

	PURPL_RESTORE_ERRNO(___errno);
}
