#include <stdio.h>


int find_max(const int numbers[], int length) {
    int max = numbers[0];

    for (int i = 0; i < length; i++) {
        if (numbers[i] > max) {
            max = numbers[i];
        }
    }
    return max;
}

int calculate_sum(const int numbers[], int length) {
    int sum = 0;

    for (int i = 0; i < length; i++) {
        sum += numbers[i];

    }
    return sum;
}

int main() {
    int numbers[] = {12, 5, 27, 8, 19};
    int length = sizeof(numbers) / sizeof(numbers[0]);

    int max = find_max(numbers, length);
    int sum = calculate_sum(numbers, length);

    printf("max = %d \n", max);
    printf("sum = %d \n", sum);

    return 0;
}