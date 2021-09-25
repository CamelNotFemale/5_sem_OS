#include <Windows.h>
#include <iostream>
#include <string.h>

using namespace std;
// меню
int menu();
// Вывод списка дисков
void show_disk_list();
// Вывод информации о диске
void disk_information();
// Создание нового каталога
void create_directory();
// Удаление каталога
void remove_directory();
// Создание файлов в заданном каталоге
void create_file();
// Копирование файла
void copy_file();
// Перемещение файла
void move_file();
// Вывод информации об атрибутах
void get_file_attributes();
// Изменение информации об атрибутах
void set_file_attributes();
// Вывод информации о файле по дескриптору
void get_info_by_handle();
// Информация о системном времени
void get_file_time();
// Изменение системного времени
void set_file_time();

int main()
{
    system("chcp 1251");
    int select = -1;

    do {
        select = menu();

		switch (select) {
        case 1:
            show_disk_list();
            break;
        case 2:
            disk_information();
            break;
        case 3:
            create_directory();
            break;
        case 4:
            remove_directory();
            break;
        case 5:
            create_file();
            break;
        case 6:
            copy_file();
            break;
        case 7:
            move_file();
            break;
        case 8:
            get_file_attributes();
            break;
        case 9:
            set_file_attributes();
            break;
        case 10:
            get_info_by_handle();
            break;
        case 11:
            get_file_time();
            break;
        case 12:
            set_file_time();
            break;
        case 0:
            cout << "Завершение программы..." << endl;
            break;
        default:
            cout << "Выбранный пункт меню отсутствует. Повторите действие." << endl;
		}
		system("pause");
    } while (select != 0);

    return 0;
}
int menu() {
    int select;

    system("cls");

    cout << "ГЛАВНОЕ МЕНЮ" << endl;
    cout << "1. Список дисков" << endl;
    cout << "2. Информация о диске" << endl;
    cout << "3. Создание нового каталога" << endl;
    cout << "4. Удаление каталога" << endl;
    cout << "5. Создание файла" << endl;
    cout << "6. Копирование файла" << endl;
    cout << "7. Перемещение файла" << endl;
    cout << "8. Информация об атрибутах файла" << endl;
    cout << "9. Установка атрибутов файла" << endl;
    cout << "10. Информация об атрибутах файла по дескриптору" << endl;
    cout << "11. Системное время" << endl;
    cout << "12. Изменение системного времени" << endl;
    cout << "0. Завершение программы" << endl;

    cout << "> ";
    cin >> select;

    return select;
}

void show_disk_list() {
    DWORD mask;
    mask = GetLogicalDrives();
    cout << "Список дисков системы:" << endl;
    for (int x = 0; x < 26; x++)
    {
        if ((mask >> x) & 1)
            cout << "- " << (char)(65 + x) << ":\\" << endl;
    }
}

void disk_information() {
    //GetDriveType
    string out,
        disk_name,
        path;
    UINT drive_type;

    //GetVolumeInfomation
    char drive_label[30];
    char drive_fat[30];
    DWORD drive_sn;
    DWORD drive_name_size = sizeof(drive_label);
    DWORD drive_flags;

    //GetFreeDiskSpace
    DWORD drive_sectorsPerCluster;
    DWORD drive_bytesPerSector;
    DWORD drive_numberOfFreeClusters;
    DWORD drive_totalNumberOfClusters;

    cout << "Введите название диска: ";
    cin >> disk_name;
    path = (disk_name+":\\").c_str();
    drive_type = GetDriveType(path.c_str());

    if (drive_type == DRIVE_UNKNOWN) out +=     "- UNKNOWN           (Неизвестный тип)";
    if (drive_type == DRIVE_NO_ROOT_DIR) out += "- DRIVE NO ROOT DIR (Неправильный путь)";
    if (drive_type == DRIVE_REMOVABLE) out +=   "- REMOVABLE         (Съёмный диск)";
    if (drive_type == DRIVE_FIXED) out +=       "- FIXED             (Фиксированный диск)";
    if (drive_type == DRIVE_REMOTE) out +=      "- REMOTE            (Удалённый диск)";
    if (drive_type == DRIVE_CDROM) out +=       "- CDROM             (CD-ROM диск)";
    if (drive_type == DRIVE_RAMDISK) out +=     "- RAMDISK           (RAM диск)";
    out += '\n';

    //GetVolumeInfomation

    if (!GetVolumeInformation(path.c_str(), // Имя тома
                             drive_label,       // Название Fileовой структуры
                             sizeof(drive_label), // Максимальная длина имени Fileа
                             &drive_sn, // Адрес серийного номера
                             &drive_name_size, // Адрес переменной, который получает максимальную длину компонента имени Fileа
                             &drive_flags, // Флаги
                             drive_fat, // Адрес имени Fileовой системы
                             sizeof(drive_fat) // Длина буфера имени Fileовой системы
                              )
        )
    {
        out += "Ошибка. Не удалось обработать запрос\n";
    }
    else
    {
        out += "Название указанного диска: " + string(drive_label) + "\n";
        out += "Cерийный номер диска:      " + to_string(drive_sn) + "\n";
        out += "Максимальная длина файла:  " + to_string(drive_name_size) + "\n";
        out += "Опции файловой системой:   \n";

        //Расшифровка drive_flags
        if (drive_flags & FILE_CASE_PRESERVED_NAMES)
            out += " -Указанный том поддерживает сохраненный регистр имен Fileов, когда он помещает имя на диск.\n";
        if (drive_flags & FILE_CASE_SENSITIVE_SEARCH)
            out += " -Указанный том поддерживает имена Fileов с учетом регистра.\n";
        if (drive_flags & FILE_FILE_COMPRESSION)
            out += " - Указанный том поддерживает сжатие Fileов.\n";
        if (drive_flags & FILE_NAMED_STREAMS)
            out += " - Указанный том поддерживает именованные потоки.\n";
        if (drive_flags & FILE_PERSISTENT_ACLS)
            out += " -Указанный том сохраняет и применяет списки управления доступом (ACL).\n";
        if (drive_flags & FILE_READ_ONLY_VOLUME)
            out += " - Указанный том доступен только для чтения.\n";
        if (drive_flags & FILE_SEQUENTIAL_WRITE_ONCE)
            out += " - Указанный том поддерживает одну последовательную запись.\n";
        if (drive_flags & FILE_SUPPORTS_ENCRYPTION)
            out += " -Указанный том поддерживает зашифрованную Fileовую систему (EFS).\n";
        if (drive_flags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
            out += " - Указанный том поддерживает расширенные атрибуты.\n";
        if (drive_flags & FILE_SUPPORTS_HARD_LINKS)
            out += " - Указанный том поддерживает жесткие ссылки.\n";
        if (drive_flags & FILE_SUPPORTS_OBJECT_IDS)
            out += " - Указанный том поддерживает идентификаторы объектов.\n";
        if (drive_flags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
            out += " - Fileовая система поддерживает открытие по FileID.\n";
        if (drive_flags & FILE_SUPPORTS_REPARSE_POINTS)
            out += " - Указанный том поддерживает точки повторного разбора.\n";
        if (drive_flags & FILE_SUPPORTS_SPARSE_FILES)
            out += " - Указанный том поддерживает разреженные Fileы.\n";
        if (drive_flags & FILE_SUPPORTS_TRANSACTIONS)
            out += " - Указанный том поддерживает транзакции.\n";
        if (drive_flags & FILE_SUPPORTS_USN_JOURNAL)
            out += " -Указанный том поддерживает журналы обновления порядковых номеров (USN).\n";
        if (drive_flags & FILE_UNICODE_ON_DISK)
            out += " -Указанный том поддерживает Unicode в именах Fileов по мере их появления на диске.\n";
        if (drive_flags & FILE_VOLUME_IS_COMPRESSED)
            out += " - Указанный том является сжатым томом.\n";
        if (drive_flags & FILE_VOLUME_QUOTAS)
            out += " - Указанный том поддерживает дисковые квоты.\n";

        out += "Имя файловой системы:      " + string(drive_fat) + "\n";
    }

    //GetFreeDiskSpace
    if (GetDiskFreeSpace (
                         path.c_str(),
                         &drive_sectorsPerCluster,
                         &drive_bytesPerSector,
                         &drive_numberOfFreeClusters,
                         &drive_totalNumberOfClusters
                        )
        )
    {
        out += "- Число секторов в кластере: " + to_string(drive_sectorsPerCluster) + '\n';
        out += "- Число байт в секторе: " + to_string(drive_bytesPerSector) + '\n';
        out += "- Число свободных кластеров: " + to_string(drive_numberOfFreeClusters) +'\n';
        out += "- Общее число кластеров: " + to_string(drive_totalNumberOfClusters) +'\n';
    }
    cout << out;
}

void create_directory() {
    string direct_name;
    cout << "Введите имя создаваемого каталога (пример: c:\\folder): ";
    cin >> direct_name;

    //CreateDirectory
    if (!CreateDirectory(direct_name.c_str(), nullptr)) {
        cout << "Каталог не удалось создать! Ошибка." << endl;
    }
    else {
        cout << "Каталог создан." << endl;
    }
}

void remove_directory() {
    string direct_name;
    cout << "Введите имя удаляемого каталога (пример: c:\\folder): ";
    cin >> direct_name;

    //RemoveDirectory
    if (!RemoveDirectory(direct_name.c_str())) {
        cout << "Каталог не удалось удалить! Ошибка." << endl;
    }
    else {
        cout << "Каталог удален." << endl;
    }
}

void create_file() {
    string file_name;
    cout << "Введите имя файла (пример: c:\\folder\\text.txt): ";
    cin >> file_name;

    if (INVALID_HANDLE_VALUE == CreateFile(
                                            file_name.c_str(), // указатель на имя файла
                                            GENERIC_WRITE | GENERIC_READ, // права доступа к файлу
                                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // режим совместного использования
                                            nullptr, // указатель на дескриптор безопасности
                                            CREATE_ALWAYS, // как создавать
                                            FILE_ATTRIBUTE_NORMAL, // аттрибуты файла
                                            nullptr // дескриптор файла с атрибутами для копирования
                                          )
        )
    {
        cout << "Ошибка. Код: : " << GetLastError() << endl;
    }
    else {
        cout << "Файл успешно создан." << endl;
    }
}

void copy_file() {
    string  file_original,
            file_copy;

    cout << "Введите имя копируемого файла (пример: c:\\folder\\text.txt):";
    cin >> file_original;
    cout << "Введите имя файла-копии: ";
    cin >> file_copy;
    if (CopyFile(file_original.c_str(),
                 file_copy.c_str(), false) // Перезапись файла с тем же именем не разрешена
        )
        cout << "File copied" << endl;
    else
        cout << "Code error: " << GetLastError() << endl;
}

void move_file() {
    string  path_from,
            path_to;

    cout << "Введите имя файла (пример: c:\\folder\\text.pdf): ";
    cin >> path_from;
    cout << "Введите путь для перемещения (пример: c:\\folder\\text.pdf): ";
    cin >> path_to;

    if (MoveFile(path_from.c_str(), path_to.c_str())) {
        cout << "Файл успешно перемещен." << endl;
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        cout << "Уже существует такой файл в данном каталоге. Заменить его? (1 - ДА, 0 - НЕТ)" << endl;
        int select;
        cin >> select;
        if (select) {
            if (MoveFileEx(path_from.c_str(), path_to.c_str(), MOVEFILE_REPLACE_EXISTING))
            {
                cout << "Файл или каталог успешно перемещен!" << endl;
            }
            else {
                cout << "Ошибка! Не удалось переместить файл. Код ошибки: " << GetLastError() << endl;
            }
        }
    }
    else {
        cout << "Ошибка! Не удалось переместить файл. Код ошибки: " << GetLastError() << endl;
    }
}

void get_file_attributes() {
    string path;
    cout << "Введите имя файла (пример: c:\\folder\\text.pdf): ";
    cin >> path;

    DWORD attribute = GetFileAttributes(path.c_str());

    if (attribute == 0) {
        cout << "Ошибка! Не удалось получить атрибуты файла." << endl;
    }
    else if (attribute == -1) {
        cout << "Ошибка! Файл не найден." << endl;
    }
    else {
        cout << "Атрибуты файла (" << attribute << "):" << endl;
        if (FILE_ATTRIBUTE_ARCHIVE & attribute) cout <<    "- Архивный файл" << endl;
        if (FILE_ATTRIBUTE_COMPRESSED & attribute) cout << "- Сжатый файл" << endl;
        if (FILE_ATTRIBUTE_DIRECTORY & attribute) cout <<  "- Каталог" << endl;
        if (FILE_ATTRIBUTE_HIDDEN & attribute) cout <<     "- Скрытый файл или каталог" << endl;
        if (FILE_ATTRIBUTE_NORMAL & attribute) cout <<     "- Файл не имеет атрибутов" << endl;
        if (FILE_ATTRIBUTE_READONLY & attribute) cout <<   "- Файл только для чтения" << endl;
    }
}

void set_file_attributes() {
    string path;
    cout << "Введите имя файла (пример: c:\\folder\\text.pdf): ";
    cin >> path;

    DWORD attribute;
    int select;
    cout << "Изменить атрибут файла на:" << endl;
    cout << "1 Архивный файл" << endl;
    cout << "2 Скрытый файл" << endl;
    cout << "3 Файл без атрибутов" << endl;
    cout << "4 Файл только для чтения" << endl;
    cout << "5 Системный файл" << endl;
    cin >> select;
    switch (select) {
    case 1:
        attribute = FILE_ATTRIBUTE_ARCHIVE;
        break;
    case 2:
        attribute = FILE_ATTRIBUTE_HIDDEN;
        break;
    case 3:
        attribute = FILE_ATTRIBUTE_NORMAL;
        break;
    case 4:
        attribute = FILE_ATTRIBUTE_READONLY;
        break;
    case 5:
        attribute = FILE_ATTRIBUTE_SYSTEM;
        break;
    }

    if (SetFileAttributes(path.c_str(), attribute))
    {
        cout << "Атрибут файла успешно изменен!" << endl;
    }
    else {
        cout << "Ошибка! Не удалось изменить атрибут файла. Код ошибки: " << GetLastError() << endl;
    }
}

void get_info_by_handle() {
    long FileAttributes;
    BY_HANDLE_FILE_INFORMATION infoFile;
    HANDLE handleFile;

    string path;
    cout << "Введите имя файла (пример: c:\\folder\\text.txt): ";
    cin >> path;

    handleFile = CreateFileA((LPCSTR)path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (GetFileInformationByHandle(handleFile, &infoFile)) {
        FileAttributes = infoFile.dwFileAttributes;
        if (FileAttributes == -1)
            cout << " Файл " << path << " не найден." << endl;
        else {
            cout << "Информация о файле по дескриптору:" << endl;

            if (FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
                cout << " Файл архивный." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                cout << " Файл - директория." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_READONLY)
                cout << " Файл - только для чтения." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_SYSTEM)
                cout << " Файл - системный." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_COMPRESSED)
                cout << " Файл - сжатый." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_HIDDEN)
                cout << " Файл - скрытый." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_NORMAL)
                cout << " Таких атрибутов файл не имеет.";
            if (FileAttributes == 0)
                cout << " Произошла ошибка. Код ошибки: " << GetLastError() << endl;
        }

        cout << " Старшая часть уникального идентификатора связи с файлом: " << infoFile.nFileIndexHigh << endl;
        cout << " Порядковый номер тома, который содержит файл: " << infoFile.dwVolumeSerialNumber << endl;
        cout << " Старшая часть размера файла: " << infoFile.nFileSizeHigh << endl;
        cout << " Младшая часть размера файла: " << infoFile.nFileSizeLow << endl;
        cout << " Число связей с этим файлом: " << infoFile.nNumberOfLinks << endl;
        cout << " Младшая часть уникального идентификатора связи с файлом: " << infoFile.nFileIndexLow << endl;
    }
    else {
        cout << " Произошла ошибка. Код ошибки: " << GetLastError() << endl;
    }

    CloseHandle(handleFile);
    cout << endl;
}

void get_file_time() {
    FILETIME timeCreate, timeLastAccess, timeLastWrite;
    SYSTEMTIME time;

    string path;
    cout << "Введите имя файла (пример: c:\\folder\\text.txt): ";
    cin >> path;

    HANDLE fileHandle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (GetFileTime(fileHandle, &timeCreate, &timeLastAccess, &timeLastWrite))
    {
        FileTimeToSystemTime(&timeCreate, &time);
        cout << "Дата и время создания файла:                " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" << time.wSecond << endl;
        FileTimeToSystemTime(&timeLastAccess, &time);
        cout << "Дата и время последнего обращения к файлу:  " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" << time.wSecond << endl;
        FileTimeToSystemTime(&timeLastWrite, &time);
        cout << "Дата и время последней модификации файла:   " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" <<time.wSecond << endl;
    }
    else {
        cout << "Ошибка! Не удалось получить информацию о файле. Код ошибки: " << GetLastError() << endl;
    }
}

void set_file_time() {
    FILETIME fileTime;
    SYSTEMTIME systemTime;

    string path;
    cout << "Введите имя файла (пример: c:\\folder\\text.txt): ";
    cin >> path;

    HANDLE fileHandle = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &fileTime);
    if (SetFileTime(fileHandle, // путь к файлу
                    nullptr,    // дата создания
                    &fileTime,  // дата последнего обращения
                    &fileTime)  // дата последнего изменения
        )
    {
        cout << "Атрибуты времени заменены успешно!" << endl;
    }
    else {
        cout << "Ошибка! Код ошибки: " << GetLastError() << endl;
    }
}
