#include "debug.h"

FILE * _debug_fp;

void debug_init()
{
	_debug_fp = _fsopen("c:\\tmp\\debug.log", "a", _SH_DENYNO);
}

void debug_term()
{
	fclose(_debug_fp);
}

FILE * debug_fp()
{
	return _debug_fp;
}
