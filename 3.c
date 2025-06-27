#include <stdio.h>

#define MENU_TRANSL 1
#define MENU_ADD 2 * MENU_TRANSL
#define MENU_EXIT 3
#define MENU_FMT "You have selected \
menu item %d\n" // Перенос строки
#define FIRST_LETTER 'a'

int main()
{
    int item;

    scanf("%d", &item);

    switch(item)
    {
        case MENU_TRANSL:
            puts("TR words\n");
            break;
        case MENU_ADD:
            puts("Add words\n");
            break;
        case MENU_EXIT:
            puts("Exit\n");
            break;
        default:
            puts("Incorrect value\n");
    }

    return 0;
}