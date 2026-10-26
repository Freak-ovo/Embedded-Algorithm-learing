#include <stdio.h>

int main(void) {

    //int numbers[] = { 3, 7, 2, 9, 5};
    int numbers[] = { -8, 3, -2, 10};
    
    int length = 0; /* TODO：计算数组元素数量 */
    int sum = 0;    /* TODO: 累加前初值 */

    /* 计算数组长度 = 数组总字节数/ 单个元素字节 */
    length = sizeof(numbers) / sizeof(numbers[0]);
    for (int i = 0; i < length; i++){

        sum += numbers[i];
        printf("i = %d, value = %d, sum before = %d\n", i, numbers[i], sum);

    }

    printf("sum = %d\n", sum);


}
/* for循环的时间复杂度是o(n) */