#include <stdio.h>

void ptrs_1();

void ptrs_2();

int main()
{
    ptrs_1();
    ptrs_2();

    int g = 4;
    int *ptr = &g;

    printf("ptr = %u\n", ptr);

    ptr++;
    printf("ptr = %u\n", ptr);
   

    return 0;
}

void ptrs_1()
{
    int d = 10;
    int *ptr = &d;

    printf("ptr = %p, *ptr = %d, d = %d\n", ptr, *ptr, d);

    *ptr = 75432;

    printf("ptr = %p, *ptr = %d, d = %d\n", ptr, *ptr, d);
}

void ptrs_2()
{
    int arg = 7;
    int *ptr_1 = &arg, *ptr;

    ptr = ptr_1;
    printf("*ptr = %d, arg = %d\n", *ptr, arg);

    *ptr_1 = 100;
    printf("ptr = %d, arg = %d\n", *ptr, arg);
}