#include <stdio.h>

int calculate_sum(const int numbers[], int length) {
    //int sum;        /* x sum没有初始化 */
    int sum = 0; 
    for (int i = 0; i < length; i++) {
        sum += numbers[i];
        printf("i = %d, numbers[i] = %d sum = %d \n", i, numbers
        [i], sum);
    }
    return sum;
}

int main(void) {
    int numbers[] = {10, 20, 30, 40, 50};
    int length = sizeof(numbers) / sizeof(numbers[0]);

    int result = calculate_sum(numbers, length);

    printf("sum = %d\n", result);
    return 0;
}