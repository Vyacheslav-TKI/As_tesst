#include <stdio.h>

#define TOTAL_MARKS 10

int main()
{
    float func_1[100] = {2.4, -3.8, 0, 10.2, 11.78, -5.43};
    float func_2[50];

    int size_1 = sizeof(func_1) / sizeof(func_1[0]);
    int size_2 = sizeof(func_2) / sizeof(func_2[0]);
    // копирование
    int min_size = (size_1 < size_2) ? size_1 : size_2;

    for(int i = 0; i < min_size; ++i)
    {
        func_2[i] = func_1[i];
    }

    // for(int i = 0; i < size_2; ++i)
    // {
    //     printf("%.2f ", func_2[i]);
    // }

    // вставка
    int marks[TOTAL_MARKS] = {3, 2, 5};
    int insert_idx = 2;

    for(int i = TOTAL_MARKS - 1; i > insert_idx; --i)
    {
        marks[i] = marks[i-1];
    }

    marks[insert_idx] = 4;

    // for(int i = 0; i < TOTAL_MARKS; ++i)
    // {
    //     printf("%d ", marks[i]);
    // }

    // удаление

    int points[TOTAL_MARKS] = {3, 2, 4, 5, 2, 4};
    int del_idx = 3;

    for(int i = del_idx; i < TOTAL_MARKS - 1; ++i)
    {
        points[i] = points[i+1];
    }

    // сортировка выбором
    int mas[6] = {-3, 5, 0, -8, 1, 10};
    puts("Before ");

    for(int i = 0; i < 6; ++i)
    {
        printf("%d ", mas[i]);
    }

    puts("\nAfter");

    int kas = 0;
    for(int i = 0; i < 5; ++i){
        int min_idx = i;
        for(int j = 1; j < 6; ++j)
        {
            if(mas[j] <= mas[j-1] && mas[j] <= mas[min_idx])
            {
                min_idx = j;
            }
        }
        int *ptr_i = &mas[i], *ptr_j = &mas[min_idx];
        kas = mas[i];
        mas[i] = mas[min_idx];
        mas[min_idx] = kas;
    }

    for(int i = 0; i < 6; ++i)
    {
        printf("%d ", mas[i]);
    }

    return 0;
}