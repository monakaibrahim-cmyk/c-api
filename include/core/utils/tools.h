#pragma once

/**
 * @file tools.h
 * @brief Core utility tools.
 */

/**
 * @brief Returns length of string.
 * @param string Null-terminated string.
 * @return Length of the string.
 */
[[nodiscard]] extern unsigned long sys_strlen(const char *string);

/**
 * @brief Copies source string into destination.
 * @param distination Destination buffer.
 * @param source Null-terminated source string.
 * @return Pointer to the destination string.
 */
[[nodiscard]] extern char *sys_strcpy(char *distination, const char *source);

/**
 * @brief Writes formatted timestamp into buffer.
 * @param buffer Output buffer.
 * @param format Timestamp format string (can be NULL for default format).
 */
extern void sys_timestamp(char *buffer, const char *format);

/**
 * @brief Macro wrapper around sys_strlen.
 * @param s String to measure.
 */
#define strlen(s) sys_strlen(s)

/**
 * @brief Macro wrapper around sys_strcpy.
 * @param x Destination buffer.
 * @param y Source string.
 */
#define strcpy(x, y) sys_strcpy(x, y)

/**
 * @brief Helper macro to select correct overload based on argument count.
 */
#define GET_MACRO(_1, _2, NAME, ...) NAME

/**
 * @brief Variadic macro dispatcher for timestamp() with or without format.
 */
#define timestamp(...) GET_MACRO(__VA_ARGS__, TIMESTAMP_WITH_FORMAT, TIMESTAMP_NO_FORMAT)(__VA_ARGS__)

/**
 * @brief Timestamp call using default format (NULL).
 * @param buffer Output buffer.
 */
#define TIMESTAMP_NO_FORMAT(buffer) sys_timestamp(buffer, NULL)

/**
 * @brief Timestamp call using provided format string.
 * @param buffer Output buffer.
 * @param format Format string.
 */
#define TIMESTAMP_WITH_FORMAT(buffer, format) sys_timestamp(buffer, format)