// Дан массив целых чисел a[0..N−1]. Требуется найти самый длинный подотрезок, на котором знаки соседних разностей (a[i+1]−a[i]) строго чередуются, нулевые разности прерывают подотрезок. Вывести длину и левую границу такого подотрезка; при нескольких ответах выбрать самый левый.

#include <iostream>
using namespace std;

int main()
{
    const int n = 10;
    int a[n] = {1, 3, 2, 4, 1, 5, 3, 7, 4, 6};
    int max_len = 1;
    int start = 0;
    int curr_len = 1;
    int curr_start = 0;
    
    for (int i = 1; i < n; ++i) {
        int diff = a[i] - a[i-1];
        
        if (diff == 0) {
            curr_len = 1;
            curr_start = i;
        }
        else if (i > 1) {
            int prev_diff = a[i-1] - a[i-2];
            if (diff * prev_diff < 0) {
                ++curr_len;
            }
            else {
                curr_len = 2;
                curr_start = i-1;
            }
        }
        else {
            curr_len = 2;
            curr_start = i-1;
        }
        
        if (curr_len > max_len) {
            max_len = curr_len;
            start = curr_start;
        }
    }
    
    cout << "Массив: ";
    for (int i = 0; i < n; ++i) {
        cout << a[i] << ' ';
    }
    cout << '\n';
    
    cout << "Длина: " << max_len << '\n';
    cout << "Начало: " << start << '\n';
    
    return 0;
}