// Дан одномерный массив из целых чисел. Сформировать два новых массива: 
// один — из положительных элементов исходного массива, другой — из отрицательных.
// Если элемент равен нулю, не включать его ни в один из новых массивов.

#include <iostream>
using namespace std;

int main()
{
    cout << " 123pppp132" << endl;
    const int N = 10;
    int a[N] = {-9, 16, 14, -44, 875, 0, -1, -8, 68, -72};
    int b[N];
    int c[N];
    int b_count = 0;
    int c_count = 0;
    for (int i = 0; i < N; ++i) {
        if (a[i] > 0){
            b[b_count] = a[i];
            ++b_count;
        }
        else if (a[i] < 0){
            c[c_count] = a[i];
            ++c_count;
        }
        cout << a[i] << ' ';
    }
    cout << '\n';
    for (int i = 0; i < b_count; ++i) {
        cout << b[i] << ' ';
    }
    cout << '\n';
    for (int i = 0; i < c_count; ++i) {
        cout << c[i] << ' ';
    }
    cout << endl;
}




