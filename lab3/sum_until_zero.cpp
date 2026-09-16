#include <iostream>

using namespace std;

int main() {
    int num = 1;
    int countNumbers = 0;

    cout << "Вводите числа, чтобы остановить ввод - введите 0: \n";
    while (num != 0) {
        cin >> num;
        if (num == 0 && countNumbers == 0) {
            return 1;
        }
        if (num != 0) {
            countNumbers++;
        }
    }
    
    for (int i = 0; i < countNumbers; i++) {
        cout << "*";
    }
    cout << endl;
    return 0;
}