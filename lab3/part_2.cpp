#include <iostream>
#include <windows.h>
#include <time.h>
#include <omp.h>

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");
    DWORD start, end;
    int count_thr;
    int N = 100000000, block_size = 930814, i;

    cout << "Введите количество потоков - ";
    cin >> count_thr;
    long double x, pi = 0.0;

    start = clock();
    omp_set_num_threads(count_thr);
    #pragma omp parallel for private(x, i) schedule(dynamic, block_size) reduction(+:pi)
    for ( i = 0; i < N; ++i) {
        x = (i + 0.5) / N;
        pi += 4 / (1 + x * x);
    }
    end = clock();

    cout << "Время операции: " << end - start << endl;
    cout.precision(15);
    cout << "Вычисленное значение: " << pi / N;
}
