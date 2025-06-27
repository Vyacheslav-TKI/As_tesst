#include <stdio.h>

#define FIVE 5
#define TEN 2 * FIVE
#undef TEN // отмена верхнего макроса
#define TEN 10
#define PRINT_D printf("Digit = %d\n", digit)
#define FORMAT "Digit=%d\n"

int main()
{
    int digit = FIVE;
    PRINT_D;

    digit = TEN;
    printf(FORMAT, digit);

    return 0;
}