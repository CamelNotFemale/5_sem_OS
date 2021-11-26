#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;

DWORD block_size = 930814;
DWORD N = 100000000;
long double pi = 0.0;
int count_thr;
int current_pos = 0;

LPCRITICAL_SECTION section = new CRITICAL_SECTION;

DWORD WINAPI calc_pi(LPVOID lpParam) {
    int* first = (int*)lpParam;
    int end = *first + block_size;
    long double x = 0.0, sl = 0.0;

    do {
        sl = 0.0;
        for (int i = *first; (i < end) && (i < N); ++i) {
            x = (i + 0.5) / N;
            sl += (4 / (1 + x * x));
        }
        EnterCriticalSection(section);
        pi += sl;
        current_pos += block_size;
        *first = current_pos;
        LeaveCriticalSection(section);
        end = *first + block_size;
    } while (*first < N);

    return 0;
}

int main()
{
    setlocale(LC_ALL, "rus");

    DWORD start, end;

    cout << "Введите количество потоков - ";
    cin >> count_thr;

    HANDLE* Threads = new HANDLE[count_thr];
    int* position = new int[count_thr];
    InitializeCriticalSection(section);

    for (int i = 0; i < count_thr; ++i) {
        position[i] = block_size * i;
        current_pos = position[i];
        Threads[i] = CreateThread(NULL, 0, calc_pi, &position[i], CREATE_SUSPENDED, NULL);
    }

    start = clock();
    for (int i = 0; i < count_thr; ++i) {
        ResumeThread(Threads[i]);
    }
    WaitForMultipleObjects(count_thr, Threads, TRUE, INFINITE);
    end = clock();

    cout << "\nВремя работы: " << end - start << endl;
    cout.precision(15);
    cout << "Вычисленное значение: " << pi / N << "\n";

    DeleteCriticalSection(section);
}
