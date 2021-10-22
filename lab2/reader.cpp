#include <iostream>
#include <windows.h>
#include <string>
#include <conio.h>

using namespace std;

int main()
{
    system("chcp 1251");
    system("cls");

    wstring nameFile;
    HANDLE FileMap = NULL;
    LPCWSTR  lpFileName = L"";
    LPVOID lpFileMap = NULL;
    string fileInput;

    cout << "������� ��� �����������:" << endl;
    cout << "> ";
    wcin >> nameFile;

    lpFileName = nameFile.c_str();

    FileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, (LPCTSTR)lpFileName);

    if (FileMap != NULL)
    {
        lpFileMap = MapViewOfFile(FileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (lpFileMap == 0) {
            cout << "� �������� ������ ���, ������� ��������� ���������� writer\n";
            system("pause");
            return 1;
        }

        cout << "�� ������ " << lpFileMap << " �������� ��������� ������:" << endl;
        cout << "> ";

        cout << (char*)lpFileMap << endl << endl;

        cout << "������ ����� �������� ���������.";

        _getch();

    }
    else {
        cout << "������ �������� ��������.\n";
    }

    UnmapViewOfFile(lpFileMap);
    CloseHandle(FileMap);

    return 0;
}
