#include <iostream>
#include <cmath>
#define _WIN32_WINNT 0x0501
#include <windows.h>
using namespace std;
// ���������� ����������, �������� ���������� ������� ��������� �������� �����-������
int callback = 0;
// ������ �� �����, ������������ � �������� �����������
HANDLE firstHandle;
HANDLE secondHandle;
// ������� ����� ��������� ����������� ����� ��� �������� ������ � ������ ��������������
LARGE_INTEGER shiftRead;
LARGE_INTEGER shiftWrite;

// ������� ����������, ������� ����� ���������� ������ ��� ��� ���������� �������� �����-������
VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // ���������� �������
    callback++;
}
void ReadFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
void WriteFileOverlapped(long long fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
// ������� ������������ �����������
void copyFile(HANDLE firstHandle, HANDLE secondHandle, DWORD blockSize, int operationsCount);

int main()
{
    system("chcp 1251");

    int part = 0, operations = 0, sizeToCopy;

    // ���� ��������� ��� ������� �����
    cout << "������� ������ ����� ������-������:\n";
    do {
        cout << "> 4096*";
        cin >> part;
    } while (part <= 0);
    //���� ���������� ��������
    cout << "������� ���������� ��������:\n1 2 4 8 12 16\n";
    do {
        cout << "> ";
        cin >> operations;
    } while (operations <= 0);
    //���� ������������ ������� �����
    cout << "������� ���� �� ������� �����:" << endl;
    cout << "> ";
    string firstFileName;
    cin >> firstFileName;
    //���� ������������ ������� �����
    cout << "������� ���� �� ������� �����:" << endl;
    cout << "> ";
    string secondFileName;
    cin >> secondFileName;
    if (firstFileName.length() < MAX_PATH)
    {
        //�������� ���������� � ��������� ������
        firstHandle = CreateFile(firstFileName.data(), GENERIC_READ, 0, NULL,OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
        secondHandle = CreateFile(secondFileName.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);

        if (firstHandle != INVALID_HANDLE_VALUE)
        {
            sizeToCopy = 4096 * part;

            //������ ������ ������� � �����������
            DWORD startTime = GetTickCount();
            copyFile(firstHandle, secondHandle, sizeToCopy, operations);
            DWORD endTime = GetTickCount();
            //����� ������ ������� � �����������
            cout << endl << "����� �����������: " << endTime - startTime << endl;
        }
        else
        {
            if (GetLastError())
            cout << endl << "������: " << GetLastError() << endl;
        }
    }
    //�������� Handle'��
    if (!CloseHandle(firstHandle))
        cout << "������: �� ������� ������� Handle ������� �����!" << endl;
    if (!CloseHandle(secondHandle))
        cout << "������: �� ������� ������� Handle ������� �����!" << endl;
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
    // ������ ���������� ����������� ��� ���������� �������� ������.
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
