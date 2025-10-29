//Дан массив из n чисел. Найти максимальный элемент и заменить все элементы, стоящие после него, на единицы.

#include <iostream>
using namespace std;

int main()
{
    const int N = 9;
    int arr[N] = {3, 8, 2, 15, 7, 4, 12, 6, 9};
    int max_val = arr[0];
    int max_idx = 0;
    
    for (int i = 1; i < N; ++i) {
        if (arr[i] > max_val) {
            max_val = arr[i];
            max_idx = i;
        }
    }
    
    for (int i = max_idx + 1; i < N; ++i) {
        arr[i] = 1;
    }
    
    cout << "Результат: ";
    for (int i = 0; i < N; ++i) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
    
    cout << "Максимальный элемент: " << max_val << '\n';
    cout << "Индекс максимального: " << max_idx << '\n';
    
    return 0;
}