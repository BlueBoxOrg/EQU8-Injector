#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

const char* myDll = "C:\\Users\\jacks\\Desktop\\fuckswedish\\BEClient.dll";

DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		//loop through all process
		if (Process32First(hSnap, &procEntry))
		{

			do
			{
				//compare current lopping process name with procName parameters
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	//close handle and return the procId of the process
	CloseHandle(hSnap);
	return procId;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
	DWORD procId = GetProcId(L"TotallyAccurateBattlegrounds.exe");
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(hProc, loc, myDll, strlen(myDll) + 1, 0);
		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
		if (hThread)
			CloseHandle(hThread);
	}
	CloseHandle(hProc);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

