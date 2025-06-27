#include <stdio.h>

#define SQ_PR(A, B) ((A) * (B)) // макрос функция
#define TEXT(A, B) "Square of rect (" #A ") x (" #B ")\n"
#define X_N(N) x ## N

int main()
{
    int res = SQ_PR(2, 3);
    int x1 = 1, x2  = -2, x4 = 10;
    printf("%d\n", X_N(4)); // выведет x4

    printf("res = %d\n", res);
    printf(TEXT(x-2, y-3));
    return 0;
}