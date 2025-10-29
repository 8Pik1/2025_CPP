// Дан массив целых чисел. Найти длину самой длинной последовательности (числа) подряд идущих чётных чисел.

#include <iostream>
using namespace std;

int main()
{
    const int N = 10;
    int arr[N] = {2, 4, 7, 8, 10, 12, 3, 6, 8, 14};
    int max_len = 0;
    int curr_len = 0;
    
    for (int i = 0; i < N; ++i) {
        if (arr[i] % 2 == 0) {
            ++curr_len;
        }
        else {
            if (curr_len > max_len) {
                max_len = curr_len;
            }
            curr_len = 0;
        }
    }
    
    if (curr_len > max_len) {
        max_len = curr_len;
    }
    
    cout << "Массив: ";
    for (int i = 0; i < N; ++i) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
    
    cout << "Максимальная длина четной последовательности: " << max_len << '\n';
    
    return 0;
}