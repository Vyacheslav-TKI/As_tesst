#include <stdio.h>
#include <math.h>
#include <stdbool.h>

/**
 * @brief Безопасный ввод целочисленных значений
 */
int safe_insert_d();

/**
 * @brief Безопасный ввод положительных целочисленных значений
 */
size_t safe_insert_size_t();

/**
 * @brief вычисляет количество цифр
 */
int count_numbers(int *num);

/**
 * @brief вычисляет количесво цифр но в size_t
 */
size_t count_numbers_t(size_t *num);

/**
 * @brief переворачивает положительное число
 */
size_t reverse_number(size_t *snum);

/**
 * @brief проверяет число на его простоту
 */
bool is_simple(int *digit);

/**
 * @brief меняет указатели местами, без лишних переменных
 */
void swap(int *a, int *b);

/**
 * @brief точка входа в программу
 */
int main()
{
    puts("Task 1\n");
    int numb = safe_insert_d();
    int *ptr_1 = &numb;
    int res_1 = count_numbers(ptr_1);
    printf("Task 1 ans: %d\n", res_1);

    puts("Task 2\n");
    size_t numb_2 = safe_insert_size_t();
    size_t *ptr_2 = &numb_2;
    size_t res_2 = reverse_number(ptr_2);
    printf("Task 2 ans: %d\n", res_2);

    puts("Task 3\n");
    int diga = safe_insert_d();
    int *ptr_3 = &diga;
    bool simp = is_simple(ptr_3);
    printf("Task 3 ans: %s\n", simp ? "true" : "false"); 

    puts("Task 4\n");
    int lha = safe_insert_d();
    int rha = safe_insert_d();
    int *pla = &lha, *pra = &rha;
    printf("Before: a = %d, b = %d\n", *pla, *pra);
    swap(pla, pra);
    printf("After: a = %d, b = %d\n", *pla, *pra);
    
    return 0;
}

int safe_insert_d()
{
    int res, success;
    do{
        puts("Insert a digit\n");
        success = scanf("%d", &res);
        if (success != 1)
        {
            puts("Try again\n");
            int c;
            while((c = getchar()) != '\n' && c != EOF){}
        }
    }while(success != 1);

    return res;
}

size_t safe_insert_size_t()
{
    int res;
    int success;
    do{
        puts("Insert a digit\n");
        success = scanf("%d", &res);
        if (success != 1 || res <= -1 * __DBL_EPSILON__)
        {
            puts("Try again\n");
            int c;
            while((c = getchar()) != '\n' && c != EOF){}
            success = 0;
        }
    }while(success != 1);

    return (size_t)res;
}

int count_numbers(int *num)
{
    int count = 1;
    int nnum = *num;
    while(nnum / 10 > 0)
    {
        count++;
        nnum /= 10;
    }
    return count;
}

size_t count_numbers_t(size_t *num)
{
    size_t count = 1, nnum = *num;
    while(nnum / 10 > 0)
    {
        count++;
        nnum /= 10;
    }
    return count;
}

size_t reverse_number(size_t *snum)
{
    size_t nnum = *snum, cst = *snum;
    size_t res = 0;
    int len;

    for(len = (int)count_numbers_t(&cst) - 1;len >= 0; --len)
    {
        res += (nnum % 10) * pow(10, len);
        nnum /= 10;
    }

    return res;
}

bool is_simple(int *digit)
{
    int adig = fabs(*digit);
    for(int i = 2; i < adig; i++)
    {
        if (adig % i == 0)
        {
            return false;
        }
    }

    return true;

}

void swap(int *a, int *b)
{
    *a = *a ^ *b; // ^ - сложение по мод 2, тут идет сумма адресов a+b
    *b = *a ^ *b; // тут идет вычитание из суммы - адреса b
    *a = *a ^ *b; // тут идет вычитание из суммы - адреса b, который стал адресом a 
}