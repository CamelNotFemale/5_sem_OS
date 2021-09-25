#include <Windows.h>
#include <iostream>
#include <string.h>

using namespace std;
// ����
int menu();
// ����� ������ ������
void show_disk_list();
// ����� ���������� � �����
void disk_information();
// �������� ������ ��������
void create_directory();
// �������� ��������
void remove_directory();
// �������� ������ � �������� ��������
void create_file();
// ����������� �����
void copy_file();
// ����������� �����
void move_file();
// ����� ���������� �� ���������
void get_file_attributes();
// ��������� ���������� �� ���������
void set_file_attributes();
// ����� ���������� � ����� �� �����������
void get_info_by_handle();
// ���������� � ��������� �������
void get_file_time();
// ��������� ���������� �������
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
            cout << "���������� ���������..." << endl;
            break;
        default:
            cout << "��������� ����� ���� �����������. ��������� ��������." << endl;
		}
		system("pause");
    } while (select != 0);

    return 0;
}
int menu() {
    int select;

    system("cls");

    cout << "������� ����" << endl;
    cout << "1. ������ ������" << endl;
    cout << "2. ���������� � �����" << endl;
    cout << "3. �������� ������ ��������" << endl;
    cout << "4. �������� ��������" << endl;
    cout << "5. �������� �����" << endl;
    cout << "6. ����������� �����" << endl;
    cout << "7. ����������� �����" << endl;
    cout << "8. ���������� �� ��������� �����" << endl;
    cout << "9. ��������� ��������� �����" << endl;
    cout << "10. ���������� �� ��������� ����� �� �����������" << endl;
    cout << "11. ��������� �����" << endl;
    cout << "12. ��������� ���������� �������" << endl;
    cout << "0. ���������� ���������" << endl;

    cout << "> ";
    cin >> select;

    return select;
}

void show_disk_list() {
    DWORD mask;
    mask = GetLogicalDrives();
    cout << "������ ������ �������:" << endl;
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

    cout << "������� �������� �����: ";
    cin >> disk_name;
    path = (disk_name+":\\").c_str();
    drive_type = GetDriveType(path.c_str());

    if (drive_type == DRIVE_UNKNOWN) out +=     "- UNKNOWN           (����������� ���)";
    if (drive_type == DRIVE_NO_ROOT_DIR) out += "- DRIVE NO ROOT DIR (������������ ����)";
    if (drive_type == DRIVE_REMOVABLE) out +=   "- REMOVABLE         (������� ����)";
    if (drive_type == DRIVE_FIXED) out +=       "- FIXED             (������������� ����)";
    if (drive_type == DRIVE_REMOTE) out +=      "- REMOTE            (�������� ����)";
    if (drive_type == DRIVE_CDROM) out +=       "- CDROM             (CD-ROM ����)";
    if (drive_type == DRIVE_RAMDISK) out +=     "- RAMDISK           (RAM ����)";
    out += '\n';

    //GetVolumeInfomation

    if (!GetVolumeInformation(path.c_str(), // ��� ����
                             drive_label,       // �������� File���� ���������
                             sizeof(drive_label), // ������������ ����� ����� File�
                             &drive_sn, // ����� ��������� ������
                             &drive_name_size, // ����� ����������, ������� �������� ������������ ����� ���������� ����� File�
                             &drive_flags, // �����
                             drive_fat, // ����� ����� File���� �������
                             sizeof(drive_fat) // ����� ������ ����� File���� �������
                              )
        )
    {
        out += "������. �� ������� ���������� ������\n";
    }
    else
    {
        out += "�������� ���������� �����: " + string(drive_label) + "\n";
        out += "C������� ����� �����:      " + to_string(drive_sn) + "\n";
        out += "������������ ����� �����:  " + to_string(drive_name_size) + "\n";
        out += "����� �������� ��������:   \n";

        //����������� drive_flags
        if (drive_flags & FILE_CASE_PRESERVED_NAMES)
            out += " -��������� ��� ������������ ����������� ������� ���� File��, ����� �� �������� ��� �� ����.\n";
        if (drive_flags & FILE_CASE_SENSITIVE_SEARCH)
            out += " -��������� ��� ������������ ����� File�� � ������ ��������.\n";
        if (drive_flags & FILE_FILE_COMPRESSION)
            out += " - ��������� ��� ������������ ������ File��.\n";
        if (drive_flags & FILE_NAMED_STREAMS)
            out += " - ��������� ��� ������������ ����������� ������.\n";
        if (drive_flags & FILE_PERSISTENT_ACLS)
            out += " -��������� ��� ��������� � ��������� ������ ���������� �������� (ACL).\n";
        if (drive_flags & FILE_READ_ONLY_VOLUME)
            out += " - ��������� ��� �������� ������ ��� ������.\n";
        if (drive_flags & FILE_SEQUENTIAL_WRITE_ONCE)
            out += " - ��������� ��� ������������ ���� ���������������� ������.\n";
        if (drive_flags & FILE_SUPPORTS_ENCRYPTION)
            out += " -��������� ��� ������������ ������������� File���� ������� (EFS).\n";
        if (drive_flags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
            out += " - ��������� ��� ������������ ����������� ��������.\n";
        if (drive_flags & FILE_SUPPORTS_HARD_LINKS)
            out += " - ��������� ��� ������������ ������� ������.\n";
        if (drive_flags & FILE_SUPPORTS_OBJECT_IDS)
            out += " - ��������� ��� ������������ �������������� ��������.\n";
        if (drive_flags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
            out += " - File���� ������� ������������ �������� �� FileID.\n";
        if (drive_flags & FILE_SUPPORTS_REPARSE_POINTS)
            out += " - ��������� ��� ������������ ����� ���������� �������.\n";
        if (drive_flags & FILE_SUPPORTS_SPARSE_FILES)
            out += " - ��������� ��� ������������ ����������� File�.\n";
        if (drive_flags & FILE_SUPPORTS_TRANSACTIONS)
            out += " - ��������� ��� ������������ ����������.\n";
        if (drive_flags & FILE_SUPPORTS_USN_JOURNAL)
            out += " -��������� ��� ������������ ������� ���������� ���������� ������� (USN).\n";
        if (drive_flags & FILE_UNICODE_ON_DISK)
            out += " -��������� ��� ������������ Unicode � ������ File�� �� ���� �� ��������� �� �����.\n";
        if (drive_flags & FILE_VOLUME_IS_COMPRESSED)
            out += " - ��������� ��� �������� ������ �����.\n";
        if (drive_flags & FILE_VOLUME_QUOTAS)
            out += " - ��������� ��� ������������ �������� �����.\n";

        out += "��� �������� �������:      " + string(drive_fat) + "\n";
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
        out += "- ����� �������� � ��������: " + to_string(drive_sectorsPerCluster) + '\n';
        out += "- ����� ���� � �������: " + to_string(drive_bytesPerSector) + '\n';
        out += "- ����� ��������� ���������: " + to_string(drive_numberOfFreeClusters) +'\n';
        out += "- ����� ����� ���������: " + to_string(drive_totalNumberOfClusters) +'\n';
    }
    cout << out;
}

void create_directory() {
    string direct_name;
    cout << "������� ��� ������������ �������� (������: c:\\folder): ";
    cin >> direct_name;

    //CreateDirectory
    if (!CreateDirectory(direct_name.c_str(), nullptr)) {
        cout << "������� �� ������� �������! ������." << endl;
    }
    else {
        cout << "������� ������." << endl;
    }
}

void remove_directory() {
    string direct_name;
    cout << "������� ��� ���������� �������� (������: c:\\folder): ";
    cin >> direct_name;

    //RemoveDirectory
    if (!RemoveDirectory(direct_name.c_str())) {
        cout << "������� �� ������� �������! ������." << endl;
    }
    else {
        cout << "������� ������." << endl;
    }
}

void create_file() {
    string file_name;
    cout << "������� ��� ����� (������: c:\\folder\\text.txt): ";
    cin >> file_name;

    if (INVALID_HANDLE_VALUE == CreateFile(
                                            file_name.c_str(), // ��������� �� ��� �����
                                            GENERIC_WRITE | GENERIC_READ, // ����� ������� � �����
                                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // ����� ����������� �������������
                                            nullptr, // ��������� �� ���������� ������������
                                            CREATE_ALWAYS, // ��� ���������
                                            FILE_ATTRIBUTE_NORMAL, // ��������� �����
                                            nullptr // ���������� ����� � ���������� ��� �����������
                                          )
        )
    {
        cout << "������. ���: : " << GetLastError() << endl;
    }
    else {
        cout << "���� ������� ������." << endl;
    }
}

void copy_file() {
    string  file_original,
            file_copy;

    cout << "������� ��� ����������� ����� (������: c:\\folder\\text.txt):";
    cin >> file_original;
    cout << "������� ��� �����-�����: ";
    cin >> file_copy;
    if (CopyFile(file_original.c_str(),
                 file_copy.c_str(), false) // ���������� ����� � ��� �� ������ �� ���������
        )
        cout << "File copied" << endl;
    else
        cout << "Code error: " << GetLastError() << endl;
}

void move_file() {
    string  path_from,
            path_to;

    cout << "������� ��� ����� (������: c:\\folder\\text.pdf): ";
    cin >> path_from;
    cout << "������� ���� ��� ����������� (������: c:\\folder\\text.pdf): ";
    cin >> path_to;

    if (MoveFile(path_from.c_str(), path_to.c_str())) {
        cout << "���� ������� ���������." << endl;
    }
    else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        cout << "��� ���������� ����� ���� � ������ ��������. �������� ���? (1 - ��, 0 - ���)" << endl;
        int select;
        cin >> select;
        if (select) {
            if (MoveFileEx(path_from.c_str(), path_to.c_str(), MOVEFILE_REPLACE_EXISTING))
            {
                cout << "���� ��� ������� ������� ���������!" << endl;
            }
            else {
                cout << "������! �� ������� ����������� ����. ��� ������: " << GetLastError() << endl;
            }
        }
    }
    else {
        cout << "������! �� ������� ����������� ����. ��� ������: " << GetLastError() << endl;
    }
}

void get_file_attributes() {
    string path;
    cout << "������� ��� ����� (������: c:\\folder\\text.pdf): ";
    cin >> path;

    DWORD attribute = GetFileAttributes(path.c_str());

    if (attribute == 0) {
        cout << "������! �� ������� �������� �������� �����." << endl;
    }
    else if (attribute == -1) {
        cout << "������! ���� �� ������." << endl;
    }
    else {
        cout << "�������� ����� (" << attribute << "):" << endl;
        if (FILE_ATTRIBUTE_ARCHIVE & attribute) cout <<    "- �������� ����" << endl;
        if (FILE_ATTRIBUTE_COMPRESSED & attribute) cout << "- ������ ����" << endl;
        if (FILE_ATTRIBUTE_DIRECTORY & attribute) cout <<  "- �������" << endl;
        if (FILE_ATTRIBUTE_HIDDEN & attribute) cout <<     "- ������� ���� ��� �������" << endl;
        if (FILE_ATTRIBUTE_NORMAL & attribute) cout <<     "- ���� �� ����� ���������" << endl;
        if (FILE_ATTRIBUTE_READONLY & attribute) cout <<   "- ���� ������ ��� ������" << endl;
    }
}

void set_file_attributes() {
    string path;
    cout << "������� ��� ����� (������: c:\\folder\\text.pdf): ";
    cin >> path;

    DWORD attribute;
    int select;
    cout << "�������� ������� ����� ��:" << endl;
    cout << "1 �������� ����" << endl;
    cout << "2 ������� ����" << endl;
    cout << "3 ���� ��� ���������" << endl;
    cout << "4 ���� ������ ��� ������" << endl;
    cout << "5 ��������� ����" << endl;
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
        cout << "������� ����� ������� �������!" << endl;
    }
    else {
        cout << "������! �� ������� �������� ������� �����. ��� ������: " << GetLastError() << endl;
    }
}

void get_info_by_handle() {
    long FileAttributes;
    BY_HANDLE_FILE_INFORMATION infoFile;
    HANDLE handleFile;

    string path;
    cout << "������� ��� ����� (������: c:\\folder\\text.txt): ";
    cin >> path;

    handleFile = CreateFileA((LPCSTR)path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (GetFileInformationByHandle(handleFile, &infoFile)) {
        FileAttributes = infoFile.dwFileAttributes;
        if (FileAttributes == -1)
            cout << " ���� " << path << " �� ������." << endl;
        else {
            cout << "���������� � ����� �� �����������:" << endl;

            if (FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
                cout << " ���� ��������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                cout << " ���� - ����������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_READONLY)
                cout << " ���� - ������ ��� ������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_SYSTEM)
                cout << " ���� - ���������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_COMPRESSED)
                cout << " ���� - ������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_HIDDEN)
                cout << " ���� - �������." << endl;
            if (FileAttributes & FILE_ATTRIBUTE_NORMAL)
                cout << " ����� ��������� ���� �� �����.";
            if (FileAttributes == 0)
                cout << " ��������� ������. ��� ������: " << GetLastError() << endl;
        }

        cout << " ������� ����� ����������� �������������� ����� � ������: " << infoFile.nFileIndexHigh << endl;
        cout << " ���������� ����� ����, ������� �������� ����: " << infoFile.dwVolumeSerialNumber << endl;
        cout << " ������� ����� ������� �����: " << infoFile.nFileSizeHigh << endl;
        cout << " ������� ����� ������� �����: " << infoFile.nFileSizeLow << endl;
        cout << " ����� ������ � ���� ������: " << infoFile.nNumberOfLinks << endl;
        cout << " ������� ����� ����������� �������������� ����� � ������: " << infoFile.nFileIndexLow << endl;
    }
    else {
        cout << " ��������� ������. ��� ������: " << GetLastError() << endl;
    }

    CloseHandle(handleFile);
    cout << endl;
}

void get_file_time() {
    FILETIME timeCreate, timeLastAccess, timeLastWrite;
    SYSTEMTIME time;

    string path;
    cout << "������� ��� ����� (������: c:\\folder\\text.txt): ";
    cin >> path;

    HANDLE fileHandle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (GetFileTime(fileHandle, &timeCreate, &timeLastAccess, &timeLastWrite))
    {
        FileTimeToSystemTime(&timeCreate, &time);
        cout << "���� � ����� �������� �����:                " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" << time.wSecond << endl;
        FileTimeToSystemTime(&timeLastAccess, &time);
        cout << "���� � ����� ���������� ��������� � �����:  " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" << time.wSecond << endl;
        FileTimeToSystemTime(&timeLastWrite, &time);
        cout << "���� � ����� ��������� ����������� �����:   " <<
            time.wYear << "/" << time.wMonth << "/" << time.wDay << "   " << (time.wHour + 3)%24 << ":" <<
            time.wMinute << ":" <<time.wSecond << endl;
    }
    else {
        cout << "������! �� ������� �������� ���������� � �����. ��� ������: " << GetLastError() << endl;
    }
}

void set_file_time() {
    FILETIME fileTime;
    SYSTEMTIME systemTime;

    string path;
    cout << "������� ��� ����� (������: c:\\folder\\text.txt): ";
    cin >> path;

    HANDLE fileHandle = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &fileTime);
    if (SetFileTime(fileHandle, // ���� � �����
                    nullptr,    // ���� ��������
                    &fileTime,  // ���� ���������� ���������
                    &fileTime)  // ���� ���������� ���������
        )
    {
        cout << "�������� ������� �������� �������!" << endl;
    }
    else {
        cout << "������! ��� ������: " << GetLastError() << endl;
    }
}
