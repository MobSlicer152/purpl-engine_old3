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



#endif /* !PURPL_ASSET_H */
