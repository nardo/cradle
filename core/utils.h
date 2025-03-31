// The core library - copyright KAGR LLC, portions copyright GarageGames. The use of this source code is governed by the MIT software license described in the "license.txt" file in this directory.

#if 0

static void output_debug_string();
static void os_debug_break();
static void exit(int code);
static void alert_ok(const char *window_title, const char *message);
static void os_sleep(uint32 millisecond_count);

#elif defined(PLATFORM_WIN32)

static void output_debug_string(const char *string)
{
	OutputDebugStringA(string);
}

static void os_debug_break()
{
	DebugBreak();
}

static void exit(int code)
{
	ExitProcess(code);
}

static void alert_ok(const char *window_title, const char *message)
{
	ShowCursor(true);
	MessageBoxA(NULL, message, window_title, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
}

static void os_sleep(uint32 millisecond_count)
{
	Sleep(millisecond_count);
}

#else

#define os_debug_break() raise(SIGTRAP)

static void os_sleep(uint32 millisecond_count)
{
    usleep(millisecond_count * 1000);
}
#endif
