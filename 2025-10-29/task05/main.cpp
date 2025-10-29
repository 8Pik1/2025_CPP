// Дан массив из n целых чисел. Выполни по нему следующие действия: Подсчёт чётных и
// нечётных чисел; Сумма чисел, кратных 3; Количество чисел, кратных первому элементу массива; 
// Поиск числа с наибольшим количеством делителей; Проверить, есть ли в массиве число, 
// которое нечётное, делится на 5, и больше среднего арифметического всех элементов.

#include <iostream>
using namespace std;

int main()
{
    const int n = 8;
    int a[n] = {12, 25, 9, 30, 15, 6, 10, 18};
    int even = 0;
    int odd = 0;
    int sum3 = 0;
    int cnt1 = 0;
    int max_cnt = 0;
    int num_max = a[0];
    double avg = 0;
    bool flag = false;
    
    for (int i = 0; i < n; ++i) {
        avg += a[i];
    }
    avg /= n;
    
    for (int i = 0; i < n; ++i) {
        if (a[i] % 2 == 0) {
            ++even;
        }
        else {
            ++odd;
        }
        
        if (a[i] % 3 == 0) {
            sum3 += a[i];
        }
        
        if (a[i] % a[0] == 0) {
            ++cnt1;
        }
        
        int cnt = 0;
        for (int j = 1; j <= a[i]; ++j) {
            if (a[i] % j == 0) {
                ++cnt;
            }
        }
        if (cnt > max_cnt) {
            max_cnt = cnt;
            num_max = a[i];
        }
        
        if (a[i] % 2 != 0 && a[i] % 5 == 0 && a[i] > avg) {
            flag = true;
        }
    }
    
    cout << "Массив: ";
    for (int i = 0; i < n; ++i) {
        cout << a[i] << ' ';
    }
    cout << '\n';
    
    cout << "Четные: " << even << '\n';
    cout << "Нечетные: " << odd << '\n';
    cout << "Сумма кратных 3: " << sum3 << '\n';
    cout << "Кратных первому: " << cnt1 << '\n';
    cout << "Число с макс делителями: " << num_max << " (" << max_cnt << ")" << '\n';
    cout << "Среднее: " << avg << '\n';
    cout << "Есть особое число: " << (flag ? "Да" : "Нет") << '\n';
    
    return 0;
}