#include <stdio.h>

#define BUFFER_CAPACITY 5

static int tests_run = 0;
static int test_passed = 0;

int buffer_add(int buffer[], int capcity, int *size, int value) {
    
    if (*size >= capcity) {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;
    return 1;
}

int buffer_remove(int buffer[], int *size, int index) {
    if (index < 0 || index >= *size) {
        return 0;
    }
    else {
        for (int i = index; i < *size -1; i++) {

            buffer[i] = buffer[i + 1];
        }
        (*size)--;
        return 1;
    }
}

void buffer_print(const int buffer[], int size) {
    printf("buffer: ");

    for (int i = 0; i < size; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}


void check_int(const char *test_name, int actual, int expected) {
    tests_run++;

    if (actual == expected) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    }
    else {
        printf("[FAIL] %s : expected = %d, actual = %d\n", test_name, expected, actual);
    }
}


int main() {



    int buffer[BUFFER_CAPACITY];
    int size = 0;
    int result = 0;



    /* 测试一： 向空缓冲区添加元素 */
    result = buffer_add(buffer, BUFFER_CAPACITY, &size, 10);
    check_int("add to empty buffer", result, 1);
    check_int("size afte add", size, 1);
    check_int("first value after add", buffer[0], 10);
    
    /* 测试二： 添加元素直到缓冲区满 */
    buffer_add(buffer, BUFFER_CAPACITY, &size, 20);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 30);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 40);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 50);

    check_int("size when buffer is full", size, BUFFER_CAPACITY);
    check_int("last element in full buffer", buffer[4], 50);

    /* 测试三： 缓冲区满后继续添加*/
    result = buffer_remove(buffer, &size, 2);
    check_int("remove middle element", result, 1);
    check_int("size after middle removal", size, 4);
    check_int("shifted middle value", buffer[2], 40);
    check_int("shifted last value", buffer[3], 50);


    /* 测试四： 删除中间元素*/
    result = buffer_remove(buffer, &size, 2);
    check_int("remove middle element", result, 1);
    check_int("size after middle removal", size, 4);
    check_int("shifted middle value", buffer[2], 40);
    check_int("shifted last value", buffer[3], 50);


    /* 测试五：负数下标*/
    result = buffer_remove(buffer, &size, -1);
    check_int("remove negative index", result, 0);
    check_int("size unchanged after negative index", size, 4);

    /* 测试六：越界下标 */
    result = buffer_remove(buffer, &size, 10);
    check_int("remove negative index", result, 0);
    check_int("size unchanged after negative index", size, 4);

    /* 测试七： 删除第一个元素 */
    result = buffer_remove(buffer, &size, 0);
    check_int("remove first element", result, 1);
    check_int("size after first removal", size, 3);
    check_int("new first element", buffer[0], 20);

    /* 测试七： 删除最一个元素 */
    result = buffer_remove(buffer, &size, size - 1);
    check_int("remove last element", result, 1);
    check_int("size after first removal", size, 2);
    check_int("remaining last element", buffer[0], 40);

    /* 测试九：删除空缓冲区中的元素 */
    int empty_buffer[BUFFER_CAPACITY];
    int empty_size = 0;
    result = buffer_remove(empty_buffer, &empty_size, 0);
    check_int("remove from empty buffer", result, 0);
    check_int("empty size unchanged", empty_size, 0);
    printf("\n Test passed: %d/%d\n", test_passed, tests_run);

    return 0;
}