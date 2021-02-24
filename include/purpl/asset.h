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

/**
 * @brief A structure to hold information about an asset
 */
struct purpl_asset {
	char *name; /**< The file name of the asset */
	char *data; /**< The raw data of the asset */
	size_t size; /**< The size of `data` */
};

/**
 * @brief Load an asset from a file, searching `search_paths`
 *
 * @param search_paths is the colon-separated list of absolute paths to search
 *  for `name` in.
 * @param map is whether or not to map the file instead of reading it into a
 *  buffer (probably set this to true)
 */
extern struct purpl_asset *purpl_load_asset_from_file(const char *search_paths,
						      bool map, const char *name, ...);

#endif /* !PURPL_ASSET_H */
