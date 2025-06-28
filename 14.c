#include <stdio.h>

int main()
{
    short ar[] = {4, 3, 2, 1, 5, 6, 7};
    const short *ptr_ar; // По этому указателю нельзя записывать значения
    short * const ptrar; // Адрес указателя постоянный, менять ячейку массива можно
    
    ptr_ar = ar;
    short x = ptr_ar[0];
    ptr_ar++;

    return 0;
}