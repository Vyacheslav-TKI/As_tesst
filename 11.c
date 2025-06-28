#include <stdio.h>

#define TOTAL_MARKS 13

int main()
{
    // double f[30];
    // char buffer[SIZE_BUFFER];

    // int marks[13];
    // short ar[8*5];
    // char bytes[sizeof(double)];

    int marks[TOTAL_MARKS];

    marks[2] = 4;
    marks[0] = 2 * 3;
    marks[12] = 7 - 2;

    int x = marks[2];
    printf("x = %d\n", x);

    return 0;
}