#pragma once

[[nodiscard]] extern unsigned long sys_strlen(const char *string);
[[nodiscard]] extern char *sys_strcpy(char *distination, const char *source);
extern void sys_timestamp(char *buffer, const char *format);

#define strlen(s) sys_strlen(s)
#define strcpy(x, y) sys_strcpy(x, y)

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define timestamp(...) GET_MACRO(__VA_ARGS__, TIMESTAMP_WITH_FORMAT, TIMESTAMP_NO_FORMAT)(__VA_ARGS__)

#define TIMESTAMP_NO_FORMAT(buffer) sys_timestamp(buffer, NULL)
#define TIMESTAMP_WITH_FORMAT(buffer, format) sys_timestamp(buffer, format)
