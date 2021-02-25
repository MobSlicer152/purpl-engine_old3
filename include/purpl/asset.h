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

#include "util.h"

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
extern struct purpl_asset *purpl_load_asset_from_file(const char *search_paths,
						      bool map, const char *name, ...);

extern void purpl_free_asset(struct purpl_asset *asset);

#endif /* !PURPL_ASSET_H */
