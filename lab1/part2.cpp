#include <iostream>
#include <cmath>
#include <Windows.h>
using namespace std;
// ���������� ����������, �������� ���������� ������� ��������� �������� �����-������
int callback = 0;
// ������ �� �����, ������������ � �������� �����������
HANDLE firstHandle;
HANDLE secondHandle;

// ������� ����������, ������� ����� ���������� ������ ��� ��� ���������� �������� �����-������
VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    // ���������� �������
    callback++;
}
void ReadFileOverlapped(DWORD fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
void WriteFileOverlapped(DWORD fileSize, DWORD blockSize, int operationsCount, OVERLAPPED* overlappeds, CHAR** buffer, HANDLE fileHandle);
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
    // ������ ���������� ����������� ��� ���������� �������� ������.
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
