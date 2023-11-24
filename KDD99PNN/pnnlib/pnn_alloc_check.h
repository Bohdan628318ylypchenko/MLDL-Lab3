#pragma once

#include <stdio.h>
#include <stdlib.h>

/// <summary>
/// Macros to check for memory allocation fail.
/// </summary>
#define ALLOC_ERR_MSG "Error: memory allocation failed.\n"
#define pnn_fail_alloc_check(p)\
{\
	if (p == NULL)\
	{\
		fprintf(stderr, ALLOC_ERR_MSG);\
		abort();\
	}\
}

