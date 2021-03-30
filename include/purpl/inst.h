#pragma once

#ifndef PURPL_INST_H
#define PURPL_INST_H 1

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <SDL.h>

#if PURPL_USE_OPENGL_GFX
#include <GL/glew.h>
#include <SDL_opengl.h>
#endif

#include <stb_ds.h>

#include "app_info.h"
#include "log.h"
#include "types.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macros for SDL window creation flags  
 */
#if PURPL_USE_OPENGL_GFX
#define PURPL_GRAPHICS_FLAGS SDL_WINDOW_OPENGL
#endif

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
	int default_x; /**< The non-fullscreen x position of the window */
	int default_y; /**< The non-fullscreen y position of the window */
	int default_w; /**< The non-fullscreen width of the window */
	int default_h; /**< The non-fullscreen height of the window */

	/* Graphics API specifics */
#if PURPL_USE_OPENGL_GFX
	SDL_GLContext ctx;
#endif
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

/**
 * @brief Create a window and renderer associated with an instance
 * 
 * @param inst is the instance to create the window for
 * @param fullscreen is whether to make a fullscreen window, in which case
 *  `width`/`height` are ignored
 * @param width is the width of the window (-1 for 1024)
 * @param height is the height of the window (-1 for 600)
 * @param title is the title of the window
 * 
 * @return Returns the value of `errno` on failure or 0 on success. If a window
 *  is already open, returns/sets `errno` to `EEXIST`
 */
extern int purpl_inst_create_window(struct purpl_inst *inst, bool fullscreen,
				    int width, int height, const char *title,
				    ...);

/**
 * @brief Initialize graphics for `inst`
 * 
 * @param inst is the instance to initialize graphics for
 *
 * @return Returns 0 for success or sets and returns `errno` in the event of an
 *  error.
 * 
 * Call this _after_ you've created a window with `purpl_inst_create_window`.
 */
extern int purpl_inst_init_graphics(struct purpl_inst *inst);

/**
 * @brief Run `inst`
 * 
 * @param inst is the instance to run
 * @param user is optional user data to be passed to the `frame` callback
 * @param frame is a callback that is called each frame after window events are
 *  processed and before the renderer is updated
 * 
 * @return Returns the amount of time passed since the start of the function.
 * 
 * It is recommended to run this on a separate thread.
 */
extern uint purpl_inst_run(struct purpl_inst *inst, void *user,
			     void(frame)(struct purpl_inst *inst, SDL_Event e,
					 uint delta,
				       void *user));

/**
 * @brief Load an asset from a file into the instance's assset list
 * 
 * @param inst is the instance to act on
 * @param map is whether to attempt to map the file
 * @param name is the name of the file
 * 
 * @return Returns the full path to the asset, to access it within the list.
 */
extern const char *purpl_inst_load_asset_from_file(struct purpl_inst *inst,
						   bool map, const char *name,
						   ...);

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
 * @brief Close an instance's window if one is open
 * 
 * @param inst is the instance whose window is to be closed
 */
extern void purpl_inst_destroy_window(struct purpl_inst *inst);

/**
 * @brief End an instance
 * 
 * @param inst is the instance to end
 */
extern void purpl_end_inst(struct purpl_inst *inst);

#ifdef __cplusplus
}
#endif

#endif /* !PURPL_INST_H */
