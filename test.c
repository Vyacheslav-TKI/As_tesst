#include "stdio.h"
#include "math.h"

int main()
{
    int a = 2, b = 5;

    if (a < b)
    {
        printf("%d", (int)pow(a, b));
        return 0;
    }

    if (a == b)
    {
        return 0;
    }
    return 0;
}