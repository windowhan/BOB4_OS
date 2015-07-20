/**----------------------------------------------------------------------------
*
*-----------------------------------------------------------------------------
* All rights reserved by Noh,Yonghwan (fixbrain@gmail.com, unsorted@msn.com)
*-----------------------------------------------------------------------------
*
**---------------------------------------------------------------------------*/

// read
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366542(v=vs.85).aspx

#include "stdafx.h"
#include "FileIoHelperClass.h"
#include "mmio.h"
/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool read_file_using_memory_map()
{
	// current directory ?? ?????
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err ] GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR) * buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err ] GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ test.txt ??ϸ?????
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\test.txt",
		buf)))
	{
		print("err ] can not create file name");
		free(buf);
		return false;
	}
	free(buf); buf = NULL;

	if (true != is_file_existsW(file_name))
	{
		print("err ] no file exists. file = %ws", file_name);
		return false;
	}

	HANDLE file_handle = CreateFileW(
		(LPCWSTR)file_name,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == file_handle)
	{
		print("err ] CreateFile(%ws) failed, gle = %u", file_name, GetLastError());
		return false;
	}

	// check file size
	//
	LARGE_INTEGER fileSize;
	if (TRUE != GetFileSizeEx(file_handle, &fileSize))
	{
		print("err ] GetFileSizeEx(%ws) failed, gle = %u", file_name, GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// [ WARN ]
	//
	// 4Gb ?̻???????? MapViewOfFile()???? ?????? ???ų?
	// ????????̵????????? ??
	// FilIoHelperClass ???? ?̿?ؾ??
	//
	_ASSERTE(fileSize.HighPart == 0);
	if (fileSize.HighPart > 0)
	{
		print("file size = %I64d (over 4GB) can not handle. use FileIoHelperClass",
			fileSize.QuadPart);
		CloseHandle(file_handle);
		return false;
	}

	DWORD file_size = (DWORD)fileSize.QuadPart;
	HANDLE file_map = CreateFileMapping(
		file_handle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
		);
	if (NULL == file_map)
	{
		print("err ] CreateFileMapping(%ws) failed, gle = %u", file_name, GetLastError());
		CloseHandle(file_handle);
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
		print("err ] MapViewOfFile(%ws) failed, gle = %u", file_name, GetLastError());

		CloseHandle(file_map);
		CloseHandle(file_handle);
		return false;
	}

	// do some io
	char a = file_view[0];  // 0x d9
	char b = file_view[1];  // 0xb3



	// close all
	UnmapViewOfFile(file_view);
	CloseHandle(file_map);
	CloseHandle(file_handle);
	return true;
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool create_very_big_file(_In_ const wchar_t* file_path, _In_ uint32_t size_in_mb)
{
	_ASSERTE(NULL != file_path);
	if (NULL == file_path) return false;

	if (is_file_existsW(file_path))
	{
		::DeleteFileW(file_path);
	}

	// create very big file
	HANDLE file_handle = CreateFile(
		file_path,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == file_handle)
	{
		print("err ] CreateFile( %ws ) failed. gle = %u", file_path, GetLastError());
		return false;
	}

	LARGE_INTEGER file_size = { 0 };
	//file_size.LowPart = 0;
	//file_size.HighPart = 1;
	file_size.QuadPart = (1024 * 1024) * (LONGLONG)size_in_mb;

	if (!SetFilePointerEx(file_handle, file_size, NULL, FILE_BEGIN))
	{
		print("err ] SetFilePointerEx() failed. gle = %u", GetLastError());

		CloseHandle(file_handle);
		return false;
	}

	SetEndOfFile(file_handle);
	CloseHandle(file_handle);
	return true;
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
pmap_context open_map_context(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	if (NULL == file_path) return false;
	if (!is_file_existsW(file_path)) return false;;

	pmap_context ctx = (pmap_context)malloc(sizeof(map_context));
	RtlZeroMemory(ctx, sizeof(map_context));

	bool ret = false;

#pragma warning(disable: 4127)
	do
	{
		ctx->handle = CreateFileW(
			(LPCWSTR)file_path,
			GENERIC_READ,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == ctx->handle)
		{
			print("err ] CreateFile( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		// check file size
		//
		LARGE_INTEGER fileSize;
		if (TRUE != GetFileSizeEx(ctx->handle, &fileSize))
		{
			print("err ] GetFileSizeEx( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		// [ WARN ]
		//
		// 4Gb ?̻???????? MapViewOfFile()???? ?????? ???ų?
		// ????????̵????????? ??
		// FilIoHelperClass ???? ?̿?ؾ??
		//
		_ASSERTE(fileSize.HighPart == 0);
		if (fileSize.HighPart > 0)
		{
			print("err ] file is too large to map. file = %ws, size = %llu", file_path, fileSize.QuadPart);
			break;
		}

		ctx->size = (DWORD)fileSize.QuadPart;
		ctx->map = CreateFileMapping(
			ctx->handle,
			NULL,
			PAGE_READONLY,
			0,
			0,
			NULL
			);
		if (NULL == ctx->map)
		{
			print("err ] CreateFileMapping( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}
		
		ctx->view = (PCHAR)MapViewOfFile(
			ctx->map,
			FILE_MAP_READ,
			0,
			0,
			0
			);
		if (ctx->view == NULL)
		{
			print("err ] MapViewOfFile( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}
		
		ret = true;
	} while (FALSE);
#pragma warning(default: 4127)

	if (!ret)
	{
		if (NULL != ctx->view) UnmapViewOfFile(ctx->view);
		if (NULL != ctx->map) CloseHandle(ctx->map);
		if (INVALID_HANDLE_VALUE != ctx->handle) CloseHandle(ctx->handle);

		free(ctx); ctx = NULL;
	}

	return ctx;
}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
pmap_context create_map_context(_In_ const wchar_t* file_path, _In_ uint32_t file_size)
{
	// ??????????? Ȯ??
	_ASSERTE(NULL != file_path);
	if (NULL == file_path) return false;
	if (is_file_existsW(file_path))
	{
		DeleteFileW(file_path);
	}

	// ??context ???
	pmap_context ctx = (pmap_context)malloc(sizeof(map_context));

	// ??context ????
	RtlZeroMemory(ctx, sizeof(map_context));

	bool ret = false;

#pragma warning(disable: 4127)
	do
	{
		// ???????
		ctx->handle = CreateFileW(
			(LPCWSTR)file_path,
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == ctx->handle)
		{
			print("err ] CreateFile( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		// File Mapping
		ctx->size = file_size;
		ctx->map = CreateFileMapping(
			ctx->handle,
			NULL,
			PAGE_READWRITE,
			0,
			ctx->size,
			NULL
			);
		if (NULL == ctx->map)
		{
			print("err ] CreateFileMapping( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		ctx->view = (PCHAR)MapViewOfFile(
			ctx->map,
			FILE_MAP_WRITE,
			0,
			0,
			ctx->size
			);
		if (ctx->view == NULL)
		{
			print("err ] MapViewOfFile( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		ret = true;
	} while (FALSE);
#pragma warning(default: 4127)

	if (!ret)
	{
		if (NULL != ctx->view) UnmapViewOfFile(ctx->view);
		if (NULL != ctx->map) CloseHandle(ctx->map);
		if (INVALID_HANDLE_VALUE != ctx->handle) CloseHandle(ctx->handle);

		free(ctx); ctx = NULL;
	}

	return ctx;
}

pmap_context user_create_map_context(_In_ const wchar_t* file_path, _In_ uint32_t file_size)
{
	// ??????????? Ȯ??
	_ASSERTE(NULL != file_path);
	if (NULL == file_path) return false;
	if (is_file_existsW(file_path))
	{
		DeleteFileW(file_path);
	}

	// ??context ???
	pmap_context ctx = (pmap_context)malloc(sizeof(map_context));

	// ??context ????
	RtlZeroMemory(ctx, sizeof(map_context));

	bool ret = false;

#pragma warning(disable: 4127)
	do
	{
		// ???????
		ctx->handle = CreateFileW(
			(LPCWSTR)file_path,
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == ctx->handle)
		{
			print("err ] CreateFile( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		// File Mapping
		ctx->size = file_size;
		ctx->map = CreateFileMapping(
			ctx->handle,
			NULL,
			PAGE_READWRITE,
			0,
			ctx->size,
			NULL
			);
		if (NULL == ctx->map)
		{
			print("err ] CreateFileMapping( %ws ) failed. gle = %u", file_path, GetLastError());
			break;
		}

		ret = true;

		
	} while (false);

	if (!ret)
	{
		if (NULL != ctx->map) CloseHandle(ctx->map);
		if (INVALID_HANDLE_VALUE != ctx->handle) CloseHandle(ctx->handle);

		free(ctx); ctx = NULL;
	}

	return ctx;
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
void close_map_context(_In_ pmap_context ctx)
{
	if (NULL != ctx)
	{
		if (NULL != ctx->view) UnmapViewOfFile(ctx->view);
		if (NULL != ctx->map) CloseHandle(ctx->map);
		if (INVALID_HANDLE_VALUE != ctx->handle) CloseHandle(ctx->handle);
		free(ctx);
	}
}


/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool
file_copy_using_memory_map(
_In_ const wchar_t* src_file,
_In_ const wchar_t* dst_file
)
{
	_ASSERTE(NULL != src_file);
	_ASSERTE(NULL != dst_file);
	if (NULL == src_file || NULL == dst_file) return false;

	if (!is_file_existsW(src_file))
	{
		print("err ] no src file = %ws", src_file);
		return false;
	}

	if (is_file_existsW(dst_file))
	{
		DeleteFileW(dst_file);
	}

	//
	// MapViewOfFile() ?????dwFileOffsetLow ??????
	// SYSTEM_INFO::dwAllocationGranularity ???????̾? ???
	// 
	static DWORD AllocationGranularity = 0;
	if (0 == AllocationGranularity)
	{
		SYSTEM_INFO si = { 0 };
		GetSystemInfo(&si);
		AllocationGranularity = si.dwAllocationGranularity;
	}


	pmap_context src_ctx = open_map_context(src_file);
	pmap_context dest_ctx = user_create_map_context(dst_file, src_ctx->size);

	LARGE_INTEGER FileOffset = { 0 };
	LARGE_INTEGER FileSize = { dest_ctx->size };
	LARGE_INTEGER i = { 0 };
	LARGE_INTEGER BytesInBlock = { 0 };

	PUCHAR buffer = (PUCHAR)malloc((uint64_t)(1024 * 1024));
	while (FileSize.QuadPart > 0)
	{
		// ???? ???ũ?Ⱑ 1MB?????۴ٸ????? ũ?⸸ŭ?? ?? ???
		BytesInBlock.QuadPart = AllocationGranularity * 1;
		if (FileSize.QuadPart < AllocationGranularity * 1)
		{
			BytesInBlock.QuadPart = FileSize.QuadPart;
		}

		dest_ctx->view = (PCHAR)MapViewOfFile(
			dest_ctx->map,
			FILE_MAP_WRITE,
			(FileOffset.HighPart), // ??? ?????
			(FileOffset.LowPart), // ??? ?????
			BytesInBlock.QuadPart // ???????
			);
		if (dest_ctx->view == NULL)
		{
			print("err ] MapViewOfFile( %ws ) failed. gle = %u", dst_file, GetLastError());
			break;
		}

		RtlMoveMemory(buffer, &src_ctx->view[FileOffset.QuadPart], BytesInBlock.QuadPart);	// ??? 0??????????ְ?
		RtlMoveMemory(&dest_ctx->view[FileOffset.QuadPart], buffer, BytesInBlock.QuadPart);
		//RtlCopyMemory(&dest_ctx->view[FileOffset.QuadPart], &src_ctx->view[FileOffset.QuadPart], BytesInBlock.QuadPart);
	

		// ?並 ???????Ƿ? ?並 ??????
		UnmapViewOfFile(dest_ctx->view);

		// ????????? ũ??????
		FileOffset.QuadPart += BytesInBlock.QuadPart;
		FileSize.QuadPart -= BytesInBlock.QuadPart;

	}


	// ????????Ʈ?????????????Ʈ?? ?ݾ??
	CloseHandle(dest_ctx->map);
	CloseHandle(dest_ctx->handle);

	return true;
}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool
file_copy_using_read_write(
_In_ const wchar_t* src_file,
_In_ const wchar_t* dst_file
)
{
	_ASSERTE(NULL != src_file);
	_ASSERTE(NULL != dst_file);
	if (NULL == src_file || NULL == dst_file) return false;

	if (!is_file_existsW(src_file))
	{
		print("err ] no src file = %ws", src_file);
		return false;
	}

	if (is_file_existsW(dst_file))
	{
		DeleteFileW(dst_file);
	}

	// open src file with READ mode
	HANDLE src_handle = CreateFileW(
		src_file,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == src_handle)
	{
		print("err ] CreateFile( %ws ) failed. gle = %u", src_file, GetLastError());
		return false;
	}

	// open dst file with WRITE mode
	HANDLE dst_handle = CreateFileW(
		dst_file,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == dst_handle)
	{
		print("err ] CreateFile( %ws ) failed. gle = %u", dst_file, GetLastError());

		CloseHandle(src_handle);
		return false;
	}

	// file copy
	bool ret = false;
	char buf[4096] = { 0 };
	DWORD bytes_written = 0;
	DWORD bytes_read = 0;

	do
	{
		// read from src
		if (!ReadFile(src_handle, buf, sizeof(buf), &bytes_read, NULL))
		{
			print("err ] ReadFile( src_handle ) failed. gle = %u", GetLastError());
			break;
		}
		else
		{
			// please read
			// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365690(v=vs.85).aspx
			if (0 == bytes_read)
			{
				ret = true;
				break;
			}
		}

		// write to dst
		if (!WriteFile(dst_handle, buf, sizeof(buf), &bytes_written, NULL))
		{
			print("err ] WriteFile( dst_handle ) failed. gle = %u", GetLastError());
			break;
		}
	} while (true);


	CloseHandle(src_handle);
	CloseHandle(dst_handle);
	return ret;
}