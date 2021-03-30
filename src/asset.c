#include "purpl/asset.h"

#ifdef __cplusplus
extern "C" {
#endif

struct purpl_embed *purpl_load_embed(const char *sym_start, const char *sym_end)
{
	struct purpl_embed *embed;
	int err;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

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

	/* Start up libarchive */
	embed->ar = archive_read_new();
	if (!embed->ar)
		return NULL;

	/* Enable support for tar archives because they're good */
	archive_read_support_format_all(embed->ar);
	archive_read_support_filter_all(embed->ar);

	/* Load in the archive */
	err = archive_read_open_memory(embed->ar, embed->start, embed->size);
	if (err != ARCHIVE_OK) {
		errno = ENOMEM; /*
				 * Some interpretation will be necessary in
				 * libarchive-related code
				 */
		return NULL;
	}

	PURPL_RESTORE_ERRNO(___errno);

	/* Return the embed details */
	return embed;
}

struct purpl_asset *purpl_load_asset_from_archive(struct archive *ar,
						  const char *path, ...)
{
	struct purpl_asset *asset;
	struct archive_entry *ent;
	int err;
	va_list args;
	char *path_fmt;
	s64 path_len;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check args */
	if (!ar || !path) {
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

	/* Iterate through archive entries until we find the right file */
	while (1) {
		/* Keep reading the next header */
		err = archive_read_next_header(ar, &ent);
		if (err == ARCHIVE_EOF) {
			errno = ENOENT;
			return NULL;
		}

		/* First, check for some other error */
		if (err != ARCHIVE_OK)
			continue;

		/* Next, check for a match */
		if (strcmp(path_fmt, archive_entry_pathname(ent)) != 0)
			continue;

		/* Now that we have a match, check if it's a directory */
		if (archive_entry_filetype(ent) == AE_IFDIR) {
			errno = EISDIR;
			return NULL;
		}

		/* At this point, we can read the file, so get its length */
		asset->size = archive_entry_size(ent);
		if (!asset->size) {
			errno = ENOENT; /* This is close enough for our purposes */
			return NULL;
		}

		/* Allocate a buffer for the file */
		asset->data = PURPL_CALLOC(asset->size + 1, char);
		if (!asset->data)
			return NULL;

		/* And at last read it */
		archive_read_data(ar, asset->data, asset->size);

		/* Append a 0 at the end of the buffer */
		asset->data[asset->size] = '\0';

		/* Fill in the name of the asset */
		asset->name =
			PURPL_CALLOC(strlen(archive_entry_pathname(ent)), char);
		if (!asset->name)
			return NULL;
		strcpy(asset->name, archive_entry_pathname(ent));

		/* If we're here, the loop can end */
		break;
	}

	PURPL_RESTORE_ERRNO(___errno);

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
	s64 name_len;
	char *full_name;
	s64 full_name_len;
	char **paths;
	char *tmp;
	char *paths_full;
	u8 path_count;
	u8 i;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

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
	tmp = strtok(paths_full, PURPL_PATH_SEP_STR);
	if (!tmp) { /* Blame the user, they probably messed up */
		errno = EINVAL;
		return NULL;
	}
	for (path_count = 1;
	     path_count < PURPL_MAX_PATHS && strtok(NULL, PURPL_PATH_SEP_STR);
	     path_count++)
		;

	/* Now allocate the array of pointers we need */
	paths = PURPL_CALLOC(path_count, char *);
	if (!paths)
		return NULL;

	/* The first round of tokenization fucks up paths_full, rebuild it */
	strcpy(paths_full, search_paths);

	/* Separate the paths into individual pointers */
	tmp = strtok(paths_full, PURPL_PATH_SEP_STR);
	paths[0] = PURPL_CALLOC(strlen(tmp), char);
	if (!paths[0])
		return NULL;
	strcpy(paths[0], tmp);
	for (i = 1; i < path_count; i++) {
		tmp = strtok(NULL, PURPL_PATH_SEP_STR);
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
		errno = 0;
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
		purpl_read_file_fp(&asset->size, &asset->mapping, &map, fp);
	if (!asset->data)
		return NULL;
	asset->mapped = map;

	/* Close the file */
	fclose(fp);

	PURPL_RESTORE_ERRNO(___errno);

	/* Return the asset */
	return asset;
}

void purpl_free_asset(struct purpl_asset *asset)
{
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Avoid a segfault/double free */
	if (!asset || !asset->mapping) {
		errno = EINVAL;
		return;
	}

	/* If the file is mapped, deal with that */
	if (asset->mapped)
		purpl_unmap_file(asset->mapping);
	else /* Otherwise free the data */
		free(asset->data);

	/* Free the rest of the structure */
	free(asset);

	PURPL_RESTORE_ERRNO(___errno);
}

void purpl_free_embed(struct purpl_embed *embed)
{
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	if (!embed) {
		errno = EINVAL;
		return;
	}

	/* Close the libarchive handle */
	archive_read_close(embed->ar);
	archive_read_free(embed->ar);

	/* Free the embed */
	free(embed);

	PURPL_RESTORE_ERRNO(___errno);
}

#ifdef __cplusplus
}
#endif