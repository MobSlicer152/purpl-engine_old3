#include "purpl/purpl.h"

struct purpl_inst *purpl_create_inst(bool allow_external_app_info, bool start_log,
			      char *embed_start, char *embed_end,
			      char *app_info_path, ...)
{
	struct purpl_inst *inst;
	bool external;
	bool have_embed;
	va_list args;
	char *path;
	size_t path_len;
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
		(path_len) ? (void)0 : free(path);
		return NULL;
	}

	/* If we have one, load the embed */
	if (have_embed) {
		inst->embed = purpl_load_embed(embed_start, embed_end);
		if (!inst->embed) {
			(path_len) ? (void)0 : free(path);
			free(inst);
			return NULL;
		}
	}

	/* Load the app info */
	inst->info = purpl_load_app_info(inst->embed, external, "%s", path);
	if (!inst->info) {
		(path_len) ? (void)0 : free(path);
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
			(path_len) ? (void)0 : free(path);
			purpl_free_app_info(inst->info);
			purpl_free_embed(inst->embed);
			free(inst);
			return NULL;
		}

		/* State that the logger has started */
		purpl_write_log(inst->logger, FILENAME, __LINE__, -1, -1,
				"Logger started");
	}

	PURPL_RESTORE_ERRNO(___errno);

	/* Return the instance */
	return inst;
}

const char *purpl_inst_load_asset_from_file(struct purpl_inst *inst, bool map, const char *name, ...)
{
	va_list args;
	char *path;
	size_t path_len;
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
	ast = purpl_load_asset_from_file(inst->info->search_paths, map, "%s", path);
	if (!ast)
		return NULL;

	/* Make a new buffer and copy in the name */
	tmp = PURPL_CALLOC(strlen(ast->name), char);
	if (!tmp)
		return NULL;
	strcpy(tmp, ast->name);

	/* Append the asset to the list */
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
	SDL_DestroyWindow(inst->wnd);

	/* Free the structure */
	free(inst);

	PURPL_RESTORE_ERRNO(___errno);
}
