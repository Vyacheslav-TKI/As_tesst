#define LANG_C
 // проверка условия определения макроса LANG_C
#if defined(LANG_C) 
#   include <stdio.h>
#else
#   include <iostream>
#endif

int main()
{
    int x = 5;
#ifdef LANG_C // также как и if defined
    printf("%d\n", x);
#else
    std::cout << x << std::endl;
#endif
    return 0;
}