#include "stdio.h"
#include "math.h"
#include "complex.h"
#include "stdlib.h"

/**
 * @brief Проверка валидности числа
 */
double check_valid_double();

/**
 * @brief ввод комплексного числа
*/
double complex insert_complex();

/**
 * @brief вычисляет первый корень в случае отрицательного дискриминанта и действ коэффициентов
 */
double complex find_x1_r_c1(double a, double b, double c);

/**
 * @brief вычисляет второй корень в случае отрицательного дискриминанта и действ коэффициентов
 */
double complex find_x2_r_c1(double a, double b, double c);

/**
 * @brief вычисляет сопряженный мнимый корень при действ коэффициентах, b=0 и с однознаковыми a и c
*/
double complex find_x_r_c2(double a, double c);

/**
 * @brief вычисляет первый корень в действ коэффициентах
 */
double find_x1_r(double a, double b, double c);

/**
 * @brief вычисляет второй корень при действ коэффициентах
 */
double find_x2_r(double a, double b, double c);

/**
 * @brief вычисляет первый корень при комплексных коэффициентах
 */
double complex find_x1_c(double complex a, double complex b, double complex c);

/**
 * @brief вычисляет второй корень при комплексных коэффициентах
 */
double complex find_x2_c(double complex a, double complex b, double complex c);

/**
 * @brief точка входа
*/
int main()
{   
    double complex a = insert_complex(), b = insert_complex(), c = insert_complex();

    if (fabs(creal(a)) + fabs(cimag(a)) + fabs(creal(b)) + fabs(cimag(b)) <= __DBL_EPSILON__ && fabs(cimag(a)) + fabs(creal(a)) + fabs(cimag(b)) + fabs(creal(b)) >= -1 * __DBL_EPSILON__)
    {
        if (fabs(creal(c)) + fabs(cimag(c)) > __DBL_EPSILON__ || fabs(creal(c)) + fabs(cimag(c)) < -1 * __DBL_EPSILON__)
        {
            puts("Решений нет\n");
            return 0;
        }

        else
        {
            puts("Логично, что 0=0\n");
            return 0;
        }
    }

    if (fabs(cimag(a)) + fabs(cimag(b)) + fabs(cimag(c)) <= __DBL_EPSILON__ && fabs(cimag(a)) + fabs(cimag(b)) + fabs(cimag(c)) >= -1 * __DBL_EPSILON__)
    {
        if ((creal(a) * creal(b) * creal(c) > __DBL_EPSILON__ || creal(a) * creal(b) * creal(c) < -1 * __DBL_EPSILON__ ) && pow(creal(b), 2) - 4 * creal(a) * creal(c) <= -1 * __DBL_EPSILON__)
        {
            double complex x_1 = find_x1_r_c1(creal(a), creal(b), creal(c)), x_2 = find_x2_r_c1(creal(a), creal(b), creal(c));
            printf("Первый корень %lf + %lfi\n", creal(x_1), cimag(x_1));
            printf("Второй корень %lf + %lfi\n", creal(x_2), cimag(x_2));
            return 0;
        }

        else
        {
            if (creal(b) <= __DBL_EPSILON__ && creal(b) >= -1 * __DBL_EPSILON__ && creal(a) * creal(c) > __DBL_EPSILON__)
            {
                double complex x = find_x_r_c2(creal(a), creal(c));
                printf("Первый корень %lfi\n", cimag(x));
                printf("Второй корень %lfi\n", -1 * cimag(x));

                return 0;
            }

            else
            {
                double x_1 = find_x1_r(creal(a), creal(b), creal(c)), x_2 = find_x2_r(creal(a), creal(b), creal(c));
                if (x_1 == x_2)
                {
                    puts("Корни совпали\n");
                    printf("Корень %lf\n", x_1);
                    return 0;
                }

                else
                {
                    printf("Первый корень %lf\n", x_1);
                    printf("Второй корень %lf\n", x_2);
                    return 0;
                }
            }
        }
    }

    else
    {
        double complex x_1 = find_x1_c(a, b, c), x_2 = find_x2_c(a, b, c);

        if (x_1 == x_2)
        {
            puts("Корни совпали\n");
            printf("Корень %lf + %lfi\n", x_1);
            return 0;
        }

        else
        {
            printf("Первый корень %lf + %lfi\n", creal(x_1), cimag(x_1));
            printf("Второй корень %lf + %lfi\n", creal(x_2), cimag(x_2));

            return 0;
        }
    }

    return 0;
}


double check_valid_double()
{
    double check;
    if (scanf(" %lf", &check) != 1)
    {
        puts("Выберите валидное значение\n");
        abort();
    }
    return check;
}

double complex insert_complex()
{
    puts("Введите действительное значение числа\n");
    double re = check_valid_double();

    puts("Введите мнимое значение числа\n");
    double im = check_valid_double();

    return re + I * im;
}

double complex find_x1_r_c1(double a, double b, double c)
{
    double discr = pow(b, 2) - 4 * a * c;
    double im = sqrt(-1 * discr) / (2 * a), re = (-1 * b) / (2 * a);
    return re + I * im;
}

double complex find_x2_r_c1(double a, double b, double c)
{
    double discr = pow(b, 2) - 4 * a * c;
    return (-1 * b / (2 * a)) - I * (sqrt(-1 * discr) / (2 * a));
}

double complex find_x_r_c2(double a, double c)
{
    return csqrt((-1 * c) / a);
}

double find_x1_r(double a, double b, double c)
{
    if (a * b * c > __DBL_EPSILON__ || a * b * c < -1 * __DBL_EPSILON__)
    {
        double discr = pow(b, 2) - 4 * a * c;
        return (-1 * b + sqrt(discr)) / (2 * a);
    }

    else
    {
        if ((b * c > __DBL_EPSILON__ || b * c < -1 * __DBL_EPSILON__) && a <= __DBL_EPSILON__ && a >= -1 * __DBL_EPSILON__)
        {
            return -1 * c / b;
        }

        else
        {
            if ((a * c > __DBL_EPSILON__ || a * c < -1 * __DBL_EPSILON__) && b <= __DBL_EPSILON__ && b >= -1 * __DBL_EPSILON__)
            {
                return sqrt(-1 * c / a);
            }

            else
            {
                if ((a * b > __DBL_EPSILON__ || a * b < -1 * __DBL_EPSILON__) && c <= __DBL_EPSILON__ && c >= -1 * __DBL_EPSILON__)
                {
                    return 0;
                }

                else
                {
                    puts("Error\n");
                    abort();
                }
            }
        }
    }
}

double find_x2_r(double a, double b, double c)
{
    if (a * b * c > __DBL_EPSILON__ || a * b * c < -1 * __DBL_EPSILON__)
    {
        double discr = pow(b, 2) - 4 * a * c;
        return (-1 * b - sqrt(discr)) / (2 * a);
    }

    else
    {
        if ((b * c > __DBL_EPSILON__ || b * c < -1 * __DBL_EPSILON__) && a <= __DBL_EPSILON__ && a >= -1 * __DBL_EPSILON__)
        {
            return -1 * c / b;
        }

        else
        {
            if ((a * c > __DBL_EPSILON__ || a * c < -1 * __DBL_EPSILON__) && b <= __DBL_EPSILON__ && b >= -1 * __DBL_EPSILON__)
            {
                return -1 * sqrt(-1 * c / a);
            }

            else
            {
                if ((a * b > __DBL_EPSILON__ || a * b < -1 * __DBL_EPSILON__) && c <= __DBL_EPSILON__ && c >= -1 * __DBL_EPSILON__)
                {
                    return -1 * b / a;
                }

                else
                {
                    puts("Error\n");
                    abort();
                }
            }
        }
    }
}

double complex find_x1_c(double complex a, double complex b, double complex c)
{
    if ((creal(a) * creal(b) * creal(c) > __DBL_EPSILON__ || creal(a) * creal(b) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(b) * cimag(c) > __DBL_EPSILON__ || cimag(a) * cimag(b) * cimag(c) < -1 * __DBL_EPSILON__))
    {
        double complex discr = b * b - 4 * a * c;
        return (-1 * b - csqrt(discr)) / (2 * a);
    }
    
    else
    {
        if ((creal(b) * creal(c) > __DBL_EPSILON__ || creal(b) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(b) * cimag(c) > __DBL_EPSILON__ || cimag(b) * cimag(c) < -1 * __DBL_EPSILON__) && creal(a) <= __DBL_EPSILON__ && creal(a) >= -1 * __DBL_EPSILON__ && cimag(a) <= __DBL_EPSILON__ && cimag(a) >= -1 * __DBL_EPSILON__)
        {
            return -1 * c / b;
        }

        else
        {
            if (((creal(a) * creal(c) > __DBL_EPSILON__ || creal(a) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(c) > __DBL_EPSILON__ || cimag(a) * cimag(c) < -1 * __DBL_EPSILON__) && creal(b) <= __DBL_EPSILON__ && creal(b) >= -1 * __DBL_EPSILON__ && cimag(b) <= __DBL_EPSILON__ && cimag(b) >= -1 * __DBL_EPSILON__))
            {
                return -1 * csqrt(-1 * c / a);
            }

            else
            {
                if (((creal(a) * creal(b) > __DBL_EPSILON__ || creal(a) * creal(b) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(b) > __DBL_EPSILON__ || cimag(a) * cimag(b) < -1 * __DBL_EPSILON__) && creal(c) <= __DBL_EPSILON__ && creal(c) >= -1 * __DBL_EPSILON__ && cimag(c) <= __DBL_EPSILON__ && cimag(c) >= -1 * __DBL_EPSILON__))
                {
                    return -1 * b / a;
                }

                else
                {
                    puts("Error\n");
                    abort();
                }
            }
        }
    }
}

double complex find_x2_c(double complex a, double complex b, double complex c)
{
    if ((creal(a) * creal(b) * creal(c) > __DBL_EPSILON__ || creal(a) * creal(b) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(b) * cimag(c) > __DBL_EPSILON__ || cimag(a) * cimag(b) * cimag(c) < -1 * __DBL_EPSILON__))
    {
        double complex discr = b * b - 4 * a * c;
        return (-1 * b + csqrt(discr)) / (2 * a);
    }
    
    else
    {
        if ((creal(b) * creal(c) > __DBL_EPSILON__ || creal(b) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(b) * cimag(c) > __DBL_EPSILON__ || cimag(b) * cimag(c) < -1 * __DBL_EPSILON__) && creal(a) <= __DBL_EPSILON__ && creal(a) >= -1 * __DBL_EPSILON__ && cimag(a) <= __DBL_EPSILON__ && cimag(a) >= -1 * __DBL_EPSILON__)
        {
            return -1 * c / b;
        }

        else
        {
            if (((creal(a) * creal(c) > __DBL_EPSILON__ || creal(a) * creal(c) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(c) > __DBL_EPSILON__ || cimag(a) * cimag(c) < -1 * __DBL_EPSILON__) && creal(b) <= __DBL_EPSILON__ && creal(b) >= -1 * __DBL_EPSILON__ && cimag(b) <= __DBL_EPSILON__ && cimag(b) >= -1 * __DBL_EPSILON__))
            {
                return csqrt(-1 * c / a);
            }

            else
            {
                if (((creal(a) * creal(b) > __DBL_EPSILON__ || creal(a) * creal(b) < -1 * __DBL_EPSILON__) && (cimag(a) * cimag(b) > __DBL_EPSILON__ || cimag(a) * cimag(b) < -1 * __DBL_EPSILON__) && creal(c) <= __DBL_EPSILON__ && creal(c) >= -1 * __DBL_EPSILON__ && cimag(c) <= __DBL_EPSILON__ && cimag(c) >= -1 * __DBL_EPSILON__))
                {
                    return (double complex)0;
                }

                else
                {
                    puts("Error\n");
                    abort();
                }
            }
        }
    }
}