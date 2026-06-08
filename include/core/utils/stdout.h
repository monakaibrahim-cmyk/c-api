#pragma once

/**
 * @file stdout.h
 * @brief Standard output and formatting utilities.
 */

/** @brief Initialize variadic argument list */
#define va_start(v, l) __builtin_va_start(v, l)
/** @brief Retrieve next argument of type t */
#define va_arg(v, t) __builtin_va_arg(v, t)
/** @brief Clean up variadic argument list */
#define va_end(v) __builtin_va_end(v)
/** @brief Copy variadic argument list */
#define va_copy(d, s) __builtin_va_copy(d, s)

/** @brief Compiler builtin type for variadic args */
typedef __builtin_va_list va_list;

/**
 * @brief External system call wrapper for writing to file descriptor.
 * @param descriptor File descriptor (e.g., STDOUT).
 * @param buffer Buffer to write.
 * @param size Number of bytes to write.
 * @return Number of bytes written.
 */
[[nodiscard]] extern unsigned long sys_write(int descriptor, const char *buffer, unsigned long size);

/**
 * @brief Formatting context structure.
 */
typedef struct
{
	char *buffer;				/**< Output buffer (NULL means write to stdout) */
	unsigned long long size;	/**< Maximum buffer size */
	unsigned long long written; /**< Number of characters written so far */
} Context;

/**
 * @brief Writes a single character to the context.
 * @param ctx Formatting context.
 * @param character Character to write.
 */
static inline void write_char(Context *ctx, char character)
{
	if (ctx->buffer) // if writing to buffer instead of stdout
	{
		if (ctx->written < ctx->size - 1) // ensure space for null terminator
		{
			ctx->buffer[ctx->written] = character; // store character in buffer
			ctx->written++;						   // increment written count
		}
		else
		{
			return; // stop writing if buffer is full
		}
	}
	else
	{
		(void)sys_write(STDOUT, &character, 1); // write directly to stdout
		ctx->written++;							// count written characters
	}
}

/**
 * @brief Writes an integer to the context.
 * @param ctx Formatting context.
 * @param integer Integer to write.
 */
static inline void write_int(Context *ctx, int integer)
{
	char buffer[16]; // temporary buffer for digits
	int index = 0;	 // index into buffer

	if (integer == 0)
	{
		write_char(ctx, '0'); // special case for zero
		return;
	}

	if (integer < 0)
	{
		write_char(ctx, '-'); // output minus sign

		unsigned int number = (integer < 0)
								  ? (unsigned int)(-(long long)integer) // handle negative conversion safely
								  : (unsigned int)integer;

		integer = number; // store converted value back into integer
	}

	unsigned int number = integer; // working copy of integer

	while (number > 0)
	{
		buffer[index++] = '0' + (number % 10); // extract digits in reverse order
		number /= 10;						   // reduce number
	}

	for (int i = index - 1; i >= 0; i--)
	{
		write_char(ctx, buffer[i]); // output digits in correct order
	}
}

/**
 * @brief Writes a floating-point number to the context.
 * @param ctx Formatting context.
 * @param number Float to write.
 */
static inline void write_float(Context *ctx, float number)
{
	if (number < 0)
	{
		write_char(ctx, '-'); // output minus sign
		number = -number;	  // make number positive
	}

	int integer = (int)number; // extract integer part

	write_int(ctx, integer); // print integer part
	write_char(ctx, '.');	 // decimal point

	float fraction = number - (float)integer; // fractional part

	for (int i = 0; i < 6; i++)
	{
		fraction *= 10.0f; // shift decimal digit left

		int decimal = (int)fraction; // extract digit

		write_char(ctx, '0' + decimal); // print digit

		fraction -= (float)decimal; // remove printed digit
	}
}

/**
 * @brief Writes a string to the context.
 * @param ctx Formatting context.
 * @param string String to write.
 */
static inline void write_string(Context *ctx, const char *string)
{
	if (!string)
	{
		string = "(null)"; // fallback for null pointer
	}

	for (int i = 0; string[i] != '\0'; i++)
	{
		write_char(ctx, string[i]); // output each character
	}
}

/**
 * @brief Core formatting function.
 * @param ctx Formatting context.
 * @param format Format string.
 * @param args Variadic arguments list.
 * @return Total written characters.
 */
[[nodiscard]] static inline int __vformat(Context *ctx, const char *format, va_list args)
{
	for (int i = 0; format[i] != '\0'; i++)
	{
		if (format[i] != '%')
		{
			write_char(ctx, format[i]); // normal character output
			continue;
		}

		i++; // move past '%'

		switch (format[i])
		{
		case 'c':
			write_char(ctx, va_arg(args, int)); // character
			break;
		case 'd':
			write_int(ctx, va_arg(args, int)); // integer
			break;
		case 'f':
			write_float(ctx, va_arg(args, double)); // float (promoted to double)
			break;
		case 's':
			write_string(ctx, va_arg(args, char *)); // string
			break;
		case '%':
			write_char(ctx, '%'); // escaped percent
			break;
		}
	}

	return (int)ctx->written; // return total written characters
}

/**
 * @brief Variadic printf implementation.
 * @param format Format string.
 * @param ... Arguments.
 * @return Number of characters written.
 */
[[nodiscard]] static inline int __vprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format); // initialize variadic arguments

	Context ctx =
		{
			.buffer = nullptr, // no buffer -> output to stdout
			.size = 0,
			.written = 0};

	int written = __vformat(&ctx, format, args); // format output

	va_end(args); // clean up variadic arguments

	return written; // return number of characters written
}

/**
 * @brief Variadic snprintf implementation.
 * @param buffer Output buffer.
 * @param size Buffer size limit.
 * @param format Format string.
 * @param ... Arguments.
 * @return Number of characters that would be written.
 */
[[nodiscard]] static inline int __vsnprintf(char *buffer, unsigned long long size, const char *format, ...)
{
	if (size == 0)
	{
		return 0; // cannot write anything
	}

	va_list args;
	va_start(args, format); // initialize variadic arguments

	Context ctx =
		{
			.buffer = buffer, // output buffer
			.size = size,	  // buffer size limit
			.written = 0};

	int written = __vformat(&ctx, format, args); // format into buffer

	va_end(args); // clean up variadic arguments

	if (buffer && size > 0)
	{
		if (ctx.written < ctx.size)
		{
			buffer[ctx.written] = '\0'; // null terminate within bounds
		}
		else
		{
			buffer[size - 1] = '\0'; // force termination at last byte
		}
	}

	return written; // return number of characters that would be written
}

/**
 * @brief printf macro wrapper.
 * @param format Format string.
 * @param ... Arguments.
 */
#define printf(format, ...) __vprintf(format, ##__VA_ARGS__)

/**
 * @brief snprintf macro wrapper.
 * @param buffer Output buffer.
 * @param size Buffer size.
 * @param format Format string.
 * @param ... Arguments.
 */
#define snprintf(buffer, size, format, ...) __vsnprintf(buffer, size, format, ##__VA_ARGS__)
