#include <iostream>
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <string>

using namespace std;
// ������� ������
void protect_info(DWORD Pro);
// ������� ���������� ������ � ������������ ������� ��������� ������������ ���������(� ����������) ������� ������
void free_virtual_memory();
// ��������� ������ ������� ��� ��������� (� ����������) ������� ������ � �� ��������
void set_virtual_protect();
// ������ ������ � ������ ������ �� �������� � ���������� �������
void input();
// �������������� ������� � �������� ��� ���������� ������ � �������������� ������ � � ������ ����� ������ ������ �������
void memory_reserve_and_commit(SYSTEM_INFO si);
// �������������� ������� � �������������� ������ � � ������ ����� ������ ������ �������
void memory_reserve(SYSTEM_INFO si);
// ����������� ��������� ����������� ������� ������ �� ��������� � ���������� ������
void virtual_query();
// ����������� ������� ����������� ������
void get_global_memory_status();
// �������� ���������� � ���. �������
void get_system_info(SYSTEM_INFO si);

int main()
{
	int c;
	SYSTEM_INFO MSI;
	GetSystemInfo(&MSI);
	system("chcp 1251");

	do {
        system("cls");
		cout << "1. �������� ���������� � ���. �������" << endl;
		cout << "2. ����������� ������� ����������� ������" << endl;
		cout << "3. ����������� ��������� ����������� ������� ������ �� ��������� � ���������� ������" << endl;
		cout << "4. �������������� ������� � �������������� ������ � � ������ ����� ������ ������ �������" << endl;
		cout << "5. �������������� ������� � �������� ��� ���������� ������ � �������������� ������ � � ������ ����� ������ ������ �������" << endl;
		cout << "6. ������ ������ � ������ ������ �� �������� � ���������� �������: " << endl;
		cout << "7. ��������� ������ ������� ��� ��������� (� ����������) ������� ������ � �� ��������" << endl;
		cout << "8. ������� ���������� ������ � ������������ ������� ��������� ������������ ���������(� ����������) ������� ������" << endl;
		cout << "\n0. �����\n" << endl;
		cout << "����>> ";
		cin >> c;
		switch (c)
		{
		case 1:
			get_system_info(MSI);
			break;
		case 2:
			get_global_memory_status();
			break;
		case 3:
			virtual_query();
			break;
		case 4:
			memory_reserve(MSI);
			break;
		case 5:
			memory_reserve_and_commit(MSI);
			break;
		case 6:
			input();
			break;
		case 7:
			set_virtual_protect();
			break;
		case 8:
			free_virtual_memory();
			break;
		case 0:
			break;
		default:
			cout << "������! ������ ������ ���." << endl;
			break;

		}
		system("pause");
	}
	while (c!=0);

	return 0;
}

void get_system_info(SYSTEM_INFO si)
{
    cout << "����������� ����������: ";

    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)    cout << "x64 (AMD ��� INTEL)" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )    cout << "Intel Itanium Processor Family (IPF)" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)    cout << "x86" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)      cout << "ARM" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)  cout << "����������� �����������" << endl;

    cout << "\t������ �������� � ������� ������������� ������ ������� � ������������:                 "   << si.dwPageSize << endl;
    cout << "\t��������� �� ����� ������ ����� ������, ��������� ����������� � �����������:           " << si.lpMinimumApplicationAddress << endl;
    cout << "\t��������� �� ����� ������� ����� ������, ��������� ����������� � �����������:          " << si.lpMaximumApplicationAddress << endl;
    cout << "\t����������, ������������������ � �������:" << endl;
	for (DWORD i = 0; i < si.dwNumberOfProcessors; i++)
	{
		if (si.dwActiveProcessorMask & (1 << i))
			cout << "\t\t��������� " << i<<endl;
	}
    cout << "\t���������� ���������� ����������� � ������� ������:                                    "   << si.dwNumberOfProcessors << endl;
    cout << "\t������������� ���������� ������, �� �������� ����� ���� �������� ����������� ������:   "   << si.dwAllocationGranularity << endl;
    cout << "\t������� ����������:                                                                    "   << si.wProcessorLevel << endl;
    cout << "\t������ ����������, ��������� �� �����������:                                           "   << si.wProcessorRevision << endl;
}

void get_global_memory_status()
{
    MEMORYSTATUSEX MS;
	MS.dwLength = sizeof(MS);
	GlobalMemoryStatusEx(&MS);
	cout << "���������� � ������ ����������:" << endl;
	cout << "\t������� ������������ ������: " << MS.dwMemoryLoad << "%"<< endl;
	cout << "\t����� ����� ���������� ������ � ������: " << MS.ullTotalPhys <<" ��� "<< (LONGLONG)MS.ullTotalPhys/1024.0/1024.0/1024.0<<" ��\n"<<endl;
	cout << "\t����� ��������� ���������� ������ � ������: " << MS.ullAvailPhys << " ��� " << (LONGLONG)MS.ullAvailPhys / 1024.0 / 1024.0 / 1024.0 << " ��\n" << endl;
	cout << "\t������ ����� �������� � ������: " << MS.ullTotalPageFile << " ��� " << (LONGLONG)MS.ullTotalPageFile / 1024.0 / 1024.0 / 1024.0 << " ��\n" << endl;
	cout << "\t��������� ����� ������ � ����� ��������: " << MS.ullAvailPageFile << " ��� " << (LONGLONG)MS.ullAvailPageFile / 1024.0 / 1024.0 / 1024.0 << " ��\n" << endl;
	cout << "\t����� ����� ����������� ������ � ������: " << MS.ullTotalVirtual << " ��� " << (LONGLONG)MS.ullTotalVirtual / 1024.0 / 1024.0 / 1024.0 << " ��\n" << endl;
	cout << "\t����� ��������� ����������� ������: " << MS.ullAvailVirtual << " ��� " << (LONGLONG)MS.ullAvailVirtual / 1024.0 / 1024.0 / 1024.0 << " ��\n" << endl << endl << endl;
}
void virtual_query()
{
    SIZE_T S;
	MEMORY_BASIC_INFORMATION MBI;
	S = sizeof(MBI);
	LPVOID adr=NULL;
	cout << "������� ����� ������, ��� ������� �������� ������ ������ ���������" << endl;
	cout << "����: 0x";
	cin >> adr;
	if (adr != NULL)
	{
		S = VirtualQuery(adr, &MBI, S);
		if (S != 0) {
			cout << "��������� ������� ������ ��� ��������� ������ " << "0x" << hex << adr << ":" << endl;
			cout << "\t��������� �� ������� ����� ������� �������: " << MBI.BaseAddress << endl;
			cout << "\t��������� �� ������� ����� ��������� �������, ���������� �������� VirtualAlloc: " << MBI.AllocationBase << endl;
			cout << "\t�������� ������ ������ ��� �������������� ��������� �������: ";
			protect_info(MBI.AllocationProtect);
			cout << "\t������ �������, ������������ � �������� ������, � ������� ��� �������� ����� ���������� ��������, � ������: " <<dec <<(LONGLONG)MBI.RegionSize << endl << endl;
			cout << "\t\n\t��������� ������� � �������: ";
			switch (MBI.State)
			{
			case MEM_COMMIT:
				cout << "�������������� ��������, ��� ������� ���� �������� ���������� ���������, ���� � ������, ���� � ����� �������� �� �����" << endl;
				break;
			case MEM_FREE:
				cout << "���� ��������� ��������, ����������� ����������� �������� � ��������� ��� ���������" << endl;
				break;
			case MEM_RESERVE:
				cout << "���������������� ��������, �� ������� �������������� �������� ������������ ��������� ������������ �������� ��� ��������� ������-���� ����������� ���������." << endl;
				break;
			}
			cout << "\t������ ������� � ��������� � �������: ";
			protect_info(MBI.Protect);
			cout << "\t��� ������� � �������: ";
			switch (MBI.Type)
			{
			case MEM_IMAGE:
				cout << "�������� � ������� �������������� � ������������� ����������� �������" << endl;
				break;
			case MEM_MAPPED:
				cout << "�������� � ������� �������������� � ������������� �������" << endl;
				break;
			case MEM_PRIVATE:
				cout << "�������� � ������� ��������" << endl;
				break;
			}
		}
		else cerr << "������: " << GetLastError();
	}
	else cout << "ADR==NULL" << endl;
}
void memory_reserve(SYSTEM_INFO si)
{
	int q;
	cout << "�������� �����:" << endl;
	cout << "1 - �������������� �����" << endl;
	cout << "2 - ����� ����� ������ ������ ������" << endl;
	cin >> q;
	if (q == 1 || q == 2)
	{
		void *address = NULL;
		if (q == 2)
		{
			cout << "�������������� ������� ������ � ������ ����� ������ ������ �������" << endl;
			cout << "������� ����� ������ �������:" << endl;
			cin >> address;
			address = VirtualAlloc(address, si.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		}
		else
		{
			cout << "�������������� ������� ������ � �������������� ������" << endl;
			address = VirtualAlloc(NULL, si.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		}
		if (address != NULL)
		{
			VirtualAlloc(address, si.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
			cout << "������ ������� ��������." << endl;
			cout << "�����: " << address << endl;
		}
		else cout << "������ VirtualAlloc" << endl;
	}
	else cout << "����� ������ 1 ��� 2." << endl;
}
void memory_reserve_and_commit(SYSTEM_INFO si)
{
    int q;
	cout << "�������� �����:" << endl;
	cout << "1 - �������������� �����" << endl;
	cout << "2 - ����� ����� ������ ������ ������" << endl;
	cin >> q;
	if (q == 1 || q == 2)
	{
		void *address = NULL;
		if (q == 2)
		{
			cout << "�������������� ������� ������ � ������ ����� ������ ������ �������" << endl;
			cout << "������� ����� ������ �������:" << endl;
			cin >> address;
			address = VirtualAlloc(address, si.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
		else
		{
			cout << "�������������� ������� ������ � �������������� ������" << endl;
			address = VirtualAlloc(NULL, si.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
		if (address != NULL)
		{
			cout << "������ ��������." << endl;
			cout << "�����: " << address << endl;
			}
		else cout << "������ VirtualAlloc" << endl;
	}
	else cout << "����� ������ 1 ��� 2." << endl;
}
void input()
{
    string  source = "";
    LPVOID  address = NULL;
    CHAR*   destination = NULL;

    cout << "������� ������ ��� ������:" << endl;
    cin >> source;

    cout << "������� ����� ��� ������: 0x";
    cin >> address;

    if (address != NULL)
    {
        MEMORY_BASIC_INFORMATION WD;

        VirtualQuery(address, &WD, 256);

        if (WD.Protect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READWRITE | PAGE_WRITECOPY))
        {
            destination = (CHAR*)address;
            CopyMemory(destination, source.c_str(), source.length() * sizeof(char));
            cout << endl << "������ ������ " << address << " ��������� �������. �������� ����������:" << endl;
            for (size_t i = 0; i < source.length(); i++)
                cout << destination[i];
        }
        else {
            cout << "������� ������� �� ������������� �����������!";
        }
        cout << endl << endl;
    }
    else cout << "������� �����." << endl;
}

DWORD protect_choose() {
    cout << "�������� ���� �� �������:" << endl;
    cout << "1. PAGE_EXECUTE" << endl;
    cout << "2. PAGE_EXECUTE_READ" << endl;
    cout << "3. PAGE_EXECUTE_READWRITE" << endl;
    cout << "4. PAGE_EXECUTE_WRITECOPY" << endl;
    cout << "5. PAGE_NOACCESS" << endl;
    cout << "6. PAGE_READONLY" << endl;
    cout << "7. PAGE_READWRITE" << endl;
    cout << "8. PAGE_WRITECOPY" << endl;

    int x;
    cin >> x;

    DWORD level;

    switch (x) {
    case 1:
        level = PAGE_EXECUTE;
        break;
    case 2:
        level = PAGE_EXECUTE_READ;
        break;
    case 3:
        level = PAGE_EXECUTE_READWRITE;
        break;
    case 4:
        level = PAGE_EXECUTE_WRITECOPY;
        break;
    case 5:
        level = PAGE_NOACCESS;
        break;
    case 6:
        level = PAGE_READONLY;
        break;
    case 7:
        level = PAGE_READWRITE;
        break;
    case 8:
        level = PAGE_WRITECOPY;
        break;
    }

    return level;
}
void set_virtual_protect()
{
    LPVOID address = NULL;

    DWORD oldLevel;
    DWORD newLevel;

    cout << "������� �����: 0x";
    cin >> address;

    if (address != NULL) {
        newLevel = protect_choose();
        cout << "����� ������� ������: ";
        protect_info(newLevel);
        cout << endl;
        if (VirtualProtect(address, sizeof(DWORD), newLevel, &oldLevel))
        {
            cout << "������ ������� ������:" << endl;
            protect_info(oldLevel);
        }
        else cout << "������: " << GetLastError() << endl;

    }
    else cout << "������� ����� (NULL)" << endl;
    cout << endl << endl;
}
void free_virtual_memory()
{
    LPVOID address = NULL;
    cout << "������� ����� ��� �������� ���. ������ � ������������ ������� ��������� ������������: 0x";
    cin >> address;

    if (VirtualFree(address, 0, MEM_RELEASE))
        cout << "������ ������� ����������" << endl << endl;
    else cerr << "������: " << GetLastError();
    cout << endl << endl;
}
void protect_info(DWORD Pro)
{
	switch (Pro)
	{
	case 0:
		cout << "���������� �������" << endl;
		break;
	case PAGE_EXECUTE:
		cout << "�������� ���������� ������� � ��������������� ������� ������� (PAGE_EXECUTE)" << endl;
		break;
	case PAGE_EXECUTE_READ:
		cout << "\t\t\t������� ������ ������ ��� ���������� ��� ������ � ��������������� ������� �������. " << endl;
		cout << "\t\t\t������� ������ � ��������������� ������ �������� � ��������� ������� (PAGE_EXECUTE_READ)" << endl;
		break;
	case PAGE_EXECUTE_READWRITE:
		cout << "������� ������ ������ ��� ����������, ������ ��� ������/������ � ��������������� ������� ������� (PAGE_EXECUTE_READWRITE)" << endl;
		break;
	case PAGE_EXECUTE_WRITECOPY:
		cout << "�������� ������ ������ ��� ����������, ������ ��� ����������� ��� ������ � ��������������� ������������� ������� ������������� ������ (PAGE_EXECUTE_WRITECOPY)" << endl;
		break;
	case PAGE_NOACCESS:
		cout << "�������� ���� ������ � ��������������� ������� ������� (PAGE_NOACCESS)" << endl;
		break;
	case PAGE_READONLY:
		cout << "������� ������ ������ ��� ������ � ��������������� ������� ������� (PAGE_READONLY)" << endl;
		break;
	case PAGE_READWRITE:
		cout << "�������� ������ ������ ��� ������ ��� ������/������ � ��������������� ������� ������� (PAGE_READWRITE)" << endl;
		break;
	case PAGE_WRITECOPY:
		cout << "�������� ������ ������ ��� ������ ��� ����������� ��� ������ � ��������������� ������������� ������� ������������� ������ (PAGE_WRITECOPY)" << endl;
		break;
	}
	if ((Pro & PAGE_GUARD) != 0)
	{
		cout << "\t\t\t\t�������� � ������� �������� (PAGE_GUARD)" << endl;
	}
	if ((Pro & PAGE_NOCACHE) != 0)
	{
		cout << "\t\t\t\t�������� �� ���������� (PAGE_NOCACHE)" << endl;
	}
	if ((Pro & PAGE_WRITECOMBINE) != 0)
	{
		cout << "\t\t\t\t�������� � ����������� � ����� ��������������� ������ (PAGE_WRITECOMBINE)" << endl;
	}

}
