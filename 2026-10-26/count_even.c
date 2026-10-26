#include <stdio.h>

/* 封装成函数 */
int count_even(const int numbers[], int length) {

    int even_cont;  /* 初始偶数值 */
    
    for (int i = 0; i < length; i++) {
        if (numbers[i] % 2 == 0) {

            even_cont += 1;
            printf("even: %d\n", numbers[i]);
        }
        else {
            printf("不是偶数 %d\n", numbers[i]);
        }

    }
    return even_cont;
}
int main() {


    int numbers[] = {12, 5, 27, 3, 18, 9, 4};

    int length, even, even_cont, result;

    length = sizeof(numbers) / sizeof(numbers[0]);

    for (int i = 0; i < length; i++) {
        if (numbers[i] % 2 == 0) {

            even_cont += 1;
            printf("even: %d\n", numbers[i]);
        }

    }

    printf("even_cont: %d\n", even_cont);
    printf("函数输出：\n");

    result = count_even(numbers, length);
    printf("even_conut:  %d\n", result);

}



