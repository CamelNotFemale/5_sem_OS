#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <Windows.h>
#include <vector>

int main()
{
    HANDLE freeSem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, (LPCTSTR)"freeSem");
    HANDLE usedSem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, (LPCTSTR)"usedSem");
    
    HANDLE mutex;
    int page;
    std::string mutexName = "mutex_*";
    DWORD waitResult;

    std::srand(std::time(nullptr));

    HANDLE logOut = GetStdHandle(STD_OUTPUT_HANDLE); // извлекает дескриптор устройства стандартного вывода

    LONG semPrev = -1;
    DWORD written = 0;
    std::string outputString = "";

    const char mapName[] = "MAPPING";
    HANDLE mappingHandle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, (LPCTSTR)mapName);
    if (mappingHandle)
    {
        for (int i = 0; i < 3; ++i)
        {
            page = -1;

            //! Ожидание любой свободной страницы
            outputString = "WAIT FREE SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            WaitForSingleObject(freeSem, INFINITE);
            
            outputString = "TAKE SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);

            // ищем конкретную свободную страницу для записи
            do {
                page += 1;
                mutexName.back() = '0' + page;
                mutex = OpenMutexA(SYNCHRONIZE, false, mutexName.c_str());
                waitResult = WaitForSingleObject(mutex, 0);
            } while (waitResult == WAIT_TIMEOUT);

            outputString = "TAKE MUTEX: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);

            Sleep(std::rand() % 1000 + 500);

            if (ReleaseMutex(mutex))
            {
                outputString = "FREE MUTEX: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }
            else
            {
                outputString = "CODE MUTEX: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }

            if (ReleaseSemaphore(usedSem, 1, &semPrev))
            {
                outputString = "FREE USED SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);

                outputString = "PAGE NUMBER = " + std::to_string(page + 1) + ": " + std::to_string(GetTickCount()) + "\n\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }
            else
            {
                std::string str = std::to_string(GetLastError()) + " CODE SEMAPHORE\n";
                WriteFile(logOut, str.data(), str.length(), &written, NULL);
            }
        }
    }
    else
    {
        WriteFile(logOut, "Mapping creation failed\n", strlen("Mapping creation failed\n"), &written, NULL);
    }

    CloseHandle(logOut);

    return 0;
}