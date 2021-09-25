#include <iostream>
#include <cmath>
#include <Windows.h>
using namespace std;
// глобальная переменная, хранящая количество вызовов окончания операций ввода-вывода
int callback = 0;
// хэндлы на файлы, учавствующие в операции копирования
HANDLE firstHandle;
HANDLE secondHandle;
// должны оставаться допустимыми для длительной операции чтения.
OVERLAPPED* over_1;
OVERLAPPED* over_2;

// Функция завершения, которая будет вызываться всякий раз при завершении операции ввода-вывода
VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // количество вызовов
    callback++;
}
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

            delete[] over_1;
            delete[] over_2;
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

void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount) {
    int fileSize = GetFileSize(firstHandle, NULL);
    int firstSize = fileSize;
    int operation_counter = 0; // Счетчик проделанных операций
    CHAR* buffer = new CHAR[fileSize + (fileSize % blockSize > 0 ? 1 : 0) * blockSize];
    CHAR* buffer2 = new CHAR[fileSize + (fileSize % blockSize > 0 ? 1 : 0) * blockSize];
    //
    over_1 = new OVERLAPPED[operationsCount];
    over_2 = new OVERLAPPED[operationsCount];
    for (int i = 0; i < operationsCount; i++)
    {
        over_1[i].Offset = 0;
        over_1[i].OffsetHigh = 0;
        over_1[i].hEvent = NULL;

        over_2[i].Offset = 0;
        over_2[i].OffsetHigh = 0;
        over_2[i].hEvent = NULL;
    }
    buffer2 = buffer;
    bool inFlag = false;
    bool outFlag = false;
    do
    {
        callback = 0;
        operation_counter = 0;

        // ReadFileEx()
        for (int i = 0; i < operationsCount; i++)
        {
            if (fileSize > 0)
            {
                operation_counter++;
                if (i || inFlag)
                {
                    for (int j = 0; j < operationsCount; j++)
                        over_1[j].Offset += blockSize;
                    buffer += blockSize;
                }
                inFlag = true;
                // firstHandle - дескриптор файла или устройства ввода-вывода с FILE_FLAG_OVERLAPPED
                ReadFileEx(firstHandle, buffer, blockSize, &over_1[i], FileIOCompletionRoutine);
                fileSize -= blockSize;
            }
        }
        // поток, запустивший операции чтения/записи должен
        // приостановить себя, например, с помощью функций Sleep () и SleepEx(),
        // и предоставить возможность выполнения функции завершения.
        while (callback < operation_counter)
            SleepEx(-1, TRUE);
        // Функция приостанавливает выполнения потока до наступления события ввода/вывода или на время.
        // -1 - запрещаем планировать поток

        callback = 0;

        // WriteFileEx()
        for (int i = 0; i < operation_counter; i++)
        {
            if (i || outFlag)
            {
                for (int j = 0; j < operationsCount; j++)
                    over_2[j].Offset += blockSize;
                buffer2 += blockSize;
            }
            outFlag = true;
            WriteFileEx(secondHandle, buffer2, blockSize, &over_2[i], FileIOCompletionRoutine);
        }
        while (callback < operation_counter)
            SleepEx(-1, TRUE);
    } while (fileSize > 0);
    SetFilePointer(secondHandle, firstSize, NULL, FILE_BEGIN);
    SetEndOfFile(secondHandle);
}
