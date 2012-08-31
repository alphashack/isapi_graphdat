#include "debug.h"

#define DEBUG_FILE "graphdat_debug.log"
extern HMODULE MODULE_HANDLE;

FILE * _debug_fp = NULL;

void debug_init()
{
	char * path;

	rsize_t len = MAX_PATH + 1;
	char exepath[MAX_PATH + 1];
	if (GetModuleFileNameA(MODULE_HANDLE, exepath, MAX_PATH) != 0)
	{
		char * psz = strrchr(exepath, '\\') + 1;
		len -= (psz - exepath);

		strcpy_s(psz, len, DEBUG_FILE);

		path = exepath;
	}
	else
	{
		return;
	}

	_debug_fp = _fsopen(path, "a", _SH_DENYNO);
}

void debug_term()
{
	if(_debug_fp != NULL)
		fclose(_debug_fp);
}

FILE * debug_fp()
{
	return _debug_fp;
}
