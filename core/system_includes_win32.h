// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

#include "stdio.h" // C standard i/o library
#include "winsock.h" // windows implementation of berkeley sockets networking API
#include <windows.h>
#include <timeapi.h>

#include <new>

typedef int socklen_t;

#define strcasecmp stricmp
