#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>
#include <Strsafe.h>
#include <crtdbg.h>

typedef unsigned __int32 uint32_t;

void print(_In_ const char* fmt, _In_ ...);
bool is_file_existsW(wchar_t * FileName);

