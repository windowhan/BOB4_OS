#include "mmapio.h"

void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args, fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}

bool is_file_existsW(wchar_t * FileName)
{
	_wfinddatai64_t c_file;
	intptr_t hFile;
	bool result = false;


	if ((hFile = _wfindfirsti64(FileName, &c_file)) == -1L)
		result = false;
	else
		if (c_file.attrib & _A_SUBDIR)
			result = false;
		else
			result = true;
	_findclose(hFile);

	return result;
}

