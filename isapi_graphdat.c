#include "common.h"

#include "isapi_graphdat.h"
#include "lib/module_graphdat/graphdat.h"
#include "configuration.h"
#include "debug.h"

#ifndef ISAPI_GRAPHDAT_SOURCE
#define ISAPI_GRAPHDAT_SOURCE "iis"
#endif

static HANDLE  s_eventSource = NULL;
gd_config_t CONFIG;
HMODULE MODULE_HANDLE;

static double s_performanceFrequency = 0;

typedef struct
{
	LARGE_INTEGER start;
	LARGE_INTEGER end;
} request_time_t;

void delegate_logger(graphdat_log_t type, void * user, const char * fmt, ...)
{
	if(s_eventSource == NULL)
	{
		 s_eventSource = RegisterEventSource(NULL, TEXT("Graphdat_ISAPI"));
	}

	if (s_eventSource != NULL)
	{
			va_list argp;
			va_start(argp, fmt);

			char buff[1024];
			vsprintf_s(buff, fmt, argp);

			va_end(argp);

			LPCSTR message = buff;

			ReportEventA(
				s_eventSource, // handle of event source
				type, // error event type (this is the same as graphdat_log_t fortunately)
				0, // event category
				0x20000001L, // generic event ID
				NULL, // current user's SID
				1, // strings in lpszStrings
				0, // no bytes of raw data
				&message, // array of error strings
				NULL); // no raw data
	}
}

void isapi_init()
{
	LARGE_INTEGER li;
	if(QueryPerformanceFrequency(&li))
	{
		s_performanceFrequency = double(li.QuadPart)/1000.0;
	}

	if(CONFIG.debug)
	{
		debug_log("PerformanceFrequency %d\n", s_performanceFrequency);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	MODULE_HANDLE = hModule;
        switch (ul_reason_for_call)
        {
			case DLL_PROCESS_ATTACH:
				config_init();
				if(CONFIG.debug)
				{
					debug_init();
				}
				graphdat_init(CONFIG.agent_request_socket_config, strlen(CONFIG.agent_request_socket_config), ISAPI_GRAPHDAT_SOURCE, strlen(ISAPI_GRAPHDAT_SOURCE), delegate_logger, NULL);
				isapi_init();
				break;
			case DLL_THREAD_ATTACH:
				break;
			case DLL_THREAD_DETACH:
				break;
			case DLL_PROCESS_DETACH:
				graphdat_term(delegate_logger, NULL);
				if(s_eventSource != NULL)
				{
					DeregisterEventSource(s_eventSource);
				}
				config_free();
				if(CONFIG.debug)
				{
					debug_term();
				}
				break;
        }
        return TRUE;
}

BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
        pVer->dwFilterVersion = MAKELONG(1, 0);
        strcpy_s(pVer->lpszFilterDesc, "GraphdatFilter, Version 1.0");

        pVer->dwFlags = SF_NOTIFY_PREPROC_HEADERS | SF_NOTIFY_END_OF_REQUEST | SF_NOTIFY_LOG;
				
		return TRUE;
}

DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD NotificationType, VOID *pvData)
{
	    switch (NotificationType)
        {
		case SF_NOTIFY_LOG:
			{
				PHTTP_FILTER_LOG r = (PHTTP_FILTER_LOG)pvData;

				// resolution of request time is dependent on machine settings
				double diff_msec = double(((request_time_t *)pfc->pFilterContext)->end.QuadPart - ((request_time_t *)pfc->pFilterContext)->start.QuadPart) / s_performanceFrequency;

				graphdat_store((char *)r->pszOperation, strlen(r->pszOperation), (char *)r->pszTarget, strlen(r->pszTarget), diff_msec, delegate_logger, NULL, 0);

				if(CONFIG.debug)
				{
					debug_log("SF_NOTIFY_LOG %s %s %fms\n", r->pszOperation, r->pszTarget, diff_msec);
				}
			}
			break;
		case SF_NOTIFY_PREPROC_HEADERS:
			{
				pfc->pFilterContext = pfc->AllocMem(pfc, sizeof(request_time_t), NULL);
				QueryPerformanceCounter(&((request_time_t *)pfc->pFilterContext)->start);

				//if(CONFIG.debug)
				//{
				//	debug_log("SF_NOTIFY_PREPROC_HEADERS %lu\n", ((request_time_t *)pfc->pFilterContext)->start.QuadPart);
				//}
			}
			break;
		case SF_NOTIFY_END_OF_REQUEST:
			{
				QueryPerformanceCounter(&((request_time_t *)pfc->pFilterContext)->end);

				//if(CONFIG.debug)
				//{
				//	debug_log("SF_NOTIFY_END_OF_REQUEST %lu\n", ((request_time_t *)pfc->pFilterContext)->end.QuadPart);
				//}
			}
			break;
        default:
			break;
        }

        return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

BOOL WINAPI TerminateFilter(DWORD dwFlags)
{
    return TRUE;
}
