#pragma once

#define va_start(v, l) __builtin_va_start(v, l) // initialize variadic argument list
#define va_arg(v, t) __builtin_va_arg(v, t)		// retrieve next argument of type t
#define va_end(v) __builtin_va_end(v)			// clean up variadic argument list
#define va_copy(d, s) __builtin_va_copy(d, s)	// copy variadic argument list

typedef __builtin_va_list va_list; // compiler builtin type for variadic args

[[nodiscard]] extern unsigned long sys_write(int descriptor, const char *buffer, unsigned long size);
// external system call wrapper for writing to file descriptor

typedef struct
{
	char *buffer;				// output buffer (NULL means write to stdout)
	unsigned long long size;	// maximum buffer size
	unsigned long long written; // number of characters written so far
} Context;

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

#define printf(format, ...) __vprintf(format, ##__VA_ARGS__)								 // printf macro wrapper
#define snprintf(buffer, size, format, ...) __vsnprintf(buffer, size, format, ##__VA_ARGS__) // snprintf macro wrapper
