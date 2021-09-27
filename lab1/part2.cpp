#include <iostream>
#include <cmath>
#include <Windows.h>
using namespace std;
// глобальная переменная, хранящая количество вызовов окончания операций ввода-вывода
int callback = 0;
// хэндлы на файлы, учавствующие в операции копирования
HANDLE firstHandle;
HANDLE secondHandle;

// Функция завершения, которая будет вызываться всякий раз при завершении операции ввода-вывода
VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // количество вызовов
    callback++;
}
void ReadFileOverlapped(DWORD fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
void WriteFileOverlapped(DWORD fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
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
        overlappeds[i].Offset += blockSize*operationsCount;
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
        overlappeds[i].Offset += blockSize*operationsCount;
    }
    callback = 0;
}

void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount) {
    DWORD high = 0;
    long long fileSize = GetFileSize(firstHandle, &high);
    long long curSize = fileSize;
    CHAR** buffer = new CHAR*[operationsCount];
    // должны оставаться допустимыми для длительной операции чтения.
    OVERLAPPED* over_1 = new OVERLAPPED[operationsCount];
    OVERLAPPED* over_2 = new OVERLAPPED[operationsCount];

    for (int i=0; i < operationsCount; i++)
    {
        buffer[i] = new CHAR[(int)blockSize];

        over_1[i].Offset = over_2[i].Offset = i*(int)blockSize;
        over_1[i].OffsetHigh = over_2[i].OffsetHigh = i*(int)high;
        over_1[i].hEvent = over_2[i].hEvent = NULL;
    }

    do
    {
        ReadFileOverlapped(fileSize, blockSize, operationsCount, over_1, buffer, firstHandle);
        WriteFileOverlapped(fileSize, blockSize, operationsCount, over_2, buffer, secondHandle);
        curSize -= (long long)(blockSize*operationsCount);
    } while (curSize > 0);

    SetFilePointer(secondHandle, fileSize, NULL, FILE_BEGIN);
    SetEndOfFile(secondHandle);
}
