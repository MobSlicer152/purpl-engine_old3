/**
 * @file app_info.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Facilities to load information about your application from a JSON
 *  file
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef PURPL_APP_INFO_H
#define PURPL_APP_INFO_H 1

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <json.h>

#include "asset.h"
#include "log.h"
#include "types.h"
#include "util.h"

/*
 * A structure representation of `app.json`, which can provide information such
 *  as app name, default log location, and asset search paths to your project.
 *  Use `root` to get custom keys not covered here if you need them. Look at
 *  the json-c examples and the app info functions for guidance.
 */
struct purpl_app_info {
	struct purpl_asset *json; /**< The JSON file loaded from the archive */
	struct json_object *root; /**< The root key of the JSON file */
	char *name; /**< The name of the app */
	char ver_maj; /**< The major version of the app */
	char ver_min; /**< The minor version of the app */
	char *search_paths; /**< Where to search for assets */
};

/**
 * @brief Load app info from the specified JSON file
 * 
 * @param embed is the embedded archive to search (optional)
 * @param allow_external is whether to check outside the embedded archive if
 *  one is specified. Useful for debugging and modding. If `embed` is NULL,
 *  this is ignored.
 * @param path is the location of the JSON file to use
 *  
 * @return Returns `NULL` or a usable `purpl_app_info` structure.
 * 
 * Loads app information from a JSON file. If you have an embedded archive
 *  (recommended), you can search that on its own, or check for an external
 *  file first, which can be helpful.
 */
extern struct purpl_app_info *purpl_load_app_info(struct purpl_embed *embed,
						  bool allow_external,
						  const char *path, ...);

#endif /* !PURPL_APP_INFO_H */
