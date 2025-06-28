#include <stdio.h>

int main()
{
    short ar[] = {4, 3, 2, 1, 5, 6, 7};

    short a_1 = *(ar+3);

    short *ptr_ar;
    ptr_ar = ar;
    // ptr_ar = &ar[0]
    *(ptr_ar+1) = -3;

    short x = ptr_ar[2]; // ar[indx] - *(ar-indx)

    for(int i = 0; i < sizeof(ar) / sizeof(*ar); ++i)
    {
        printf("%d\n", *(ar+i));
    }

    return 0;
}