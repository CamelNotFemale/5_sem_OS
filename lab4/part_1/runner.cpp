#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

using namespace std;

int main()
{
    const int pageSize = 4096;              //Размер страницы
    const int Count = 16;                   //Число страниц буферной памяти (3+0+8+1+4)
    const int N = Count;                    //Количестов семафоров (с учётом нулевого)
    const int writerCount = Count / 2;      //Счётчик писателей
    const int readerCount = Count / 2;      //Счётчик читателей
    std::vector<HANDLE> pageMutex;
    std::string mutexName = "mutex_*";

    int fileSize = Count * pageSize;

    HANDLE freeSem = CreateSemaphore(NULL, N, N, (LPCTSTR)"freeSem");     //Создание семафора для неотработанных страниц
    HANDLE usedSem = CreateSemaphore(NULL, 0, N, (LPCTSTR)"usedSem");     //Создание семафора для отработанных страниц
    for(int i = 0;i < Count;i++)
    {
        mutexName.back() = '0' + i;
        pageMutex.push_back(CreateMutexA(nullptr, false, mutexName.c_str()));
    }
    
    HANDLE mapHandle;

    cout << "Starting program...\n";


    //Создаём файл, с которым будет производиться работа

    HANDLE fHandle = CreateFileA("C:\\Users\\Dima\\Documents\\5_sem_OS\\lab4\\file.txt", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    //Создаём проекцию файла

    const char sharedFile[] = "MAPPING";

    mapHandle = CreateFileMapping(fHandle, NULL, PAGE_READWRITE, 0, 0, (LPCTSTR)sharedFile);

    if(!mapHandle) {
        cout << "Empty";
    }

    //Отображаем проекцию

    LPVOID mapView = MapViewOfFile(mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);

    //Блокируем страницы

    VirtualLock(mapView, fileSize);

    //Массив HANDLE для процессов

    HANDLE processHandles[Count];

    //Цикл для писателей

    for (int i = 0; i < writerCount; ++i)
    {
        //Называем очередной файл

        string logName = "C:\\Users\\Dima\\Documents\\5_sem_OS\\lab4\\part_1\\WriterLog" + to_string(i + 1) + ".txt";

        STARTUPINFO sysInfo;                                                    //Информация для создания процесса
        PROCESS_INFORMATION procInfo;                                           //Объект для записи информации о процессе
        SECURITY_ATTRIBUTES secureAttr = { sizeof(secureAttr), NULL, TRUE };    //Атрибуты безопасности

        //Обнуляем нашу информацию о процессе

        ZeroMemory(&sysInfo, sizeof(sysInfo));

        //Создаём файл для логгирования

        HANDLE outHandle = CreateFile((LPCTSTR)(logName.c_str()), GENERIC_WRITE, FILE_SHARE_WRITE, &secureAttr,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        sysInfo.cb = sizeof(sysInfo);               //Устанавливает размер структуры, в байтах
        sysInfo.hStdOutput = outHandle;             //Определяет дескриптор, который будет использован как стандартный дескриптор вывода для процесса
        sysInfo.hStdError = NULL;                   //Определяет дескриптор, который будет использован как стандартный дескриптор ошибки для процесса
        sysInfo.hStdInput = NULL;                   //Определяет дескриптор, который будет использован как стандартный дескриптор ввода для процесса
        sysInfo.dwFlags |= STARTF_USESTDHANDLES;    //Устанавливается стандартный ввод данных, стандартный вывод и стандартная обработка ошибок дескрипторов процесса

        ZeroMemory(&procInfo, sizeof(procInfo));

        int mainProcess = CreateProcess(
            (LPCTSTR)"C:\\Users\\Dima\\Documents\\5_sem_OS\\lab4\\part_1\\writer.exe",
            NULL,                                   // Командная строка
            NULL,                                   // Дескриптор процесса не наследуется
            NULL,                                   // Дескриптор потока не наследуется
            TRUE,                                   // Установить наследование дескрипторов в TRUE
            0,                                      // Нет флагов создания
            NULL,                                   // Использовать родительский блок среды
            NULL,                                   // Использовать начальный каталог родителя
            &sysInfo,                               // Указатель на структуру STARTUPINFO
            &procInfo);                             // Указатель на структуру PROCESS_INFORMATION

        if (mainProcess)
            processHandles[i] = procInfo.hProcess;
    }

    //Аналогично работаем и с читателями

    for (int i = 0; i < readerCount; ++i)
    {
        string fname = "C:\\Users\\Dima\\Documents\\5_sem_OS\\lab4\\part_1\\ReaderLog" + to_string(i + 1) + ".txt";

        STARTUPINFO sysInfo;
        PROCESS_INFORMATION procInfo;
        SECURITY_ATTRIBUTES secureAttr = { sizeof(secureAttr), NULL, TRUE };

        ZeroMemory(&sysInfo, sizeof(sysInfo));

        sysInfo.hStdOutput = CreateFile((LPCTSTR)(fname.c_str()), GENERIC_WRITE, FILE_SHARE_WRITE, &secureAttr,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        sysInfo.cb = sizeof(sysInfo);
        sysInfo.hStdError = NULL;
        sysInfo.hStdInput = NULL;
        sysInfo.dwFlags |= STARTF_USESTDHANDLES;
        ZeroMemory(&procInfo, sizeof(procInfo));

        int createProcess = CreateProcess(
            (LPCTSTR)"C:\\Users\\Dima\\Documents\\5_sem_OS\\lab4\\part_1\\reader.exe",
            NULL,                                   // Командная строка
            NULL,                                   // Дескриптор процесса не наследуется
            NULL,                                   // Дескриптор потока не наследуется
            TRUE,                                   // Установить наследование дескрипторов в TRUE
            0,                                      // Нет флагов создания
            NULL,                                   // Использовать родительский блок среды
            NULL,                                   // Использовать начальный каталог родителя
            &sysInfo,                               // Указатель на структуру STARTUPINFO
            &procInfo);                             // Указатель на структуру PROCESS_INFORMATION


        if (createProcess != 0)
            processHandles[writerCount + i] = procInfo.hProcess;
    }

    WaitForMultipleObjects(Count, processHandles, true, INFINITE);

    for (int i = 0; i < Count; ++i)
        CloseHandle(processHandles[i]);
        
    VirtualUnlock(mapView, fileSize);
    UnmapViewOfFile(mapView);
    CloseHandle(mapHandle);
    CloseHandle(fHandle);
    for(int i = 0;i < Count;i++)
    {
        CloseHandle(pageMutex[i]);
    }
    CloseHandle(freeSem);
    CloseHandle(usedSem);

    cout << endl << "Done. Press ENTER to close the program." << endl;
    system("pause");
}
