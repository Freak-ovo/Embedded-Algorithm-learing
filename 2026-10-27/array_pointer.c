#include<stdio.h>



int main() {

    int number[] = {10, 20, 30, 40, 50};
    int length;

    length = sizeof(number) / sizeof(number[1]);
    for (int i = 0; i < length; i++) {
        printf("index = %d\n", i);

        printf("subscript = %d\n", number[i]);

        printf("pointer value = %d\n", *(number + i));

        printf("subscript addr = %p\n", (void *)&number[i]);
        printf("pointer value = %p\n", (number + i));

    }

    return 0;
}