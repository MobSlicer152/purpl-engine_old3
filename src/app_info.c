#include "purpl/app_info.h"

#ifdef __cplusplus
extern "C" {
#endif

struct purpl_app_info *purpl_load_app_info(struct purpl_embed *embed,
					   bool allow_external,
					   const char *path, ...)
{
	struct purpl_app_info *info;
	va_list args;
	char *path_fmt;
	s64 path_len;
	struct json_object *name;
	struct json_object *log;
	struct json_object *ver_maj;
	struct json_object *ver_min;
	struct json_object *search_paths;
	struct json_object *obj;
	char *cur;
	bool alw_ext;
	size_t n_paths;
	s64 paths_len;
	size_t i;
	ptrdiff_t j;
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Check our parameters */
	if (!path) {
		errno = EINVAL;
		return NULL;
	}

	/* Format the path */
	va_start(args, path);
	path_fmt = purpl_fmt_text_va(&path_len, path, args);
	va_end(args);

	/* Allocate the structure */
	info = PURPL_CALLOC(1, struct purpl_app_info);
	if (!info)
		return NULL;

	/* Load the JSON file */
	alw_ext = allow_external;
	if (!embed || alw_ext) {
		info->json =
			purpl_load_asset_from_file(".", false, "%s", path_fmt);
		if (!info->json)
			alw_ext = false;
	}

	if (!alw_ext) {
		info->json = purpl_load_asset_from_archive(embed->ar, "%s",
							   path_fmt);
	}
	if (!info->json)
		return NULL;

	/* Read the root key from the file */
	info->root = json_tokener_parse(info->json->data);
	if (!info->root) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}

	/* Read the rest of the keys */
	name = json_object_object_get(info->root, "name");
	if (!name) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}
	log = json_object_object_get(info->root, "log_path");
	if (!log) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}
	ver_maj = json_object_object_get(info->root, "ver_maj");
	if (!ver_maj) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}
	ver_min = json_object_object_get(info->root, "ver_min");
	if (!ver_min) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}
	search_paths = json_object_object_get(info->root, "search_paths");
	if (!search_paths) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}

	/* Validate the keys */
	if (json_object_get_type(name) != json_type_string ||
	    json_object_get_type(log) != json_type_string ||
	    json_object_get_type(ver_maj) != json_type_int ||
	    json_object_get_type(ver_min) != json_type_int ||
	    json_object_get_type(search_paths) != json_type_array) {
		purpl_free_asset(info->json);
		errno = EINVAL;
		return NULL;
	}

	/* Parse the non-array stuff */
	info->name = json_object_get_string(name);
	if (errno) {
		purpl_free_asset(info->json);
		return NULL;
	}
	info->log = json_object_get_string(log);
	if (errno) {
		purpl_free_asset(info->json);
		return NULL;
	}
	info->ver_maj = json_object_get_int(ver_maj);
	if (errno) {
		purpl_free_asset(info->json);
		return NULL;
	}
	info->ver_min = json_object_get_int(ver_min);
	if (errno) {
		purpl_free_asset(info->json);
		return NULL;
	}

	/* Figure out how much space to allocate */
	n_paths = json_object_array_length(search_paths);
	paths_len = 0;
	for (i = 0; i < n_paths; i++) {
		/* Get a pointer to the current element */
		obj = json_object_array_get_idx(search_paths, i);
		if (!obj) {
			purpl_free_asset(info->json);
			return NULL;
		}

		/* Get a string */
		cur = json_object_get_string(obj);
		if (errno) {
			purpl_free_asset(info->json);
			return NULL;
		}

		/* Get its length */
		paths_len += strlen(cur);
		if (i < n_paths - 1)
			paths_len += sizeof(PURPL_PATH_SEP_STR);
	}

	/* Allocate the buffer */
	info->search_paths = PURPL_CALLOC(paths_len, char);
	if (!info->search_paths)
		return NULL;

	/* Copy in the paths */
	j = 0;
	for (i = 0; i < n_paths; i++) {
		/* Get a pointer to the current element */
		obj = json_object_array_get_idx(search_paths, i);
		if (!obj) {
			purpl_free_asset(info->json);
			return NULL;
		}

		/* Get a string */
		cur = json_object_get_string(obj);
		if (errno) {
			purpl_free_asset(info->json);
			return NULL;
		}

		/* Copy it to the buffer */
		j += sprintf(info->search_paths + j, "%s", cur);
		if (i < n_paths - 1)
			j += sprintf(info->search_paths + j, "%s",
				     PURPL_PATH_SEP_STR);
	}

	/* Free stuff */
	(path_len) ? (void)0 : free(path_fmt);

	PURPL_RESTORE_ERRNO(___errno);

	return info;
}

void purpl_free_app_info(struct purpl_app_info *info)
{
	int ___errno;

	PURPL_SAVE_ERRNO(___errno);

	/* Avoid a segfault/double free */
	if (!info) {
		errno = EINVAL;
		return;
	}

	/* Free the root JSON object */
	json_object_put(info->root);

	/* Free the JSON file's asset details */
	purpl_free_asset(info->json);

	/* Free the structure */
	free(info);

	PURPL_RESTORE_ERRNO(___errno);
}

#ifdef __cplusplus
}
#endif
