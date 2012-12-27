#include "common.h"

#include "isapi_graphdat.h"
#include "lib/module_graphdat/graphdat.h"
#include "configuration.h"

#ifndef ISAPI_GRAPHDAT_SOURCE
#define ISAPI_GRAPHDAT_SOURCE "iis"
#endif

static HANDLE  s_eventSource = NULL;
gd_config_t CONFIG;
HMODULE MODULE_HANDLE;

typedef struct
{
	timeval start;
	timeval end;
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	MODULE_HANDLE = hModule;
        switch (ul_reason_for_call)
        {
			case DLL_PROCESS_ATTACH:
				config_init();
				graphdat_init(CONFIG.agent_request_socket_config, strlen(CONFIG.agent_request_socket_config), ISAPI_GRAPHDAT_SOURCE, strlen(ISAPI_GRAPHDAT_SOURCE), delegate_logger, NULL);
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

				// resolution of request time is usec
				unsigned long end_usec = ((request_time_t *)pfc->pFilterContext)->end.tv_sec * 1000000 + ((request_time_t *)pfc->pFilterContext)->end.tv_usec;
				unsigned long start_usec = ((request_time_t *)pfc->pFilterContext)->start.tv_sec * 1000000 + ((request_time_t *)pfc->pFilterContext)->start.tv_usec;
				long diff_usec = end_usec - start_usec;
				double diff_msec = (double)diff_usec / 1000;

				char hostname[DEFAULT_BUFFER_SIZE] = "\0";
				DWORD dwSize = DEFAULT_BUFFER_SIZE;
				if (pfc->GetServerVariable(pfc, "HTTP_HOST", hostname, &dwSize))
				{
					if (strlen (hostname) > 0)
					{
						graphdat_store((char *)r->pszOperation, strlen(r->pszOperation), (char *)r->pszTarget, strlen(r->pszTarget), (char *)hostname, strlen(hostname), diff_msec, delegate_logger, NULL, 0);
					}
				}
			}
			break;
		case SF_NOTIFY_PREPROC_HEADERS:
			{
				pfc->pFilterContext = pfc->AllocMem(pfc, sizeof(request_time_t), NULL);
				gettimeofday(&((request_time_t *)pfc->pFilterContext)->start, NULL);
			}
			break;
		case SF_NOTIFY_END_OF_REQUEST:
			{
				gettimeofday(&((request_time_t *)pfc->pFilterContext)->end, NULL);
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
