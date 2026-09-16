#include <iostream>
#include <vector>

using namespace std;

int CalcSum(const vector<int>& array) {
    int sum = 0;
    for (const auto& x: array) {
        sum += x;
    }
    return sum;
} 

float CalcAverage(const int sum, const int size) {
    return float(sum)/size;
}

int FindMin(const vector<int>& array) {
    int min = array[0];
    for (const auto& x: array) {
        if (x < min) {
            min = x;
        }
    }
    return min;
}

int FindMax(const vector<int>& array) {
    int max = array[0];
    for (const auto& x: array) {
        if (x > max) {
            max = x;
        }
    }
    return max;
}

int main() {
    vector<int> array;
    int countNumbers;
    cout << "Введите кол-во чисел: ";
    cin >> countNumbers;
    cout << "Вводите числа: ";
    for (int i = 0; i < countNumbers; i++) {
        int num;
        cin >> num;
        array.push_back(num);
    }
    int sum = CalcSum(array);
    float average = CalcAverage(sum, countNumbers);
    int min = FindMin(array);
    int max = FindMax(array);
    cout << "Stat:" << "\nSum: " << sum << "\nAverage: " << average << "\nMin: " << min << "\nMax: " << max << endl;
    return 0;
}