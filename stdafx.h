#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <httpfilt.h>

#include <Winsock2.h>

#include <time.h>
#include <stdio.h>
#include <string.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
