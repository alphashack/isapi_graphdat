#ifndef ISAPI_GRAPHDAT_COMMON_H
#define ISAPI_GRAPHDAT_COMMON_H

#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <Winsock2.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);

#define strcasecmp(psz1, psz2) lstrcmpiA(psz1, psz2)

#endif
