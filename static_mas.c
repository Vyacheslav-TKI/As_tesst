#include <stdio.h>

/**
 * @brief Заполнение статического массива
 */
void insert_mas(int *p_mas, int n);

/**
 * @brief Вывод массива
 */
void print_m(int *p_mas, int len);

/**
 * @brief Ввод пользователем числа
 */
int safe_insert_d();

/**
 * @brief Вычисляет среднее арифметическое из 10 чисел
 */
float sr_ar_10();

/**
 * @brief Обеспечивает циклический сдвиг
 */
void cyclic_code();

/**
 * @brief Подсчитывает положительные элементы массива
 */
int count_pozitive(int *p_mas, int len);

/**
 * @brief Подсчитывает отрицательные элементы массива
 */
int count_negative(int *p_mas, int len);

/**
 * @brief Подсчитывает нули массива
 */
int count_zeros(int *p_mas, int len);

/**
 * @brief Выводит статистику значений массива
 */
void count_values();

/**
 * @brief Точка входа в программу
 */
int main()
{

    float task_1 = sr_ar_10();
    printf("Task 1: %.2f\n", task_1);

    puts("Task 2\n");
    cyclic_code();

    puts("Task 3\n");
    count_values();

    return 0;
}

int safe_insert_d()
{
    int success, res;
    do{
        puts("Insert a digit\n");
        success = scanf("%d", &res);
        if(success != 1)
        {
            puts("Try again\n");
            int c;
            while((c = getchar() != '\n' && c != EOF));
        }
    }while(success != 1);
    return res;
}

void insert_mas(int *p_mas, int n)
{
    for(int i = 0; i < n; ++i)
    {
        p_mas[i] = safe_insert_d();
    }
}

void print_m(int *p_mas, int len)
{
    for(int i = 0; i < len; ++i)
    {
        printf("a[%d] = %d\n", i, *(p_mas + i));
    }
}

float sr_ar_10()
{
    int mas[10];
    int *p_mas, len = 10;
    p_mas = mas;

    puts("Insert 10 numbers\n");
    insert_mas(p_mas, len);

    int sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += mas[i];
    }

    return sum / len; 

}

void cyclic_code()
{
    int mas[6], len = 6;
    int *p_mas;

    p_mas = mas;

    puts("Insert a 6 digits\n");
    insert_mas(p_mas, len);
    puts("Before\n");
    print_m(p_mas, len);

    int k = mas[5];

    for(int i = len; i > 0; i--)
    {
        mas[i] = mas[i-1];
    }

    mas[0] = k;
    
    puts("After\n");

    print_m(p_mas, len);
}

int count_pozitive(int *p_mas, int len)
{
    int count = 0;
    for(int i = 0; i < len; ++i)
    {
        if(p_mas[i] > 0)
        {
            count++;
        }
    }
    return count;
}

int count_negative(int *p_mas, int len)
{
    int count = 0;
    for(int i = 0; i < len; ++i)
    {
        if(p_mas[i] < 0)
        {
            count++;
        }
    }
    return count;
}

int count_zeros(int *p_mas, int len)
{
    int count = 0;
    for(int i = 0; i < len; ++i)
    {
        if(p_mas[i] == 0)
        {
            count++;
        }
    }
    return count;
}

void count_values()
{
    puts("Insert 15 digits\n");
    int mas[15];
    int *p_mas, len = 15, poz, neg, zer;

    p_mas = mas;
    insert_mas(p_mas, len);

    poz = count_pozitive(p_mas, len);
    neg = count_negative(p_mas, len);
    zer = count_zeros(p_mas, len);

    printf("Pozitives: %d\n", poz);
    printf("Negatives: %d\n", neg);
    printf("Zeros: %d\n", zer);

}