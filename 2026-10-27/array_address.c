#include<stdio.h>


/* 数组

    1. number[0] 数组第一个元素值 是 10
    2. &number[0] 是数组第一个元素地址

 */

int main() {

    int numbers[] = {10, 20, 30, 40, 50};

    int length;

    length = sizeof(numbers) / sizeof(numbers[0]);

    printf("sizeof(int) = %zu bytes\n", sizeof(int)) ;

    for (int i = 0; i < length; i++) {
        printf("index = %d, value = %d, address = %p \n", i, numbers[i], &numbers[i]);

    }

    return 0;
}
