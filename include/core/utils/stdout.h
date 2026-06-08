#pragma once

#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, t) __builtin_va_arg(v, t)
#define va_end(v) __builtin_va_end(v)
#define va_copy(d, s) __builtin_va_copy(d, s)

typedef __builtin_va_list va_list;

[[nodiscard]] extern unsigned long sys_write(int descriptor, const char *buffer, unsigned long size);

typedef struct
{
	char *buffer;
	unsigned long long size;
	unsigned long long written;
} Context;

static inline void write_char(Context *ctx, char character)
{
	if (ctx->buffer)
	{
		if (ctx->written < ctx->size - 1)
		{
			ctx->buffer[ctx->written] = character;
			ctx->written++;
		}
		else
		{
			return;
		}
	}
	else
	{
		(void)sys_write(STDOUT, &character, 1);
		ctx->written++;
	}
}

static inline void write_int(Context *ctx, int integer)
{
	char buffer[16];
	int index = 0;

	if (integer == 0)
	{
		write_char(ctx, '0');
		return;
	}

	if (integer < 0)
	{
		write_char(ctx, '-');

		unsigned int number = (integer < 0)
								  ? (unsigned int)(-(long long)integer)
								  : (unsigned int)integer;

		integer = number;
	}

	unsigned int number = integer;

	while (number > 0)
	{
		buffer[index++] = '0' + (number % 10);
		number /= 10;
	}

	for (int i = index - 1; i >= 0; i--)
	{
		write_char(ctx, buffer[i]);
	}
}

static inline void write_float(Context *ctx, float number)
{
	if (number < 0)
	{
		write_char(ctx, '-');
		number = -number;
	}

	int integer = (int)number;

	write_int(ctx, integer);
	write_char(ctx, '.');

	float fraction = number - (float)integer;

	for (int i = 0; i < 6; i++)
	{
		fraction *= 10.0f;

		int decimal = (int)fraction;

		write_char(ctx, '0' + decimal);

		fraction -= (float)decimal;
	}
}

static inline void write_string(Context *ctx, const char *string)
{
	if (!string)
	{
		string = "(null)";
	}

	for (int i = 0; string[i] != '\0'; i++)
	{
		write_char(ctx, string[i]);
	}
}

[[nodiscard]] static inline int __vformat(Context *ctx, const char *format, va_list args)
{
	for (int i = 0; format[i] != '\0'; i++)
	{
		if (format[i] != '%')
		{
			write_char(ctx, format[i]);
			continue;
		}

		i++;

		switch (format[i])
		{
		case 'c':
			write_char(ctx, va_arg(args, int));
			break;
		case 'd':
			write_int(ctx, va_arg(args, int));
			break;
		case 'f':
			write_float(ctx, va_arg(args, double));
			break;
		case 's':
			write_string(ctx, va_arg(args, char *));
			break;
		case '%':
			write_char(ctx, '%');
			break;
		}
	}

	return (int)ctx->written;
}

[[nodiscard]] static inline int __vprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	Context ctx =
		{
			.buffer = nullptr,
			.size = 0,
			.written = 0

		};

	int written = __vformat(&ctx, format, args);

	va_end(args);

	return written;
}

[[nodiscard]] static inline int __vsnprintf(char *buffer, unsigned long long size, const char *format, ...)
{
	if (size == 0)
	{
		return 0;
	}

	va_list args;
	va_start(args, format);

	Context ctx =
		{
			.buffer = buffer,
			.size = size,
			.written = 0};

	int written = __vformat(&ctx, format, args);

	va_end(args);

	if (buffer && size > 0)
	{
		if (ctx.written < ctx.size)
		{
			buffer[ctx.written] = '\0';
		}
		else
		{
			buffer[size - 1] = '\0';
		}
	}

	return written;
}

#define printf(format, ...) __vprintf(format, ##__VA_ARGS__)
#define snprintf(buffer, size, format, ...) __vsnprintf(buffer, size, format, ##__VA_ARGS__)
