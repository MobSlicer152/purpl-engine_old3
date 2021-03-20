/**
 * @file purpl.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Interface and initialization/deinitialization functions for the Purpl Engine API
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef PURPL_H
#define PURPL_H 1

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <SDL.h>

#include <stb_ds.h>

#include "app_info.h"
#include "asset.h"
#include "log.h"
#include "types.h"
#include "util.h"
#include "window.h"

/**
 * @brief This is an internal structure for keeping track of assets, don't mess
 *  with it
 */
struct purpl_asset_list {
	char *key;
	struct purpl_asset *value;
};

/**
 * @brief A structure that holds critical information about an instance of the
 *  engine. This is a collection of other structures that provide access to the
 *  engine's facilities.
 */
struct purpl_inst {
	bool running; /**< Whether the instance is running */
	struct purpl_app_info *info; /**< The app info for the instance */
	struct purpl_logger *logger; /**< The logger for the instance */
	u8 logindex; /**< The default log index */
	struct purpl_embed *embed; /**< The embedded archive if one was given */
	struct purpl_asset_list
		*assets; /**< The list of assets opened, see `stb_ds.h` */
	struct SDL_Window *wnd; /**< The SDL window */
};

/**
 * @brief Initialize the engine's facilities.
 * 
 * @param allow_external_app_info is whether to allow an external app info file
 *  to be searched for before the embedded archive (if one is specified)
 * @param start_log is whether to start the logger with the defaults
 * @param embed_start is the start of an embedded file to be treated as an
 *  archive. If this is `NULL`, `allow_external_app_info` and `embed_end` are
 *  ignored.
 * @param embed_end is the end of the archive
 * @param app_info_path is the path to the app info. If this is `NULL`,
 *  it will be assumed to be "app.json".
 * 
 * @return Returns an initialized `purpl_inst` structure. CALL `purpl_end_inst`
 *  BEFORE CALLING THIS AGAIN IF YOU DO THAT AT ALL.
 */
extern struct purpl_inst *purpl_create_inst(bool allow_external_app_info,
					    bool start_log, char *embed_start,
					    char *embed_end,
					    char *app_info_path, ...);

extern const char *purpl_inst_load_asset_from_file(struct purpl_inst *inst, bool map,
					   const char *name, ...);

/**
 * @brief Free an asset loaded with one of the instance-based functions
 * 
 * @param inst is the instance to remove the asset from
 * @param name is the name of the asset to remove (make sure it's the return
 *  value of the function that allocated and returned the string, not a copy.
 *  If you ignore the previous sentence, you'll get a memory leak or a double
 *  -free type of thing) 
 */
extern void purpl_inst_free_asset(struct purpl_inst *inst, const char *name);

/**
 * @brief End an instance
 * 
 * @param inst is the instance to end
 */
extern void purpl_end_inst(struct purpl_inst *inst);

#endif /* !PURPL_H */
