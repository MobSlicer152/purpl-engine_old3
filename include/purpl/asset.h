/**
 * @file asset.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Utilities for loading assets
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef PURPL_ASSET_H
#define PURPL_ASSET_H 1

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define LIBARCHIVE_STATIC
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
	char *start; /**< The start of the embed */
	char *end; /**< The end of the embed */
	size_t size; /**< The size of the embed */
	struct archive *ar; /**< The libarchive handle to the archive */
};

/**
 * @brief Load the information about an embedded archive
 *  into a `purpl_embed` structure
 * 
 * @param sym_start The start of the data (created by mkembed)
 * @param sym_end The end of the data (also created by mkembed)
 *
 * @return Returns `NULL` or a `purpl_embed` structure.
 * 
 * Embedding an archive in your executable requires you to use the
 *  `tools/mkembed` utility that gets built when you build the engine.
 */
extern struct purpl_embed *purpl_load_embed(const char *sym_start,
					    const char *sym_end);

/**
 * @brief Loads an asset from an archive
 * 
 * @param ar is the libarchive object to load from (for embedded, use `embed->ar`)
 * @param path is the path within the archive to the asset
 * 
 * @return Returns `NULL` or a `purpl_asset` structure. Sets `errno` to ENOENT if
 *  the file is nonexistent or empty and EISDIR if it's a directory.
 */
extern struct purpl_asset *purpl_load_asset_from_archive(struct archive *ar,
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
 * @return Returns either `NULL` or a usable `purpl_asset` structure.
 */
extern struct purpl_asset *purpl_load_asset_from_file(const char *search_paths,
						      bool map,
						      const char *name, ...);

/**
 * @brief Free the information associated with an asset
 * 
 * @param asset is the asset to free
 */
extern void purpl_free_asset(struct purpl_asset *asset);

/**
 * @brief Free the information associated with an embed
 * 
 * @param embed is the `purpl_embed` structure to free
 */
extern void purpl_free_embed(struct purpl_embed *embed);

#ifdef __cplusplus
}
#endif

#endif /* !PURPL_ASSET_H */
