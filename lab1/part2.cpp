#include <iostream>
#include <Windows.h>
using namespace std;

int callback = 0;
int operations;
int sizeToCopy;
HANDLE firstHandle;
HANDLE secondHandle;
OVERLAPPED* over_1;
OVERLAPPED* over_2;
// Функция завершения, которая будет вызываться всякий раз при завершении операци и ввода-вывода
// dwErrorCode – состояние завершения вводавывода, значения параметра ограничены 0 (успешное завершение) и ERROR_HANDLE_EOF (при попытке выполнить чтение с выходом за пределы файла);
// dwNumberOfBytesTransfered – переданное число байтов;
// lpOverlapped – структура, которая использовалась завершившимся вызовом ReadFileEx () или WriteFileEx ()

VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // Отслеживаем количество вызовов
    callback++;
}
//Функция копирования
void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount) {
    int fileSize = GetFileSize(firstHandle, NULL);
    int firstSize = fileSize;
    int countOperations = 0; // Счетчик проделанных операций
    CHAR* buffer = new CHAR[fileSize + (fileSize % blockSize > 0 ? 1 : 0) * blockSize];
    CHAR* buffer2 = new CHAR[fileSize + (fileSize % blockSize > 0 ? 1 : 0) * blockSize];
    //
    OVERLAPPED* over_1 = new OVERLAPPED[operationsCount];
    OVERLAPPED* over_2 = new OVERLAPPED[operationsCount];
    for (int i = 0; i < operationsCount; i++)
    {
        over_1[i].Offset = 0;
        over_1[i].OffsetHigh = 0;
        // Вовсе игнорируется
        over_1[i].hEvent = NULL;
        over_2[i].Offset = 0;
        over_2[i].OffsetHigh = 0;
        // Вовсе игнорируется
        over_2[i].hEvent = NULL;
    }
    buffer2 = buffer;
    bool inFlag = false;
    bool outFlag = false;
    do
    {
        callback = 0;
        countOperations = 0;

        // ReadFileEx()
        for (int i = 0; i < operationsCount; i++)
        {
            if (fileSize > 0)
            {
                countOperations++;
                if (i || inFlag)
                {
                    for (int j = 0; j < operationsCount; j++)
                    over_1[j].Offset += blockSize;
                    buffer += blockSize;
                }
                inFlag = true;
                // firstHandle - дескриптор файла или устройства вводавывода с FILE_FLAG_OVERLAPPED
                //
                ReadFileEx(firstHandle, buffer, blockSize, &over_1[i], FileIOCompletionRoutine);
                fileSize -= blockSize;
            }
        }
        // Функции завершения выполняются в том же
        // потоке что и функции файлового ввода/вывода. Это значит,
        // что поток, запустивший операции чтения/записи должен
        // приостановить себя, например, с помощью функций Sleep () и SleepEx
        // (), и предоставить возможность выполнения функции завершения.
        while (callback < countOperations)
            SleepEx(-1, TRUE);
        // Функция приостанавливает выполнения потока до наступления события ввода/вывода или на время.
        // -1 - запрещаем планировать поток

        callback = 0;

        // WriteFileEx()
        for (int i = 0; i < countOperations; i++)
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
        while (callback < countOperations)
        SleepEx(-1, TRUE);
    } while (fileSize > 0);
    SetFilePointer(secondHandle, firstSize, NULL, FILE_BEGIN);
    SetEndOfFile(secondHandle);
}
int main()
{
    int number = 0;
    int part = 0, operations = 0;
    const DWORD directoryBufferSize = MAX_PATH;
    char directoryBuffer[directoryBufferSize];
    const DWORD directoryOut = GetCurrentDirectory(directoryBufferSize, directoryBuffer);

    if (directoryOut > 0 && directoryOut <= directoryBufferSize)
    {
        //Ввод множителя для размера блока
        cout << "Enter size of the block:\n";
        cout << "> 4096*";
        do {
            cin >> part;
        } while (part == 0);
        //Ввод количества операций
        cout << "Enter number of the operations:\n1 2 4 8 12\n";
        cout << "> ";
        do {
            cin >> operations;
        } while (operations == 0);
        //Ввод расположения первого файла
        cout << "Enter the directory of first file:" << endl;
        string firstFileName;
        cin >> firstFileName;
        //Ввод расположения второго файла
        cout << "Enter the directory of second file:" << endl;
        string secondFileName;
        cin >> secondFileName;
        if (firstFileName.length() < MAX_PATH)
        {
            //Создание начального и конечного файлов
            firstHandle = CreateFile(firstFileName.data(), GENERIC_READ, 0, NULL,
            OPEN_EXISTING,
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
            secondHandle = CreateFile(secondFileName.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,

            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
            if (firstHandle != INVALID_HANDLE_VALUE)
            {
                sizeToCopy = 4096 * part;

                //Начало замера времени и копирования
                DWORD startTime = GetTickCount();
                copyFile(firstHandle, secondHandle, sizeToCopy, operations);
                DWORD endTime = GetTickCount();
                //Конец замера времени и копирования
                cout << endl << "Time to copy: " << endTime - startTime << endl;
                delete[] over_1, over_2;
            }
            else
            {
                if (GetLastError())
                cout << endl << "ERROR: " << GetLastError() << endl;
            }
        }
        //Закрытие Handle'ов
        if (!CloseHandle(firstHandle))
            cout << "ERROR: Could not close the handle!" << endl;
        if (!CloseHandle(secondHandle))
            cout << "ERROR: Could not close the handle!" << endl;
    }
    else
    {
        cout << "Too long name!";
    }
}
