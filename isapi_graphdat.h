#ifndef ISAPI_GRAPHDAT_H
#define ISAPI_GRAPHDAT_H

#include <time.h>

#define debug(...) { \
	FILE * fp = fopen("c:\\tmp\\debug.log", "a"); \
	struct timeval tv; \
	gettimeofday(&tv, NULL); \
	fprintf(fp, "%u :: ", (unsigned int)tv.tv_sec); \
	fprintf(fp, __VA_ARGS__); \
	fclose(fp); \
}

#endif /* ISAPI_GRAPHDAT_H */
