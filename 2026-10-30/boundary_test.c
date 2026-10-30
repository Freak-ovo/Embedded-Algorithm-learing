#include <stdio.h>



int linear_search(const int numbers[], int length, int target) {

    for (int i = 0; i < length; i++) {
        if (numbers[i] == target) {
            return i;
        }
    }
    return -1;
}

void run_test(const char *test_name, const int numbers[], int length, int target, int expected) {
    int actual = linear_search(numbers, length, target);
    if (actual == expected) {
        printf("[PASS] %s\n", test_name);
    }
    else {
        printf("[FAIL] %s: expected = %d, actual = %d\n", test_name, expected, actual);
        
    }
}
int main() {
    int numbers[] = {10, 20, 30, 40, 50};
    int length = sizeof(numbers) / sizeof(numbers[0]);

    int target_10 = 10;
    run_test("find frist element", numbers, length, target_10, 0);
    
    int target_50 = 50;
    run_test("find last element", numbers, length, target_50, 4);

    int target_100 = 100;
    run_test("find last element", numbers, length, target_100, 0);

    int single[] = {42};
    run_test("single element found", single, 1, 42, 0);
    run_test("single element not found", single, 1, 100, -1);

    run_test("empty  range", numbers, 0, 10, -1);
}