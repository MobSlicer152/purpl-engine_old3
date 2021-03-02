#include "purpl/asset.h"

#ifdef __cplusplus
extern "C" {
#endif

struct purpl_embed *purpl_load_embed(const char *sym_start, const char *sym_end)
{
	struct purpl_embed *embed;
	struct zip_source *src;
	struct zip_error *err;

	PURPL_RESET_ERRNO;

	/* Validate arguments */
	if (!sym_start || !sym_end || !(sym_end - sym_start)) {
		errno = EINVAL;
		return NULL;
	}

	/* Allocate the structure */
	embed = PURPL_CALLOC(1, struct purpl_embed);
	if (!embed)
		return NULL;

	/* Store our archive start, end, and size */
	embed->start = sym_start;
	embed->end = sym_end;
	embed->size = embed->end - embed->start;

	/* Start up libzip */
	err = PURPL_CALLOC(1, struct zip_error);
	if (!err)
		return NULL;
	zip_error_init(err);
	src = zip_source_buffer_create(embed->start, embed->size, 1, err);
	if (!src) {
		errno = (err->sys_err) ? err->sys_err : ENOENT;
		zip_error_fini(err);
		return NULL;
	}

	/* Open the archive */
	embed->z = zip_open_from_source(src, ZIP_RDONLY, err);
	if (err->zip_err || err->sys_err) {
		errno = (err->sys_err) ? err->sys_err : ENOENT;
		zip_source_free(src);
		zip_error_fini(err);
		return NULL;
	}

	PURPL_RESET_ERRNO;

	/* Return the embed details */
	return embed;
}

struct purpl_asset *purpl_load_asset_from_archive(struct zip *z,
						  const char *path, ...)
{
	struct purpl_asset *asset;
	struct zip_error *err;
	va_list args;
	char *path_fmt;
	size_t path_len;
	struct zip_file *zfp;
	struct zip_stat stat;
	s64 idx;
	int e;

	PURPL_RESET_ERRNO;

	/* Check args */
	if (!z || !path) {
		errno = EINVAL;
		return NULL;
	}

	/* Format the path to the file */
	va_start(args, path);
	path_fmt = purpl_fmt_text_va(&path_len, path, args);
	va_end(args);

	/* 
	 * Allocate an archive (0 is false and calloc zeroes the 
	 * memory, so no worries about asset->mapped)
	 */
	asset = PURPL_CALLOC(1, struct purpl_asset);
	if (!asset)
		return NULL;

	/* Initialize libzip error info */
	err = PURPL_CALLOC(1, struct zip_error);
	if (!err)
		return NULL;
	zip_error_init(err);

	/* Locate the requested file */
	idx = zip_name_locate(z, path, 0);
	if (idx < 0) {
		err = zip_get_error(z);
		errno = (err->sys_err) ?
				      err->sys_err :
				      ENOENT; /* Fuck custom error systems, errno is standard */
		zip_error_fini(err);
		free(asset);
		return NULL;
	}

	/* Stat the file */
	e = zip_stat_index(z, idx, 0, &stat);
	if (!e || !(stat.valid & ZIP_STAT_NAME) ||
	    !(stat.valid &
	      ZIP_STAT_SIZE)) { /*
				 * If these aren't valid, we don't give a
	      			 *  damn about other fields anyway
				 */
		err = zip_get_error(z);
		errno = (err->sys_err) ? err->sys_err : ENOENT;
	}

	/* Use the values contained in stat */
	asset->name = PURPL_CALLOC(strlen(stat.name), char);
	if (!asset->name)
		return NULL;
	strcpy(asset->name, stat.name);
	asset->size = stat.size;

	/* Now allocate our buffer */
	asset->data = PURPL_CALLOC(asset->size, char);
	if (!asset->data)
		return NULL;

	/* Open the file */
	zfp = zip_fopen_index(z, idx, 0);
	if (!zfp) {
		err = zip_get_error(z);
		errno = (err->sys_err) ? err->sys_err : ENOENT;
		free(asset->name);
		free(asset->data);
		return NULL;
	}

	/* And try to read the data */
	zip_fread(zfp, asset->data, asset->size);
	if (!asset->data) {
		err = zip_get_error(z);
		errno = (err->sys_err) ? err->sys_err : ENOENT;
		free(asset->name);
		free(asset->data);
		return NULL;
	}

	PURPL_RESET_ERRNO;

	/* Return the asset */
	return asset;
}

struct purpl_asset *purpl_load_asset_from_file(const char *search_paths,
					       bool map, const char *name, ...)
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
			(full_name_len > 0) ? (void)0 : free(full_name);
			continue;
		}
	}

	/* Check if the file still isn't open */
	if (!fp)
		return NULL;

	/* Free some shit */
	(name_len > 0) ? (void)0 : free(name_fmt);
	for (i = 0; i < path_count; i++) {
		if (paths[i])
			free(paths[i]);
	}
	free(paths);

	/* Now we can finally allocate our structure */
	asset = PURPL_CALLOC(1, struct purpl_asset);
	if (!asset) {
		(full_name_len > 0) ? (void)0 : free(full_name);
		return NULL;
	}

	/* Fill in the structure */
	asset->name = PURPL_CALLOC(strlen(full_name), char);
	if (!asset->name) {
		(full_name_len > 0) ? (void)0 : free(full_name);
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

	PURPL_RESET_ERRNO;

	/* Return the asset */
	return asset;
}

void purpl_free_asset(struct purpl_asset *asset)
{
	PURPL_RESET_ERRNO;

	/* If the file is mapped, deal with that */
	if (asset->mapped)
		purpl_unmap_file(asset->mapping);
	else /* Otherwise free the data */
		free(asset->data);

	/* Free the rest of the structure */
	free(asset);

	PURPL_RESET_ERRNO;
}

#ifdef __cplusplus
}
#endif
