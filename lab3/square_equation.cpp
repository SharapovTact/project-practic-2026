#include <iostream>
#include <cmath>

using namespace std;

typedef struct {
    int a;
    int b;
    int c;
    int D;
} SquareEquation;

void CalculateDiscriminant(SquareEquation& context) {
    context.D = context.b * context.b - 4 * context.a * context.c;
    return;
}

float calcFirstRoot(const SquareEquation& context) {
    return (-context.b + sqrt(context.D)) / (2.0 * context.a);
}

float calcSecondRoot(const SquareEquation& context) {
    return (-context.b - sqrt(context.D)) / (2.0 * context.a);
}

int main() {
    SquareEquation equationContext;
    cout << "Введите коэффициенты a, b, c: ";
    cin >> equationContext.a >> equationContext.b >> equationContext.c;

    CalculateDiscriminant(equationContext);

    if (equationContext.D > 0) {
        cout << "x1: " << calcFirstRoot(equationContext) << endl;
        cout << "x2: " << calcSecondRoot(equationContext) << endl;
    }
    else if (equationContext.D < 0) {
        cout << "x: " << calcFirstRoot(equationContext) << endl;
    }
    else {
        cout << "Нет корней" << endl;
    }
    return 0;
}