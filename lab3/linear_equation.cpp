#include <iostream>

int main()
{
    float a = 0.0f;
    float b = 0.0f;

    std::cout << "введите коэф. 'a' и 'b', для уравнения 'ax + b = 0': ";
    std::cin >> a >> b;

    if (a != 0.0f)
    {
        float x = -b / a;
        std::cout << "x = " << x << std::endl;
    }
    else
    {
        std::cout << "Ошибка, коэф. 'a' не может быть равен 0!" << std::endl;
    }

    return 0;
}
