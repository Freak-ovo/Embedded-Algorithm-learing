#include <stdio.h>

int linear_search(const int numbers[], int length, int target)
{
    int comparisons = 0;

    for (int i = 0; i < length; i++) {
        comparisons++;

        if (numbers[i] == target) {
            printf("linear search comparisons = %d\n", comparisons);
            return i;
        }
    }
    return -1;
}

int binary_search(const int numbers[], int length, int target) {
    int left = 0;
    int right = length - 1;
    int comparisons = 0;

    while (left <= right) {
        comparisons++;

        int middle = left + (right - left) / 2;

        if (numbers[middle] == target)
        {
            printf("binary search comparisons = %d\n", comparisons);

            return middle;
        }
        else if (numbers[middle] > target) {
            right = middle - 1;

        }
        else if (numbers[middle] < target){
            left = middle + 1;

        }
    }
    return 0;
}

int main(void) {
    int numbers[] = {
        2, 5, 8, 12, 16,
        23, 38, 56, 72, 91
    };
    int length;
    length = sizeof(numbers) / sizeof(numbers[0]);
    int target = 72;

    int linear_index = linear_search(numbers, length, target);
    int binary_index = binary_search(numbers, length, target);

    if (linear_index >= 0) {
        printf("linear search found target at index %d\n", linear_index);
    }
    else {
        printf("linear search did not find target\n");
    }
    if (binary_index >= 0) {
        printf("binary search found target at index %d\n", binary_index);
    }
    else {
        printf("binary search did not find target\n");
    }

    return 0;
}