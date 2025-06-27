#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, s;

    scanf("%d", &n);

    for(s=0; n > 0; --n) // 1 - условие 2 - декремент 3 - операция
    { // 4 - декремент 5 - условие 6 - ...
        s += n*n;
    }

    printf("%d\n", s);

    for(int x = rand() % 10; x != 0; x = rand() % 10)
    {
        printf("%d\n", x);
    }

    return 0;
}