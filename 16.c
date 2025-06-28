#include <stdio.h>

int main()
{
    char matrix[3][3] = {1, 2, 3, 4};
    char (*p_ar)[3] = matrix; // указатель на двумерный массив

    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            printf("%d ", matrix[i][j]);
        }
        puts("\n");
    }
    return 0;
}