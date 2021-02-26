/**
 * @file asset.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Utilities for loading assets
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 */

#pragma once

#ifndef PURPL_ASSET_H
#define PURPL_ASSET_H 1

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include <archive.h>
#include <archive_entry.h>

#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The max number of asset paths that will be processed 
 */
#define PURPL_MAX_PATHS 255

/**
 * @brief A structure to hold information about an asset
 */
struct purpl_asset {
	char *name; /**< The file name of the asset */
	char *data; /**< The raw data of the asset */
	size_t size; /**< The size of `data` */
	struct purpl_mapping *mapping; /**< The mapping information */
	bool mapped; /**< Whether the file was mapped */
};

/**
 * @brief A structure to hold information about an embedded archive.
 * 
 * To use this to load assets, use the `ar` member and pass it to
 *  `purpl_load_asset_from_archive`.
 */
struct purpl_embed {
	char *start;
	char *end;
	size_t size;
	struct archive *ar;
};

/**
 * @brief Load the information about an embedded archive
 *  into a `purpl_embed` structure
 * 
 * @param sym_start The start of the data (created by objcopy)
 * @param sym_end The end of the data (also created by objcopy)
 * 
 * @return Returns a usable `purpl_embed` structure
 * 
 * Embedding an archive in your executable requires you to use the
 *  `tools/mkembed` utility that gets built when you build the engine.
 */
extern PURPL_EXPORT struct purpl_embed *purpl_load_embed(const char *sym_start,
					    const char *sym_end);

/**
 * @brief Loads an asset from an archive
 * 
 * @param ar is the libarchive object to load from (for embedded, use `embed->ar`)
 * @param path is the path within the archive to the asset
 * 
 * @return Returns NULL or a `purpl_asset` structure. Sets `errno` to ENOENT if
 *  the file is nonexistent or empty and EISDIR if it's a directory.
 */
extern PURPL_EXPORT struct purpl_asset *
purpl_load_asset_from_archive(struct archive *ar,
							 const char *path, ...);

/**
 * @brief Load an asset from a file, searching `search_paths`
 *
 * @param search_paths is the null-terminated, colon-separated list of
 *  absolute paths to search for `name` in.
 * @param map is whether or not to map the file instead of reading it into a
 *  buffer (probably set this to true)
 * @param name is the path to the file relative to the first search
 *  path where that name is found
 * 
 * @return Returns either NULL or a usable `purpl_asset` structure.
 */
extern PURPL_EXPORT struct purpl_asset *
purpl_load_asset_from_file(const char *search_paths,
						      bool map,
						      const char *name, ...);

/**
 * @brief Free the information associated with an asset
 * 
 * @param asset is the asset to free
 */
extern PURPL_EXPORT void purpl_free_asset(struct purpl_asset *asset);

#ifdef __cplusplus
}
#endif

#endif /* !PURPL_ASSET_H */
