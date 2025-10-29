// Дан двумерный массив 4 x 4, требуется выполнить следующие операции: умножение на скаляр, найти суммы по строкам и столбцам, заменить отрицательные значения на 10, поиск минимума и максимума, сложение двух массивов 4 x 4, поэлементное сравнение двух массивов 4 x 4.

#include <iostream>
using namespace std;

int main()
{
    const int n = 4;
    int a[n][n] = {
        {1, -2, 3, 4},
        {5, 6, -7, 8},
        {-9, 10, 11, 12},
        {13, 14, 15, -16}
    };
    int b[n][n] = {
        {2, 3, 1, 5},
        {4, 2, 6, 3},
        {1, 7, 2, 4},
        {5, 3, 4, 2}
    };
    int c[n][n];
    
    int scalar = 3;
    cout << "Умножение на скаляр " << scalar << ":\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << a[i][j] * scalar << ' ';
        }
        cout << '\n';
    }
    
    cout << "Суммы по строкам:\n";
    for (int i = 0; i < n; ++i) {
        int sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += a[i][j];
        }
        cout << sum << ' ';
    }
    cout << '\n';
    
    cout << "Суммы по столбцам:\n";
    for (int j = 0; j < n; ++j) {
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += a[i][j];
        }
        cout << sum << ' ';
    }
    cout << '\n';
    
    cout << "Замена отрицательных на 10:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (a[i][j] < 0) {
                cout << 10 << ' ';
            }
            else {
                cout << a[i][j] << ' ';
            }
        }
        cout << '\n';
    }
    
    int min_val = a[0][0];
    int max_val = a[0][0];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (a[i][j] < min_val) min_val = a[i][j];
            if (a[i][j] > max_val) max_val = a[i][j];
        }
    }
    cout << "Минимум: " << min_val << " Максимум: " << max_val << '\n';
    
    cout << "Сложение двух массивов:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = a[i][j] + b[i][j];
            cout << c[i][j] << ' ';
        }
        cout << '\n';
    }
    
    cout << "Поэлементное сравнение (1 если a > b):\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << (a[i][j] > b[i][j] ? 1 : 0) << ' ';
        }
        cout << '\n';
    }
    
    return 0;
}