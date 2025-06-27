#include <stdio.h>

int factorial(int num);

int main()
{
    int n = 0;

    if (scanf("%d", &n) != 1)
    {
        return 1;
    }

    printf("factor: %d", factorial(n));
    return 0;
}

int factorial(int num)
{
    int s = 1;
    int i = 0; // задает максимум итераций
    while (++i <= num && i <= 100) // сначала инкремент потом сверка с условием
    {
        s *= num;
        num--; // сначала условие потом декремент иначе --num
    }
    return 2*s;
}