#include <iostream>
#include <cmath>
#define _WIN32_WINNT 0x0501
#include <windows.h>
using namespace std;
// глобальная переменная, хранящая количество вызовов окончания операций ввода-вывода
int callback = 0;
// хэндлы на файлы, учавствующие в операции копирования
HANDLE firstHandle;
HANDLE secondHandle;
// текущий сдвиг состояния копирования файла для операций чтения и записи соответственно
LARGE_INTEGER shiftRead;
LARGE_INTEGER shiftWrite;

// Функция завершения, которая будет вызываться всякий раз при завершении операции ввода-вывода
VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // количество вызовов
    callback++;
}
void ReadFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
void WriteFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
// Функция асинхронного копирования
void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount);

int main()
{
    system("chcp 1251");

    int part = 0, operations = 0, sizeToCopy;

    // Ввод множителя для размера блока
    cout << "Введите размер блока чтения-записи:\n";
    do {
        cout << "> 4096*";
        cin >> part;
    } while (part <= 0);
    //Ввод количества операций
    cout << "Введите количество операций:\n1 2 4 8 12 16\n";
    do {
        cout << "> ";
        cin >> operations;
    } while (operations <= 0);
    //Ввод расположения первого файла
    cout << "Введите путь до первого файла:" << endl;
    cout << "> ";
    string firstFileName;
    cin >> firstFileName;
    //Ввод расположения второго файла
    cout << "Введите путь до второго файла:" << endl;
    cout << "> ";
    string secondFileName;
    cin >> secondFileName;
    if (firstFileName.length() < MAX_PATH)
    {
        //Создание начального и конечного файлов
        firstHandle = CreateFile(firstFileName.data(), GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
        secondHandle = CreateFile(secondFileName.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);

        if (firstHandle != INVALID_HANDLE_VALUE)
        {
            sizeToCopy = 4096 * part;

            //Начало замера времени и копирования
            DWORD startTime = GetTickCount();
            copyFile(firstHandle, secondHandle, sizeToCopy, operations);
            DWORD endTime = GetTickCount();
            //Конец замера времени и копирования
            cout << endl << "Время копирования: " << endTime - startTime << endl;
        }
        else
        {
            if (GetLastError())
            cout << endl << "Ошибка: " << GetLastError() << endl;
        }
    }
    //Закрытие Handle'ов
    if (!CloseHandle(firstHandle))
        cout << "Ошибка: Не удалось закрыть Handle первого файла!" << endl;
    if (!CloseHandle(secondHandle))
        cout << "Ошибка: Не удалось закрыть Handle второго файла!" << endl;
}

void ReadFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle)
{
    int operations_counter = 0;
    for (int i=0; i<operationsCount; i++)
    {
        if (fileSize>0)
        {
            operations_counter++;
            ReadFileEx(fileHandle, buffer[i], blockSize, &overlappeds[i], FileIOCompletionRoutine);
            fileSize -= blockSize;
        }
    }
    while (callback < operations_counter)
        SleepEx(-1, true);
    for (int i=0; i<operationsCount; i++)
    {
        overlappeds[i].Offset = shiftRead.LowPart;
        overlappeds[i].OffsetHigh = shiftRead.HighPart;
        shiftRead.QuadPart += blockSize;
    }
    callback = 0;
}

void WriteFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle)
{
    int operations_counter = 0;
    for (int i=0; i<operationsCount; i++)
    {
        if (fileSize>0)
        {
            operations_counter++;
            WriteFileEx(fileHandle, buffer[i], blockSize, &overlappeds[i], FileIOCompletionRoutine);
            fileSize -= blockSize;
        }
    }
    while (callback < operations_counter)
        SleepEx(-1, true);
    for (int i=0; i<operationsCount; i++)
    {
        overlappeds[i].Offset = shiftWrite.LowPart;
        overlappeds[i].OffsetHigh = shiftWrite.HighPart;
        shiftWrite.QuadPart += blockSize;
    }
    callback = 0;
}

void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount) {
    DWORD high = 0;
    LARGE_INTEGER fileSizeStruct;
    long long fileSize;
    GetFileSizeEx(firstHandle, &fileSizeStruct);
    fileSize = fileSizeStruct.QuadPart;
    long long curSize = fileSize;

    CHAR** buffer = new CHAR*[operationsCount];
    for (int i=0; i < operationsCount; i++)
    {
        buffer[i] = new CHAR[(int)blockSize];
    }
    // должны оставаться допустимыми для длительной операции чтения.
    OVERLAPPED* over_1 = new OVERLAPPED[operationsCount];
    OVERLAPPED* over_2 = new OVERLAPPED[operationsCount];

    shiftRead.QuadPart = 0;
    shiftWrite.QuadPart = 0;
    for (int i=0; i < operationsCount; i++)
    {
        over_1[i].Offset = over_2[i].Offset = shiftRead.LowPart;
        over_1[i].OffsetHigh = over_2[i].OffsetHigh = shiftRead.HighPart;
        over_1[i].hEvent = over_2[i].hEvent = NULL;
        shiftRead.QuadPart += blockSize;
        shiftWrite.QuadPart += blockSize;
    }

    do
    {
        ReadFileOverlapped(curSize, blockSize, operationsCount, over_1, buffer, firstHandle);
        WriteFileOverlapped(curSize, blockSize, operationsCount, over_2, buffer, secondHandle);
        curSize -= (long long)(blockSize*operationsCount);
    } while (curSize > 0);

    SetFilePointerEx(secondHandle, fileSizeStruct, NULL, FILE_BEGIN);
    SetEndOfFile(secondHandle);
}
