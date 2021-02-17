/**
 * @file log.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Logging functions
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 */

#pragma once

#ifndef PURPL_LOG_H
#define PURPL_LOG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stb_sprintf.h>

#include "types.h"
#include "util.h"

/**
 * @brief The max logs that can be open in a logger
 */
#define PURPL_MAX_LOGS 64

/**
 * @brief The different log levels
 * 
 * Any message written at a log level lower than the logger's max will be
 *  written. By the way, the WTF level is for when you have no clue what
 *  the fuck happened.
 */
enum purpl_log_level { WTF, FATAL, ERROR, WARNING, INFO, DEBUG };

/**
 * @brief Holds information about log files to be used with the
 *  logging functions
 */
struct purpl_logger {
	FILE **logs; /**< The log files this logger can use */
	ubyte nlogs : 6; /**< The number of logs open */
	ubyte default_index : 6; /**< The default log */
	ubyte default_level : 3; /**< The default log level */
	ubyte max_level[PURPL_MAX_LOGS]; /**< The max level to write for each log */
};

/**
 * @brief Initializes a `purpl_logger` structure
 * 
 * @param first_index_ret is the index of the first log
 * @param default_level is the default message level. -1 means use the default of INFO
 * @param first_max_level is the max message level for the first log. -1 means use the default of DEBUG
 * @param first_log_path is the path to the first log file
 * 
 * @return
 * Returns an initialized `purpl_logger` structure.
 * 
 * This function returns a `purpl_logger` structure, filled out based on the
 *  arguments passed. It can be passed to the other log functions. Use
 *  `purpl_close_log` to close an individual log or `purpl_end_logger` to
 *  close all the logs.
 */
extern struct purpl_logger *purpl_init_logger(const ubyte *first_index_ret,
					      byte default_level,
					      byte first_max_level,
					      const char *first_log_path, ...);

/**
 * @brief Opens a log file for a `purpl_logger` structure
 * 
 * @param logger is the logger structure to add the log to
 * @param max_level is the max message level for the log. -1 means use the default
 * @param path is the path to the log file
 * 
 * @return
 * Returns the index of the log opened.
 * 
 * This function opens a new log file for a `purpl_logger` structure
 *  to be written to. Close it with `purpl_logger_close`.
 */
extern int purpl_open_log(struct purpl_logger *logger, byte max_level,
			  const char *path, ...);

/**
 * @brief Writes a message to a log
 * 
 * @param logger is the logger structure to use
 * @param file should be the FILENAME macro
 * @param line should be the __LINE__ macro
 * @param index is the index of the log to write to
 * @param level is the message level
 * @param fmt the format string to be logged
 * 
 * @return
 * The number of bytes written.
 * 
 * Writes a message to a log opened by a `purpl_logger` structure indicated
 *  by `index`. Don't be an idiot, use the right format specifiers so that 
 *  your code is less vulnerable.
 */
size_t PURPL_EXPORT purpl_write_log(struct purpl_logger *logger,
				    const char *file, const int line,
				    byte index, byte level, const char *fmt,
				    ...);

/**
 * @brief Closes a log
 * 
 * @param logger is the logger structure to use
 * @param index is the index of the log
 */

#endif /* !PURPL_LOG_H */
