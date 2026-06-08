#pragma once

[[nodiscard]] extern unsigned long sys_strlen(const char *string);
// external function: returns length of string

[[nodiscard]] extern char *sys_strcpy(char *distination, const char *source);
// external function: copies source string into destination

extern void sys_timestamp(char *buffer, const char *format);
// external function: writes formatted timestamp into buffer

#define strlen(s) sys_strlen(s)
// macro wrapper around sys_strlen

#define strcpy(x, y) sys_strcpy(x, y)
// macro wrapper around sys_strcpy

#define GET_MACRO(_1, _2, NAME, ...) NAME
// helper macro to select correct overload based on argument count

#define timestamp(...) GET_MACRO(__VA_ARGS__, TIMESTAMP_WITH_FORMAT, TIMESTAMP_NO_FORMAT)(__VA_ARGS__)
// variadic macro dispatcher for timestamp() with or without format

#define TIMESTAMP_NO_FORMAT(buffer) sys_timestamp(buffer, NULL)
// timestamp call using default format (NULL)

#define TIMESTAMP_WITH_FORMAT(buffer, format) sys_timestamp(buffer, format)
// timestamp call using provided format string