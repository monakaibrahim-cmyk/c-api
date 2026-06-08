#pragma once

/**
 * @file stdlib.h
 * @brief Standard library definitions and utilities.
 */

/** @brief Standard input file descriptor */
#define STDIN 0x00
/** @brief Standard output file descriptor */
#define STDOUT 0x01
/** @brief Standard error file descriptor */
#define STDERR 0x02

/** @brief Boolean true value (custom definition) */
#define TRUE 0x01
/** @brief Boolean false value (custom definition) */
#define FALSE 0x00
/** @brief Lowercase alias for TRUE */
#define true 0x01
/** @brief Lowercase alias for FALSE */
#define false 0x00

/** @brief Program exit code for success */
#define EXIT_SUCCESS 0x00
/** @brief Program exit code for failure */
#define EXIT_FAILURE 0x01

/** @brief Null pointer definition */
#define NULL ((void *)0x00)

#include "core/utils/tools.h"  // include utility tools header
#include "core/utils/stdout.h" // include stdout-related functions header
