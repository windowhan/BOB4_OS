#include "mmapio.h"

bool create_bob_txt(void)
{
	wchar_t *path = NULL;
	uint32_t pathlen = 0;
	pathlen = GetCurrentDirectoryW(pathlen, path);
	if (0 == pathlen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	path = (PWSTR)malloc(sizeof(WCHAR) * pathlen);
	if (0 == GetCurrentDirectoryW(pathlen, path))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(path);
		return false;
	}

	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(file_name, sizeof(file_name), L"%ws\\bob.txt", path)))
	{
		print("err, can not create file name");
		free(path);
		return false;
	}

	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}

	DWORD bytes_written = 0;
	wchar_t string_buf[1024];

	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}
	char tempa[1024];
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, tempa, len, NULL, NULL);

	if (!WriteFile(file_handle, tempa, strlen(tempa), &bytes_written, NULL))		// temp -> file
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	len = WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, tempa, len, NULL, NULL);

	if (!WriteFile(file_handle, tempa, strlen(tempa), &bytes_written, NULL))		// temp -> file
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	char string_bufa[1024];
	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	wchar_t tempw[1024 * 2];
	
	len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string_bufa, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string_bufa, -1, tempw, len);
	len = WideCharToMultiByte(CP_UTF8, 0, tempw, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, tempw, -1, string_bufa, len, NULL, NULL);

	if (!WriteFile(file_handle, string_bufa, strlen(string_bufa), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string_bufa, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string_bufa, -1, tempw, len);
	len = WideCharToMultiByte(CP_UTF8, 0, tempw, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, tempw, -1, string_bufa, len, NULL, NULL);

	if (!WriteFile(file_handle, string_bufa, strlen(string_bufa), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}
	CloseHandle(file_handle);

	wchar_t file_name2[260];
	if (!SUCCEEDED(StringCbPrintfW(file_name2, sizeof(file_name), L"%ws\\bob2.txt", path)))
	{
		print("err, can not create file name");
		free(path);
		return false;
	}

	if (true == is_file_existsW(file_name2))
	{
		::DeleteFileW(file_name2);
	}

	CopyFile(file_name, file_name2, false);

	HANDLE file_handle2 = CreateFileW(
		file_name2,
		GENERIC_READ,       
		FILE_SHARE_READ,    
		NULL,                
		OPEN_EXISTING,      
		FILE_ATTRIBUTE_NORMAL,
		NULL);           
	if (file_handle2 == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}

	memset(string_bufa, 0, sizeof(string_bufa));

	DWORD bytes_read = 0;

	if (!ReadFile(file_handle2, string_bufa, sizeof(string_bufa), &bytes_read, NULL))
	{
		print("err, ReadFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle2);
		return false;
	}
	CloseHandle(file_handle2);

	memset(string_buf, 0, sizeof(string_buf));
	len = MultiByteToWideChar(CP_UTF8, 0, string_bufa, strlen(string_bufa), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, string_bufa, strlen(string_bufa), string_buf, len);

	memset(string_bufa, 0, sizeof(string_bufa));
	len = WideCharToMultiByte(CP_ACP, 0, string_buf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, string_buf, -1, string_bufa, len, NULL, NULL);

	print("[Non-MemoryMapped I/O]\n%s\n\n", string_bufa);

	HANDLE file_handle3 = CreateFileW(
		file_name2,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == file_handle3 )
	{
		print("err, CreateFile(%ws) failed, gle = %u", file_name2, GetLastError());
		return false;
	}

	LARGE_INTEGER fileSize;
	if (TRUE != GetFileSizeEx(file_handle3, &fileSize))
	{
		print("err, GetFileSizeEx(%ws) failed, gle = %u", file_name2, GetLastError());
		CloseHandle(file_handle3);
		return false;
	}

	DWORD file_size = (DWORD)fileSize.QuadPart;
	HANDLE file_map = CreateFileMapping(
		file_handle3,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
		);
	if (NULL == file_map)
	{
		print("err, CreateFileMapping(%ws) failed, gle = %u", file_name2, GetLastError());
		CloseHandle(file_handle3);
		return false;
	}

	PCHAR file_view = (PCHAR)MapViewOfFile(
		file_map,
		FILE_MAP_READ,
		0,
		0,
		0
		);
	if (file_view == NULL)
	{
		print("err, MapViewOfFile(%ws) failed, gle = %u", file_name2, GetLastError());

		CloseHandle(file_map);
		CloseHandle(file_handle3);
		return false;
	}

	memset(string_buf, 0, sizeof(string_buf));
	len = MultiByteToWideChar(CP_UTF8, 0, file_view, strlen(file_view), NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, file_view, strlen(file_view), string_buf, len);

	memset(string_bufa, 0, sizeof(string_bufa));
	len = WideCharToMultiByte(CP_ACP, 0, string_buf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, string_buf, -1, string_bufa, len, NULL, NULL);

	// close all
	UnmapViewOfFile(file_view);
	CloseHandle(file_map);
	CloseHandle(file_handle3);

	print("[MemoryMapped I/O] \n%s\n\n", string_bufa);

	DeleteFile(file_name);
	DeleteFile(file_name2);

	free(path);
	return true;
}

int main(void)
{

	create_bob_txt();

}