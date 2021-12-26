#include <windows.h>
#include <iostream>

using namespace std;

void WINAPI ReadCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	cout << "Information received" << endl;
}

int main()
{
	CHAR mes[512];
	BOOL b = FALSE;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");
	HANDLE EVENT = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE PIPE = CreateFile(lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	int x;
	OVERLAPPED overlapped = OVERLAPPED();

	if (EVENT != INVALID_HANDLE_VALUE && PIPE != INVALID_HANDLE_VALUE)
	{
		do
		{
			cout << "1. Accept message" << endl;
			cout << "2. Detach from a named pipe" << endl;
			cout << "0. Exit" << endl;
			cin >> x;
			switch (x) {

			case 1:
				
				overlapped.hEvent = EVENT;
				b = ReadFileEx(PIPE, (LPVOID)mes, 512, &overlapped, ReadCallback);
                SleepEx(-1, TRUE);
				if (b) {
                    cout << mes << endl;
                }
				else
					cout << "Reading failed" << endl;
				break;

			case 2:
				b = CloseHandle(PIPE);
				if (b) cout << "You have been disconnected from a named pipe" << endl;
				else cout << "Failed to disconnect" << endl;
				b = FALSE;
				break;
			case 0:
				break;
			default:
				break;
			}
		} while (x != 0);
	}
	else
		cout << "Failed to create named pipe, restart the program" << endl;

	return 0;
}