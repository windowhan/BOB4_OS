// ic.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdint.h>
#include <conio.h>
#include "FileIoHelperClass.h"
#include "StopWatch.h"
#include "StatusCode.h"
#include "mmio.h"
#include "stdafx.h"
#include "windows.h"
#include "Strsafe.h"
#include "stdint.h"
#include "crtdbg.h"
#include <iostream>
#define BUF_SIZE 4096

int main(){
	// Create_very_big_file
	create_very_big_file(L"big.txt", 1024 * 5);
	StopWatch sw, sw2;

	// file_copy_read_write case
	sw.Start();
	file_copy_using_read_write(L"big.txt", L"big2.txt");
	sw.Stop();
	printf("not mmio : %f\n", sw.GetDurationMilliSecond());


	// mmio case
	sw2.Start();
	FileIoHelper WFi = FileIoHelper();
	FileIoHelper RFi = FileIoHelper();
	
	LARGE_INTEGER FileSize = { 0, };
	LARGE_INTEGER offset = { 0, };

	PUCHAR buffer = (PUCHAR)malloc(4096);

	RFi.FIOpenForRead(L"big.txt");
	WFi.FIOCreateFile(L"big3.txt", RFi.mFileSize);

	while (RFi.FIOReadFromFile(offset,4096,buffer))
	{
		WFi.FIOWriteToFile(offset, 4096, buffer);
		offset.QuadPart += 4096;
	}

	RFi.FIOClose();
	WFi.FIOClose();
	sw2.Stop();
	printf("mmio : %f\n", sw2.GetDurationMilliSecond());
	return 0;
}
	


