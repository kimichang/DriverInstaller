// DriverInstaller.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <winerror.h>

#define DRIVER_NAME L"FileTest"
#define DRIVER_PATH L"FileTest.sys"

BOOL InstallNTDriver(wchar_t* lpszDriverName, wchar_t* lpszDriverPath)
{
	wchar_t szDriverImagePath[256];
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		printf("OpenSCManager() failed %d \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenSCManager() successfully\n");
	}

	hServiceDDK = CreateService(hServiceMgr, lpszDriverName, lpszDriverName, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, szDriverImagePath, NULL, NULL, NULL, NULL, NULL);

	DWORD dwRtn;
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			printf("CreateService() failed %d \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			printf("CreateService() failed ! Service is ERROR_IO_PENDING or ERROR_IO_EXISTS!\n");

		}

		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			dwRtn = GetLastError();
			printf("OpenService() fialed %d \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			printf("OpenService() successfully\n");

		}
	}
	else
	{
		printf("CreateService() successfully\n");
	}

	bRet = StartService(hServiceDDK, NULL, NULL);
	DWORD err = GetLastError();
	HRESULT result = HRESULT_FROM_WIN32(err);

	if (!bRet)
	{
		DWORD wdRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService failed %d\n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				printf("StartService failed ERROR_IO_PENDING\n");
				bRet = FALSE;
				goto BeforeLeave;

			}
			else
			{
				printf("StartService failed ERROR_IO_ALREADY_RUNNNING\n");
				bRet = TRUE;
				goto BeforeLeave;
			}
		}
	}

	bRet = TRUE;
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);

	}
	return bRet;

}


BOOL UnInstallNTDriver(wchar_t * szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;

	SERVICE_STATUS SvrSta;
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		printf("OpenSCManager() failed %d\n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenSCManager() successfully\n");

	}

	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		printf("OpenService() failed %d\n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenService() successfully\n");

	}
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
	{
		printf("ControlService() failed %d\n",GetLastError());
	}
	else
	{
		printf("ControlService() successfully\n");
	}

	if (!DeleteService(hServiceDDK))
	{
		printf("DeleteService() failed %d", GetLastError());

	}
	else
	{
		printf("DeleteService() successfully\n");
	}
	bRet = TRUE;
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}


void TestDriver()
{
	HANDLE hDevice = CreateFile(L"\\\\.\\HelloDDK", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		printf("Create Device successfully\n");

	}
	else
	{
		printf("Create Device failed %d\n", GetLastError());
	}
	CloseHandle(hDevice);
}

int main(int argc,char* argv[])
{
	BOOL bRet = InstallNTDriver((wchar_t*)DRIVER_NAME, (wchar_t*)DRIVER_PATH);
	if (!bRet)
	{
		printf("InstallNTDriver error\n");
		return 0;
	}

	printf("Press any to create device\n");
	_getch();
	TestDriver();
	printf("Press any to unload the driver\n");
	_getch();
	UnInstallNTDriver((wchar_t*)DRIVER_NAME);
	if (!bRet)
	{
		printf("UninstallNRDriver error\n");
		return 0;
	}
    return 0;
}

