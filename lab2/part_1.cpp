#include <iostream>
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <string>

using namespace std;
// Уровень защиты
void protect_info(DWORD Pro);
// Возврат физической памяти и освобождение региона адресного пространства заданного(с клавиатуры) региона памяти
void free_virtual_memory();
// Установка защиты доступа для заданного (с клавиатуры) региона памяти и ее проверка
void set_virtual_protect();
// Запись данных в ячейки памяти по заданным с клавиатуры адресам
void input();
// Резервирование региона и передача ему физической памяти в автоматическом режиме и в режиме ввода адреса начала региона
void memory_reserve_and_commit(SYSTEM_INFO si);
// Резервирование региона в автоматическом режиме и в режиме ввода адреса начала региона
void memory_reserve(SYSTEM_INFO si);
// Определение состояния конкретного участка памяти по заданному с клавиатуры адресу
void virtual_query();
// Определение статуса виртуальной памяти
void get_global_memory_status();
// Получить информацию о выч. системе
void get_system_info(SYSTEM_INFO si);

int main()
{
	int c;
	SYSTEM_INFO MSI;
	GetSystemInfo(&MSI);
	system("chcp 1251");

	do {
        system("cls");
		cout << "1. Получить информацию о выч. системе" << endl;
		cout << "2. Определение статуса виртуальной памяти" << endl;
		cout << "3. Определение состояния конкретного участка памяти по заданному с клавиатуры адресу" << endl;
		cout << "4. Резервирование региона в автоматическом режиме и в режиме ввода адреса начала региона" << endl;
		cout << "5. Резервирование региона и передача ему физической памяти в автоматическом режиме и в режиме ввода адреса начала региона" << endl;
		cout << "6. Запись данных в ячейки памяти по заданным с клавиатуры адресам: " << endl;
		cout << "7. Установка защиты доступа для заданного (с клавиатуры) региона памяти и ее проверка" << endl;
		cout << "8. Возврат физической памяти и освобождение региона адресного пространства заданного(с клавиатуры) региона памяти" << endl;
		cout << "\n0. Выход\n" << endl;
		cout << "Ввод>> ";
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
			cout << "Ошибка! Такого пункта нет." << endl;
			break;

		}
		system("pause");
	}
	while (c!=0);

	return 0;
}

void get_system_info(SYSTEM_INFO si)
{
    cout << "Архитектура процессора: ";

    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)    cout << "x64 (AMD или INTEL)" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )    cout << "Intel Itanium Processor Family (IPF)" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)    cout << "x86" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)      cout << "ARM" << endl;
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_UNKNOWN)  cout << "Неизвестная архитектура" << endl;

    cout << "\tРазмер страницы и степень гранулярности защиты страниц и обязательств:                 "   << si.dwPageSize << endl;
    cout << "\tУказатель на самый низкий адрес памяти, доступный приложениям и библиотекам:           " << si.lpMinimumApplicationAddress << endl;
    cout << "\tУказатель на самый высокий адрес памяти, доступный приложениям и библиотекам:          " << si.lpMaximumApplicationAddress << endl;
    cout << "\tПроцессоры, сконфигурированные в системе:" << endl;
	for (DWORD i = 0; i < si.dwNumberOfProcessors; i++)
	{
		if (si.dwActiveProcessorMask & (1 << i))
			cout << "\t\tПроцессор " << i<<endl;
	}
    cout << "\tКоличество логических процессоров в текущей группе:                                    "   << si.dwNumberOfProcessors << endl;
    cout << "\tГранулярность начального адреса, по которому может быть выделена виртуальная память:   "   << si.dwAllocationGranularity << endl;
    cout << "\tУровень процессора:                                                                    "   << si.wProcessorLevel << endl;
    cout << "\tВерсия процессора, зависящая от архитектуры:                                           "   << si.wProcessorRevision << endl;
}

void get_global_memory_status()
{
    MEMORYSTATUSEX MS;
	MS.dwLength = sizeof(MS);
	GlobalMemoryStatusEx(&MS);
	cout << "Информация о памяти компьютера:" << endl;
	cout << "\tПроцент используемой памяти: " << MS.dwMemoryLoad << "%"<< endl;
	cout << "\tОбщий объем физической памяти в байтах: " << MS.ullTotalPhys <<" или "<< (LONGLONG)MS.ullTotalPhys/1024.0/1024.0/1024.0<<" Гб\n"<<endl;
	cout << "\tОбъем доступной физической памяти в байтах: " << MS.ullAvailPhys << " или " << (LONGLONG)MS.ullAvailPhys / 1024.0 / 1024.0 / 1024.0 << " Гб\n" << endl;
	cout << "\tРазмер файла подкачки в байтах: " << MS.ullTotalPageFile << " или " << (LONGLONG)MS.ullTotalPageFile / 1024.0 / 1024.0 / 1024.0 << " Гб\n" << endl;
	cout << "\tДоступный объем байтов в файле подкачки: " << MS.ullAvailPageFile << " или " << (LONGLONG)MS.ullAvailPageFile / 1024.0 / 1024.0 / 1024.0 << " Гб\n" << endl;
	cout << "\tОбщий объем виртуальной памяти в байтах: " << MS.ullTotalVirtual << " или " << (LONGLONG)MS.ullTotalVirtual / 1024.0 / 1024.0 / 1024.0 << " Гб\n" << endl;
	cout << "\tОбъем доступной виртуальной памяти: " << MS.ullAvailVirtual << " или " << (LONGLONG)MS.ullAvailVirtual / 1024.0 / 1024.0 / 1024.0 << " Гб\n" << endl << endl << endl;
}
void virtual_query()
{
    SIZE_T S;
	MEMORY_BASIC_INFORMATION MBI;
	S = sizeof(MBI);
	LPVOID adr=NULL;
	cout << "Введите адрес памяти, для участка которого хотите узнать состояние" << endl;
	cout << "Ввод: 0x";
	cin >> adr;
	if (adr != NULL)
	{
		S = VirtualQuery(adr, &MBI, S);
		if (S != 0) {
			cout << "Состояние участка памяти для заданного адреса " << "0x" << hex << adr << ":" << endl;
			cout << "\tУказатель на базовый адрес региона страниц: " << MBI.BaseAddress << endl;
			cout << "\tУказатель на базовый адрес диапазона страниц, выделенных функцией VirtualAlloc: " << MBI.AllocationBase << endl;
			cout << "\tПараметр защиты памяти при первоначальном выделении области: ";
			protect_info(MBI.AllocationProtect);
			cout << "\tРазмер области, начинающейся с базового адреса, в котором все страницы имеют одинаковые атрибуты, в байтах: " <<dec <<(LONGLONG)MBI.RegionSize << endl << endl;
			cout << "\t\n\tСостояние страниц в регионе: ";
			switch (MBI.State)
			{
			case MEM_COMMIT:
				cout << "Зафиксированны страницы, для которых было выделено физическое хранилище, либо в памяти, либо в файле подкачки на диске" << endl;
				break;
			case MEM_FREE:
				cout << "Есть свободные страницы, недоступные вызывающему процессу и доступные для выделения" << endl;
				break;
			case MEM_RESERVE:
				cout << "Зарезервированны страницы, на которых зарезервирован диапазон виртуального адресного пространства процесса без выделения какого-либо физического хранилища." << endl;
				break;
			}
			cout << "\tЗащита доступа к страницам в регионе: ";
			protect_info(MBI.Protect);
			cout << "\tТип страниц в регионе: ";
			switch (MBI.Type)
			{
			case MEM_IMAGE:
				cout << "Страницы в регионе спроецированны в представление изображения раздела" << endl;
				break;
			case MEM_MAPPED:
				cout << "Странице в регионе спроецированны в представление раздела" << endl;
				break;
			case MEM_PRIVATE:
				cout << "Страницы в регионе приватны" << endl;
				break;
			}
		}
		else cerr << "Ошибка: " << GetLastError();
	}
	else cout << "ADR==NULL" << endl;
}
void memory_reserve(SYSTEM_INFO si)
{
	int q;
	cout << "Выберите режим:" << endl;
	cout << "1 - Автоматический режим" << endl;
	cout << "2 - Режим ввода адреса начала реиона" << endl;
	cin >> q;
	if (q == 1 || q == 2)
	{
		void *address = NULL;
		if (q == 2)
		{
			cout << "Резервирование региона памяти в режиме ввода адреса начала региона" << endl;
			cout << "Введите адрес начала региона:" << endl;
			cin >> address;
			address = VirtualAlloc(address, si.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		}
		else
		{
			cout << "Резервирование региона памяти в автоматическом режиме" << endl;
			address = VirtualAlloc(NULL, si.dwPageSize, MEM_RESERVE, PAGE_READWRITE);
		}
		if (address != NULL)
		{
			VirtualAlloc(address, si.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
			cout << "Память успешно выделена." << endl;
			cout << "Адрес: " << address << endl;
		}
		else cout << "Ошибка VirtualAlloc" << endl;
	}
	else cout << "Нужно ввести 1 или 2." << endl;
}
void memory_reserve_and_commit(SYSTEM_INFO si)
{
    int q;
	cout << "Выберите режим:" << endl;
	cout << "1 - Автоматический режим" << endl;
	cout << "2 - Режим ввода адреса начала реиона" << endl;
	cin >> q;
	if (q == 1 || q == 2)
	{
		void *address = NULL;
		if (q == 2)
		{
			cout << "Резервирование региона памяти в режиме ввода адреса начала региона" << endl;
			cout << "Введите адрес начала региона:" << endl;
			cin >> address;
			address = VirtualAlloc(address, si.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
		else
		{
			cout << "Резервирование региона памяти в автоматическом режиме" << endl;
			address = VirtualAlloc(NULL, si.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
		if (address != NULL)
		{
			cout << "Память выделена." << endl;
			cout << "Адрес: " << address << endl;
			}
		else cout << "Ошибка VirtualAlloc" << endl;
	}
	else cout << "Нужно ввести 1 или 2." << endl;
}
void input()
{
    string  source = "";
    LPVOID  address = NULL;
    CHAR*   destination = NULL;

    cout << "Введите данные для записи:" << endl;
    cin >> source;

    cout << "Введите адрес для записи: 0x";
    cin >> address;

    if (address != NULL)
    {
        MEMORY_BASIC_INFORMATION WD;

        VirtualQuery(address, &WD, 256);

        if (WD.Protect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READWRITE | PAGE_WRITECOPY))
        {
            destination = (CHAR*)address;
            CopyMemory(destination, source.c_str(), source.length() * sizeof(char));
            cout << endl << "Ячейка памяти " << address << " заполнена успешно. Введённая информация:" << endl;
            for (size_t i = 0; i < source.length(); i++)
                cout << destination[i];
        }
        else {
            cout << "Уровень доступа не соответствует требованиям!";
        }
        cout << endl << endl;
    }
    else cout << "Нулевой адрес." << endl;
}

DWORD protect_choose() {
    cout << "Выберите один из пунктов:" << endl;
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

    cout << "Введите адрес: 0x";
    cin >> address;

    if (address != NULL) {
        newLevel = protect_choose();
        cout << "Новый уровень защиты: ";
        protect_info(newLevel);
        cout << endl;
        if (VirtualProtect(address, sizeof(DWORD), newLevel, &oldLevel))
        {
            cout << "Старый уровень защиты:" << endl;
            protect_info(oldLevel);
        }
        else cout << "Ошибка: " << GetLastError() << endl;

    }
    else cout << "Нулевой адрес (NULL)" << endl;
    cout << endl << endl;
}
void free_virtual_memory()
{
    LPVOID address = NULL;
    cout << "Введите адрес для возврата физ. памяти и освобождения региона адресного пространства: 0x";
    cin >> address;

    if (VirtualFree(address, 0, MEM_RELEASE))
        cout << "Регион успешно освобожден" << endl << endl;
    else cerr << "Ошибка: " << GetLastError();
    cout << endl << endl;
}
void protect_info(DWORD Pro)
{
	switch (Pro)
	{
	case 0:
		cout << "Отсутствие доступа" << endl;
		break;
	case PAGE_EXECUTE:
		cout << "Включено выполнение доступа к зафиксированной области страниц (PAGE_EXECUTE)" << endl;
		break;
	case PAGE_EXECUTE_READ:
		cout << "\t\t\tВключен доступ только для выполнения или чтения к зафиксированной области страниц. " << endl;
		cout << "\t\t\tПопытка записи в зафиксированный регион приводит к нарушению доступа (PAGE_EXECUTE_READ)" << endl;
		break;
	case PAGE_EXECUTE_READWRITE:
		cout << "Включен доступ только для выполнения, чтения или чтения/записи к зафиксированной области страниц (PAGE_EXECUTE_READWRITE)" << endl;
		break;
	case PAGE_EXECUTE_WRITECOPY:
		cout << "Включает доступ только для выполнения, чтения или копирования при записи к сопоставленному представлению объекта сопоставления файлов (PAGE_EXECUTE_WRITECOPY)" << endl;
		break;
	case PAGE_NOACCESS:
		cout << "Отключен весь доступ к зафиксированной области страниц (PAGE_NOACCESS)" << endl;
		break;
	case PAGE_READONLY:
		cout << "Включен доступ только для чтения к зафиксированной области страниц (PAGE_READONLY)" << endl;
		break;
	case PAGE_READWRITE:
		cout << "Включает доступ только для чтения или чтения/записи к зафиксированной области страниц (PAGE_READWRITE)" << endl;
		break;
	case PAGE_WRITECOPY:
		cout << "Включает доступ только для чтения или копирования при записи к сопоставленному представлению объекта сопоставления файлов (PAGE_WRITECOPY)" << endl;
		break;
	}
	if ((Pro & PAGE_GUARD) != 0)
	{
		cout << "\t\t\t\tСтраницы в регионе защищены (PAGE_GUARD)" << endl;
	}
	if ((Pro & PAGE_NOCACHE) != 0)
	{
		cout << "\t\t\t\tСтраницы не кэшируются (PAGE_NOCACHE)" << endl;
	}
	if ((Pro & PAGE_WRITECOMBINE) != 0)
	{
		cout << "\t\t\t\tСтраницы в установлены в режим комбинированной записи (PAGE_WRITECOMBINE)" << endl;
	}

}
