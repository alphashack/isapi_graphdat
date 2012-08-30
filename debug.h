#ifndef ISAPI_GRAPHDAT_DEBUG_H
#define ISAPI_GRAPHDAT_DEBUG_H

#include "common.h"

#define debug_log(...) { \
	struct timeval tv; \
	gettimeofday(&tv, NULL); \
	fprintf(debug_fp(), "%u :: ", (unsigned int)tv.tv_sec); \
	fprintf(debug_fp(), __VA_ARGS__); \
	fflush(debug_fp()); \
}

void debug_init();

void debug_term();

FILE * debug_fp();

#endif
