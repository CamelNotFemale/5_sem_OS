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
    std::vector<HANDLE> pageMutex;
    DWORD waitResult;

    std::srand(std::time(nullptr));

    HANDLE logOut = GetStdHandle(STD_OUTPUT_HANDLE);

    LONG semPrev = -1;
    DWORD written = 0;
    std::string outputString = "";

    const char mapName[] = "MAPPING";
    HANDLE mapHandle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, (LPCTSTR)mapName);
    if (mapHandle)
    {
        for (int i = 0; i < 3; ++i)
        {
            page = -1;

            // Ожидание любой занятой страницы
            outputString = "WAIT USED SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            WaitForSingleObject(usedSem, INFINITE);

            outputString = "TAKE USED SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            
            // ищем конкретную занятую страницу
            /*do {
                page += 1;
                mutexName.back() = '0' + page;
                mutex = OpenMutexA(SYNCHRONIZE, false, mutexName.c_str());
                waitResult = WaitForSingleObject(mutex, 0);
            } while (waitResult == WAIT_TIMEOUT);*/
            // получаем список мьютексов страниц буфферной памяти
            for (page = 0; page < 16; page++)
            {
                mutexName.back() = '0' + page;
                mutex = OpenMutexA(SYNCHRONIZE, false, mutexName.c_str());
                pageMutex.push_back(mutex);
            }
            waitResult = WaitForMultipleObjects(pageMutex.size(), &pageMutex[0], false, INFINITE);

            outputString = "TAKE MUTEX: " + std::to_string(GetTickCount()) + "\n";
            WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);            

            Sleep(std::rand() % 1000 + 500);

            // Освобождаем мьютекс

            if (ReleaseMutex(pageMutex[waitResult]))
            {
                outputString = "FREE MUTEX: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }
            else
            {
                outputString = "CODE MUTEX: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }

            // Освобождаем семафор

            if (ReleaseSemaphore(freeSem, 1, &semPrev))
            {
                outputString = "FREE FREE SEMAPHORE: " + std::to_string(GetTickCount()) + "\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);

                outputString = "PAGE NUMBER = " + std::to_string(page + 1) + ": " + std::to_string(GetTickCount()) + "\n\n";
                WriteFile(logOut, outputString.data(), outputString.length(), &written, NULL);
            }
            else
            {
                std::string errorString = std::to_string(GetLastError()) + " CODE SEMAPHORE\n";
                WriteFile(logOut, errorString.data(), errorString.length(), &written, NULL);
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