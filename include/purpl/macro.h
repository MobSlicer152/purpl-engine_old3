#pragma once

#ifndef PURPL_MACRO_H
#define PURPL_MACRO_H 1

/**
 * @brief Gets the size of an array
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * @brief Concatenates two values together
 */
#define CONCAT(hi, lo, type, target) ((target)((hi) << (sizeof(type) << 3) | (type)(lo)))

/**
 * @brief Gets the higher half of a value
 */
#define HIGH(val, target) ((target)((val) >> (sizeof(val) << 3)))

/**
 * @brief Gets the lower half of a value
 */
#define LOW(val, target) ((target)((val) & ((1 << (sizeof(val) << 3)) - 1)))

/**
 * @brief The base name and extension of the current file.
 */
#if _WIN32 && \
	_MSC_VER /* MSVC is the only Windows compiler that uses backslashes in __FILE__ */
#define FILENAME \
	(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define FILENAME \
	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/**
 * @brief Exports a function
 */
#ifdef _WIN32 && _MSC_VER
#define PURPL_EXPORT __declspec(dllexport)
#else
#define PURPL_EXPORT extern
#endif

#endif /* !PURPL_MACRO_H */
