#include <stdio.h>

#define TOTAL_MARKS 13

int main()
{
    int arr[TOTAL_MARKS] = {1, 2, 3, 4}; // остальные нули
    int marks[TOTAL_MARKS];
    marks[0] = 2 * 3;
    marks[2] = 4;
    marks[12] = 7 * 2;

    size_t bytes_marks = sizeof(marks);
    printf("%zu\n", bytes_marks);

    size_t size_marks = bytes_marks / sizeof(marks[0]);
    printf("%zu\n", size_marks);

    for(int i = 0; i < TOTAL_MARKS; ++i)
    {
        printf("a[%d] = %d", i, marks[i]);
    }

    return 0;
}