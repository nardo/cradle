// The kt programming language compiler - copyright KAGR LLC. The use of this source code is governed by the MIT license agreement described in the "license.txt" file in the parent "kt" directory.

typedef unsigned int uint;
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef assert
#define assert(x)
#endif

#include "memory_functions.h"
#include "formatted_string_buffer.h"
#include "page_allocator.h"
#include "parser_interface.h"

class kt_lexer;
extern void parse_buffer(const char *parse_string, uint parse_string_len, parse_result &result);
