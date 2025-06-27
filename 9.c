#include <stdio.h>

int main()
{
    const int pin = 13;

    int enter, c;

    do{ // сначала итерация - потом условие
        puts("Enter a code\n");
        if (scanf("%d", &enter) != 1)
        {
            goto pass; // идем по метке
        }

        while ((c = getchar()) != '\n' && c != EOF)
            { }
    } while(enter != pin);

    puts("Access permited\n");

    pass: return 1; // метка

    return 0;
}