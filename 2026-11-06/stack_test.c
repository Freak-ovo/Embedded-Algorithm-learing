#include <stdio.h>
#define STACK_CAPACITY  5


static int tests_run = 0;       /* 已经执行多少项检查 */
static int tests_passed = 0;    /* 其中有多少项通过 */


int stack_push(int stack[], int capacity, int *size, int value) {

    if (*size >= capacity) {
        return 0;
    }
    stack[*size] = value;
    (*size)++;

    return 1;
}


int stack_pop(int stack[], int *size, int *value) {
    if (*size <= 0) {
        return 0;
    }

    *value = stack[*size - 1];
    (*size)--;

    return 1;
}

int stack_peek(const int stack[], int size, int *value) {
    if (size <= 0) {
        return 0;
    }
    *value = stack[size - 1];

    return 1;

}



void stack_print(const int stack[], int size) {
    printf("stack:");

    for (int i = 0; i < size; i++){
        printf(" %d", stack[i]);
    }

    printf("\n");
}

void check_int(const char *test_name, int actual, int expected) {
    tests_run++;
    if (actual == expected) {
        tests_passed++;
        printf("[PASS] %s\n", test_name);
    }
    else {
        printf("[FAIL] %s: expected = %d, actual = %d \n", test_name, expected, actual);
    }
}


int main() {

    int stack[STACK_CAPACITY];
    int size = 0;
    int value = -999;
    int result;
    /* 第一组，测试初始状态 */
    check_int("initial size", size, 0);

    /* 第二组，测试空栈peek */
    result = stack_peek(stack, size, &value);
    check_int("peek empty stack", result, 0);
    check_int("size unchanged after failed peek", size, 0);
    check_int("output unchanged after failed peek", value, -999);
    
    /* 第三组，测试空栈pop */
    result = stack_pop(stack, &size, &value);
    check_int("pop empty", result, 0);
    check_int("size unchanged after failed pop", size, 0);
    check_int("output unchanged after failed pop",value, -999);




    /* 第四组，测试第一次push */
    result = stack_push(stack, STACK_CAPACITY, &size, 10);
    check_int("size after first push", size, 1);
    check_int("first stored value", stack[0], 10);

    /* 第五组，压满到栈 */
    stack_push(stack, STACK_CAPACITY, &size, 20);
    stack_push(stack, STACK_CAPACITY, &size, 30);
    stack_push(stack, STACK_CAPACITY, &size, 40);
    stack_push(stack, STACK_CAPACITY, &size, 50);
    stack_print(stack, size);
    check_int("size when stack is full", size, STACK_CAPACITY);
    check_int("top value when stack is full", stack[size - 1], 50);

    /* 第六组，满栈继续压栈 */
    result = stack_push(stack, STACK_CAPACITY, &size, 60);
    check_int("push to full stack", result, 0);
    check_int("size unchanged after failed push", size, STACK_CAPACITY);
    check_int("top unchanged after failed push", stack[size - 1], 50);

    /* 第七组，测试peek */
    value = -999;
    result = stack_peek(stack, size, &value);
    check_int("peek full stack", result, 1);
    check_int("peek returns top value", value, 50);
    check_int("size unchanged after peek", size, 5);

    /* 第八组， 验证FIFO */
    /* 第一次出栈 */
    result = stack_pop(stack, &size, &value);
    check_int("first pop succeeds", result, 1);
    check_int("first popped value", value, 50);
    check_int("size after first pop", size, 4);

    /* 第二次出栈 */
    result = stack_pop(stack, &size, &value);
    check_int("first pop succeeds", result, 1);
    check_int("first popped value", value, 40);
    check_int("size after first pop", size, 3);

    /* 第三次出栈 */
    result = stack_pop(stack, &size, &value);
    check_int("first pop succeeds", result, 1);
    check_int("first popped value", value, 30);

    /* 第四次出栈 */
    result = stack_pop(stack, &size, &value);
    check_int("first pop succeeds", result, 1);
    check_int("first popped value", value, 20);

    /* 第五次出栈 */
    result = stack_pop(stack, &size, &value);
    check_int("first pop succeeds", result, 1);
    check_int("first popped value", value, 10);

    check_int("first popped value", size, 0);


    value = -999;

    result = stack_pop(stack, &size, &value);

    check_int("pop after stack becomes empty", result, 0);
    check_int("size remains zero", size, 0);
    check_int("ouput unchanged after final failed pop", value, -999);



    printf("\n Tests passed: %d / %d\n", tests_passed, tests_run);
    




}