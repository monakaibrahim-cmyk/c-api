#pragma once

#define STDIN 0x00  // standard input file descriptor
#define STDOUT 0x01 // standard output file descriptor
#define STDERR 0x02 // standard error file descriptor

#define TRUE 0x01  // boolean true value (custom definition)
#define FALSE 0x00 // boolean false value (custom definition)
#define true 0x01  // lowercase alias for TRUE
#define false 0x00 // lowercase alias for FALSE

#define EXIT_SUCCESS 0x00 // program exit code for success
#define EXIT_FAILURE 0x01 // program exit code for failure

#define NULL ((void *)0x00) // null pointer definition

#include "core/utils/tools.h"  // include utility tools header
#include "core/utils/stdout.h" // include stdout-related functions header
