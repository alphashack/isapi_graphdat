#include "stdafx.h"

#include "isapi_graphdat.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
debug("DllMain: ");
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
debug("DLL_PROCESS_ATTACH\n");
                break;
        case DLL_THREAD_ATTACH:
debug("DLL_THREAD_ATTACH\n");
                break;
        case DLL_THREAD_DETACH:
debug("DLL_THREAD_DETACH\n");
                break;
        case DLL_PROCESS_DETACH:
debug("DLL_PROCESS_DETACH\n");
                break;
        }
        return TRUE;
}

BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
debug("GetFilterVersion\n");
        pVer->dwFilterVersion = MAKELONG(1, 0);
        strcpy(pVer->lpszFilterDesc, "GraphdatFilter, Version 1.0");

        pVer->dwFlags = SF_NOTIFY_SEND_RESPONSE | SF_NOTIFY_END_OF_REQUEST | SF_NOTIFY_LOG;
				
		return TRUE;
}

DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc, DWORD NotificationType, VOID *pvData)
{
debug("HttpFilterProc: ");

        switch (NotificationType)
        {
		case SF_NOTIFY_SEND_RESPONSE:
debug("SF_NOTIFY_SEND_RESPONSE\n");
				break;
        case SF_NOTIFY_END_OF_REQUEST:
debug("SF_NOTIFY_END_OF_REQUEST\n");
                break;
		case SF_NOTIFY_LOG:
debug("SF_NOTIFY_LOG\n");
				break;
        default:
debug("Unknown %d\n", NotificationType);
				break;
        }

        return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

BOOL WINAPI TerminateFilter(DWORD dwFlags)
{
debug("TerminateFilter\n");
        return TRUE;
}
