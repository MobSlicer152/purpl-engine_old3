#include "purpl/asset.h"

struct purpl_asset *PURPL_EXPORT purpl_load_asset_from_file(
	const char *search_paths, bool map, const char *name, ...)
{
	struct purpl_asset *asset;
	FILE *fp;
	va_list args;
	char *name_fmt;
	size_t name_len;
	char *full_name;
	size_t full_name_len;
	char **paths;
	char *tmp;
	char *paths_full;
	u8 path_count;
	u8 i;

	PURPL_RESET_ERRNO;

	/* Check args */
	if (!search_paths || !name) {
		errno = EINVAL;
		return NULL;
	}

	/* Format our filename */
	va_start(args, name);
	name_fmt = purpl_fmt_text_va(&name_len, name, args);
	va_end(args);

	/* Make our own copy of search_paths */
	paths_full = PURPL_CALLOC(strlen(search_paths) + 1, char);
	if (!paths_full)
		return NULL;
	strcpy(paths_full, search_paths);

	/* Figure out how many paths there are */
	tmp = strtok(paths_full, ":");
	if (!tmp) { /* Blame the user, they probably messed up */
		errno = EINVAL;
		return NULL;
	}
	for (path_count = 1; path_count < PURPL_MAX_PATHS && strtok(NULL, ":");
	     path_count++)
		;

	/* Now allocate the array of pointers we need */
	paths = PURPL_CALLOC(path_count, char *);
	if (!paths)
		return NULL;

	/* The first round of tokenization fucks up paths_full, rebuild it */
	strcpy(paths_full, search_paths);

	/* Separate the paths into individual pointers */
	tmp = strtok(paths_full, ":");
	paths[0] = PURPL_CALLOC(strlen(tmp), char);
	if (!paths[0])
		return NULL;
	strcpy(paths[0], tmp);
	for (i = 1; i < path_count; i++) {
		tmp = strtok(NULL, ":");
		paths[i] = PURPL_CALLOC(strlen(tmp), char);
		if (!paths[i])
			return NULL;
		strcpy(paths[i], tmp);
	}

	/* Find the path to the asset */
	for (i = 0; i < path_count; i++) {
		/* Concatenate the path */
		full_name = purpl_fmt_text(&full_name_len, "%s/%s", paths[i],
					   name_fmt);

		/* Now try opening it, if it succeeds, we're done here */
		fp = fopen(full_name, "rb");
		if (!fp) {
			/* Free the path and continue if we failed */
			(full_name_len > 0) ? (NULL) : free(full_name);
			continue;
		}
	}

	/* Check if the file still isn't open */
	if (!fp)
		return NULL;

	/* Free some shit */
	(name_len > 0) ? (NULL) : free(name_fmt);
	for (i = 0; i < path_count; i++) {
		if (paths[i])
			free(paths[i]);
	}
	free(paths);

	/* Now we can finally allocate our structure */
	asset = PURPL_CALLOC(1, struct purpl_asset);
	if (!asset) {
		(full_name_len > 0) ? (NULL) : free(full_name);
		return NULL;
	}

	/* Fill in the structure */
	asset->name = PURPL_CALLOC(strlen(full_name), char);
	if (!asset->name) {
		(full_name_len > 0) ? (NULL) : free(full_name);
		return NULL;
	}
	strcpy(asset->name, full_name);
	asset->data =
		purpl_read_file_fp(&asset->size, &asset->mapping, map, fp);
	if (!asset->data)
		return NULL;
	asset->mapped = map;

	/* Close the file */
	fclose(fp);

	/* Return the asset */
	return asset;
}
