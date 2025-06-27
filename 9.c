#include <stdio.h>

int main()
{
    const int pin = 13;

    int enter, c;

    do{ // сначала итерация - потом условие
        puts("Enter a code\n");
        scanf("%d", &enter);

        while ((c = getchar()) != '\n' && c != EOF)
            { }
    } while(enter != pin);

    puts("Access permited\n");

    return 0;
}