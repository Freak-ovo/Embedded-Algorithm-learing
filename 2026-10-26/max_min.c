#include <stdio.h>


int main(){

    int numbers[] = {12, 5, 27, 3, 18, 9};
    int MAX, MIN, length;
    
    length = sizeof(numbers) / sizeof(numbers[0]);

    /* 初始化 
        初始化不能选0，因为这个写法不可靠，当数组数字都大于0，所以最终程序可能错误的认为min = 0；
        同理当数组元素都小于0了，程序会认为max=0；*/
    MAX = numbers[0];
    MIN = numbers[0];

    for (int i = 0; i < length; i++) {

        printf("before: i = %d, current = %d, MAX = %d, MIN = %d\n", i, numbers[i], MAX, MIN);

        if (MAX < numbers[i]) {
            MAX = numbers[i];
        }
        else if (MIN > numbers[i]) {
            MIN = numbers[i];
        }

        printf("after: i = %d, current = %d, MAX = %d, MIN = %d\n", i, numbers[i], MAX, MIN);

    }


    printf("MAX = %d, MIN = %d,\n", MAX, MIN);

}

/* 复杂度分析
    1. 时间复杂度
        假设循环数组有n个元素
        循环从第二个开始，一共处理n-1个元素，每轮进行一次最大值，最小值比较，
        所以执行次数可以近似为2(n-1)
        去掉常数阶后是 o(n)
    2. 空间复杂度
        length max min i 这些变量不会随着数组增长，因此额外空间复杂度为 o(1)
    */