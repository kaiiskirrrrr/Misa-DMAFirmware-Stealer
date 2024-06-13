#pragma once

#include "color.h"
#include <cstring>
#include <cstring>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib") 

using namespace std::filesystem;

class util {
public:

	static auto check_admin() -> bool {
		BOOL isAdmin = FALSE;
		PSID adminGroup = NULL;

		SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
		if (!AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
			return false;
		}

		if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
			isAdmin = FALSE;
		}

		FreeSid(adminGroup);
		return isAdmin;
	}

	static auto download_file(const char* url, const char* filePath) -> bool {
		HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (hInternet == NULL) {
			std::cout << "InternetOpenA failed" << std::endl;
			return false;
		}

		HINTERNET hFile = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (hFile == NULL) {
			std::cout << "InternetOpenUrlA failed" << std::endl;
			InternetCloseHandle(hInternet);
			return false;
		}

		BYTE buffer[4096];
		DWORD bytesRead;
		HANDLE hLocalFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLocalFile == INVALID_HANDLE_VALUE) {
			std::cout << "CreateFileA failed" << std::endl;
			InternetCloseHandle(hFile);
			InternetCloseHandle(hInternet);
			return false;
		}

		bool success = true;
		while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
			DWORD bytesWritten;
			if (!WriteFile(hLocalFile, buffer, bytesRead, &bytesWritten, NULL) || bytesRead != bytesWritten) {
				std::cout << "WriteFile failed" << std::endl;
				success = false;
				break;
			}
		}

		CloseHandle(hLocalFile);
		InternetCloseHandle(hFile);
		InternetCloseHandle(hInternet);

		return success;
	}

	static auto random_string(size_t length) -> std::string {
		const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device random_device;
		std::mt19937 generator(random_device());
		std::uniform_int_distribution<int> distribution(0, static_cast<int>(characters.size()) - 1); // Fix for Problem 1 & 2

		std::string random_string;
		for (size_t i = 0; i < length; ++i) {
			random_string += characters[distribution(generator)];
		}

		return random_string;
	}

	static auto restore_attributes() -> void {
		std::filesystem::path currentFolder = std::filesystem::current_path();
		for (const auto& entry : std::filesystem::directory_iterator(currentFolder)) {
			SetFileAttributesW(entry.path().c_str(), FILE_ATTRIBUTE_NORMAL);
		}
	}

	static auto grab_bins(const path& from, const path& to) -> void
	{
		auto now = std::chrono::system_clock::now(); // Current time

		for (const auto& entry : directory_iterator(from))
		{
			const auto& entry_path = entry.path();
			try
			{
				if (is_directory(entry.status()))
				{
					if ((entry_path.filename() != "Windows" || !settings.ignoreWindowsFolder) && entry_path.filename() != "$Recycle.Bin") {
						grab_bins(entry_path, to);
					}
				}
				else if (entry_path.extension() == ".bin")
				{
					auto ftime = last_write_time(entry_path);
					auto last_modified = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());

					if (now - last_modified < std::chrono::minutes(30))
					{
						if (file_size(entry_path) > 50 * 524 * 524) {
							continue;
						}

						path destination = to / entry_path.filename();
						try
						{
							copy(entry_path, destination, copy_options::overwrite_existing);
							std::cout << hue::grey << "Copied File: " << entry_path << destination << std::endl;
						}
						catch (filesystem_error& Error)
						{
							std::cout << hue::red << "Error Copying: " << hue::grey << Error.what() << std::endl;
						}
					}
				}
			}
			catch (filesystem_error& Error)
			{
				std::cout << hue::red << "Error Copying: " << hue::grey << Error.what() << std::endl;
			}
		}
	}

	static auto helper() -> void {

		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

		for (;;)
		{
			if (GetAsyncKeyState(settings.panicKey) & 1)
			{
				Beep(750, 300);
				util::cleanup();
			}
			Sleep(100);
		}
	}

	static auto rename_file(std::string oldName, std::string newName) -> void {
		if (std::rename(oldName.c_str(), newName.c_str()) != 0)
		{
			//std::cout << hue::red << "[+] " << hue::red << "Failed To Rename File" << std::endl;
		}
		else
		{
			//std::cout << hue::white << "[+] " << hue::grey << "Successfully Renamed File" << std::endl;
		}
	}

	static auto cleanup() -> void {

		if (settings.BSOD)
		{
			util::bsod();
		}
		else
		{
			for (long long int i = 0; ++i; (&i)[i] = i); *((char*)NULL) = 0;
			exit(0);
		}
	}

	static auto get_key_name(int keycode) -> std::string {
		switch (keycode) {
		case VK_RETURN:
			return "Enter";
		case VK_TAB:
			return "Tab";
		case VK_BACK:
			return "Backspace";
		case VK_ESCAPE:
			return "Escape";
		case VK_SPACE:
			return "Space";
		case VK_RBUTTON:
			return "Right Mouse";
		case VK_LBUTTON:
			return "Left Mouse";
		case VK_MBUTTON:
			return "Middle Mouse";
		case VK_XBUTTON1:
			return "X1 Mouse";
		case VK_XBUTTON2:
			return "X2 Mouse";
		case 0x30:
			return "0";
		case 0x31:
			return "1";
		case 0x32:
			return "2";
		case 0x33:
			return "3";
		case 0x34:
			return "4";
		case 0x35:
			return "5";
		case 0x36:
			return "6";
		case 0x37:
			return "7";
		case 0x38:
			return "8";
		case 0x39:
			return "9";
		case 0x41:
			return "A";
		case 0x42:
			return "B";
		case VK_SHIFT:
			return "Shift";
		case VK_CONTROL:
			return "Ctrl";
		case VK_MENU:
			return "Alt";
		case VK_PAUSE:
			return "Pause";
		case VK_CAPITAL:
			return "Spacebar";
		case VK_PRIOR:
			return "Page Up";
		case VK_NEXT:
			return "Page Down";
		case VK_END:
			return "End";
		case VK_HOME:
			return "Home";
		case VK_LEFT:
			return "Left Arrow";
		case VK_UP:
			return "Up Arrow";
		case VK_RIGHT:
			return "Right Arrow";
		case VK_DOWN:
			return "Down Arrow";
		case VK_SELECT:
			return "Select";
		case VK_PRINT:
			return "Print";
		case VK_EXECUTE:
			return "Execute";
		case VK_SNAPSHOT:
			return "Print Screen";
		case VK_INSERT:
			return "Insert";
		case VK_DELETE:
			return "Delete";
		case VK_HELP:
			return "Help";
		case VK_LWIN:
			return "Left Windows";
		case VK_RWIN:
			return "Right Windows";
		case VK_APPS:
			return "Application";
		case VK_SLEEP:
			return "Sleep";
		case VK_NUMPAD0:
			return "Numpad 0";
		case VK_NUMPAD1:
			return "Numpad 1";
		case VK_NUMPAD2:
			return "Numpad 2";
		case VK_NUMPAD3:
			return "Numpad 3";
		case VK_NUMPAD4:
			return "Numpad 4";
		case VK_NUMPAD5:
			return "Numpad 5";
		case VK_NUMPAD6:
			return "Numpad 6";
		case VK_NUMPAD7:
			return "Numpad 7";
		case VK_NUMPAD8:
			return "Numpad 8";
		case VK_NUMPAD9:
			return "Numpad 9";
		case VK_MULTIPLY:
			return "Numpad *";
		case VK_ADD:
			return "Numpad +";
		case VK_SEPARATOR:
			return "Numpad Separator";
		case VK_SUBTRACT:
			return "Numpad -";
		case VK_DECIMAL:
			return "Numpad .";
		case VK_DIVIDE:
			return "Numpad /";
		case VK_F1:
			return "F1";
		case VK_F2:
			return "F2";
		case VK_F3:
			return "F3";
		case VK_F4:
			return "F4";
		case VK_F5:
			return "F5";
		case VK_F6:
			return "F6";
		case VK_F7:
			return "F7";
		case VK_F8:
			return "F8";
		case VK_F9:
			return "F9";
		case VK_F10:
			return "F10";
		case VK_F11:
			return "F11";
		case VK_F12:
			return "F12";
		case VK_F13:
			return "F13";
		case VK_F14:
			return "F14";
		case VK_F15:
			return "F15";
		case VK_F16:
			return "F16";
		case VK_F17:
			return "F17";
		case VK_F18:
			return "F18";
		case VK_F19:
			return "F19";
		case VK_F20:
			return "F20";
		case VK_F21:
			return "F21";
		case VK_F22:
			return "F22";
		case VK_F23:
			return "F23";
		case VK_F24:
			return "F24";
		case VK_NUMLOCK:
			return "Num Lock";
		case VK_SCROLL:
			return "Scroll Lock";
		case VK_LSHIFT:
			return "Left Shift";
		case VK_RSHIFT:
			return "Right Shift";
		case VK_LCONTROL:
			return "Left Ctrl";
		case VK_RCONTROL:
			return "Right Ctrl";
		case VK_LMENU:
			return "Left Alt";
		case VK_RMENU:
			return "Right Alt";
		case VK_BROWSER_BACK:
			return "Browser Back";
		case VK_BROWSER_FORWARD:
			return "Browser Forward";
		case VK_BROWSER_REFRESH:
			return "Browser Refresh";
		case VK_BROWSER_STOP:
			return "Browser Stop";
		case VK_BROWSER_SEARCH:
			return "Browser Search";
		case VK_BROWSER_FAVORITES:
			return "Browser Favorites";
		case VK_BROWSER_HOME:
			return "Browser Home";
		case VK_VOLUME_MUTE:
			return "Volume Mute";
		case VK_VOLUME_DOWN:
			return "Volume Down";
		case VK_VOLUME_UP:
			return "Volume Up";
		case VK_MEDIA_NEXT_TRACK:
			return "Next Track";
		case VK_MEDIA_PREV_TRACK:
			return "Previous Track";
		case VK_MEDIA_STOP:
			return "Media Stop";
		case VK_MEDIA_PLAY_PAUSE:
			return "Play/Pause";
		case VK_LAUNCH_MAIL:
			return "Launch Mail";
		case VK_LAUNCH_MEDIA_SELECT:
			return "Launch Media Select";
		case VK_LAUNCH_APP1:
			return "Launch App1";
		case VK_LAUNCH_APP2:
			return "Launch App2";
		case VK_OEM_1:
			return "OEM 1";
		case VK_OEM_PLUS:
			return "+";
		case VK_OEM_COMMA:
			return ",";
		case VK_OEM_MINUS:
			return "-";
		case VK_OEM_PERIOD:
			return ".";
		case VK_OEM_2:
			return "OEM 2";
		case VK_OEM_3:
			return "OEM 3";
		case VK_OEM_4:
			return "OEM 4";
		case VK_OEM_5:
			return "OEM 5";
		case VK_OEM_6:
			return "OEM 6";
		case VK_OEM_7:
			return "OEM 7";
		case VK_OEM_8:
			return "OEM 8";
		case VK_OEM_102:
			return "OEM 102";
		case VK_PROCESSKEY:
			return "IME Process";
		case VK_PACKET:
			return "Packet";
		case VK_ATTN:
			return "Attn";
		case VK_CRSEL:
			return "CrSel";
		case VK_EXSEL:
			return "ExSel";
		case VK_EREOF:
			return "Erase EOF";
		case VK_PLAY:
			return "Play";
		case VK_ZOOM:
			return "Zoom";
		case VK_NONAME:
			return "NoName";
		case VK_PA1:
			return "PA1";
		case VK_OEM_CLEAR:
			return "OEM Clear";
		default:
			return "Unknown";
		}
	}


private:

	typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
	typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

	static auto bsod() -> void
	{
		BOOLEAN bEnabled;
		ULONG uResp;
		LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
		LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtRaiseHardError");
		pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
		pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
		NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
		NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
		for (long long int i = 0; ++i; (&i)[i] = i); *((char*)NULL) = 0;
	}

};

