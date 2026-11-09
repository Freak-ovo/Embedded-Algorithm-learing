#include <stdio.h>

// 定义队列的最大容量
#define QUEUE_CAPACITY  5

// 全局变量：用于统计测试用例的运行情况
static int tests_run = 0;
static int tests_passed = 0;

/**
 * 入队函数：向队列尾部添加元素
 * @param size 传指针，因为入队成功后需要修改外部变量的 size (+1)
 */
int queue_enqueue(int queue[], int capacity, int *size, int value) {
    // 满队列保护：如果当前元素个数已达到容量上限，无法入队
    if (*size >= capacity) {
        return 0;
    }
    // 在数组末尾存入新值
    queue[*size] = value;
    (*size)++; // 元素个数加 1
    return 1;
}

/**
 * 出队函数：从队列头部取出元素
 * @param value 传指针，用于将取出的队首元素“带回”给调用者
 */
int queue_dequeue(int queue[], int *size, int *value) {
    // 空队列保护
    if (*size <= 0) {
        return 0;
    }
    *value = queue[0]; // 1. 保存队头元素
    
    // 2. 数据搬移：将后续所有元素向前移动一位
    for (int i = 0; i < *size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    
    (*size)--; // 3. 元素个数减 1
    return 1;
}

/**
 * 查看队首函数：只看不取，不修改队列状态
 * @param size 这里传的是值（int size），因为查看操作不需要修改队列长度
 */
int queue_peek(const int queue[], int size, int *value) {
    if (size <= 0) {
        return 0;
    }
    *value = queue[0]; // 仅将队首值通过指针带出
    return 1;
}

/**
 * 测试辅助函数：对比实际值与期望值，并打印结果
 */
void check_int(const char *test_name, int actual, int expected) {
    tests_run++; // 运行计数加 1

    if (actual == expected) {
        tests_passed++; // 通过计数加 1
        printf("[PASS] %s\n", test_name);
    }
    else {
        // 打印失败信息，方便调试
        printf("[FAIL] %s : expected = %d, actual = %d \n", test_name, expected, actual);
    }
}

int main() {
    int queue[QUEUE_CAPACITY]; // 底层数组
    int size = 0;              // 当前队列逻辑大小
    int value = -999;          // 用于接收出队/peek 的值，初始化为一个特殊值方便观察
    int result;                // 用于接收函数的返回值（成功1/失败0）

    /* 测试一：初始状态 */
    check_int("initial size", size, 0);

    /* 测试二：空队列 peek */
    result = queue_peek(queue, size, &value);
    check_int("peek empty queue", result, 0);             // 应该返回 0 (失败)
    check_int("size unchanged after failed peek", size, 0); // size 不应被修改
    check_int("output unchanged after failed peek", value, -999); // value 不应被修改

    /* 测试三：空队列 dequeue */
    value = -999;
    result = queue_dequeue(queue, &size, &value);
    check_int("dequeue empty queue", result, 0);
    check_int("size unchanged after failed dequeue", size, 0);
    check_int("output unchanged after failed dequeue", value, -999);

    /* 测试四：第一次 enqueue */
    result = queue_enqueue(queue, QUEUE_CAPACITY, &size, 10);
    check_int("enqueue first value", result, 1);          // 应该返回 1 (成功)
    check_int("size after first enqueue", size, 1);       // size 应变为 1
    check_int("first stored value", queue[0], 10);        // 数组第一个位置应为 10

    /* 测试五：enqueue 到满队列 */
    // 连续入队 4 个元素，加上之前的 10，正好填满容量 5
    queue_enqueue(queue, QUEUE_CAPACITY, &size, 20);
    queue_enqueue(queue, QUEUE_CAPACITY, &size, 30);
    queue_enqueue(queue, QUEUE_CAPACITY, &size, 40);
    queue_enqueue(queue, QUEUE_CAPACITY, &size, 50);

    check_int("size when queue is full", size, QUEUE_CAPACITY); // size 应为 5
    check_int("front value when is full", queue[0], 10);        // 队首仍是 10
    check_int("rear value when queue is full", queue[size - 1], 50); // 队尾是最后存入的 50

    /* 测试六：满队列继续 enqueue (溢出测试) */
    result = queue_enqueue(queue, QUEUE_CAPACITY, &size, 60);
    check_int("enqueue to full queue", result, 0);                // 应该返回 0 (失败)
    check_int("front unchanged after failed enqueue", queue[size - 1], 50); // 队尾数据不应被覆盖

    /* 测试七：peek 队首 */
    value = -999;
    result = queue_peek(queue, size, &value);
    check_int("peek full queue", result, 1);      // 应该成功
    check_int("peek returns front value", value, 10); // 取出的值应为队首 10
    check_int("size unchanged after peek", size, 5);  // peek 不改变 size

    /* 测试八：按照 FIFO (先进先出) 顺序 dequeue */
    // 第一次出队
    result = queue_dequeue(queue, &size, &value);
    check_int("first dequeue succeeds", result, 1);
    check_int("first dequeue value", value, 10);      // 最先入队的 10 最先出来
    check_int("new front after first dequeue", queue[0], 20); // 队首变为 20
    
    // 第二次出队
    result = queue_dequeue(queue, &size, &value);
    check_int("second dequeue succeeds", result, 1);
    check_int("second dequeue value", value, 20);
    check_int("size after second dequeue", size, 3);
    check_int("new front after second dequeue", queue[0], 30);
    
    // 第三次出队
    result = queue_dequeue(queue, &size, &value);
    check_int("third dequeue succeeds", result, 1);
    check_int("third dequeue value", value, 30);
    check_int("size after third dequeue", size, 2);
    check_int("new front after third dequeue", queue[0], 40);
    
    // 第四次出队
    result = queue_dequeue(queue, &size, &value);
    check_int("fourth dequeue succeeds", result, 1);
    check_int("fourth dequeue value", value, 40);
    check_int("size after fourth dequeue", size, 1);
    check_int("new front after fourth dequeue", queue[0], 50);
    
    // 第五次出队（队列变空）
    result = queue_dequeue(queue, &size, &value);
    check_int("fifth dequeue succeeds", result, 1);
    check_int("fifth dequeue value", value, 50);
    check_int("size after fifth dequeue", size, 0);
    // 注意：此时队列为空，queue[0] 的残留值通常不再关心，但线性移动后它确实还是 50
    check_int("new front after fifth dequeue", queue[0], 50); 
    
    check_int("dequeue after queue becomes empty", size, 0);

    /* 测试九： 空队列再次 dequeue (下溢测试) */
    value = -999;
    result = queue_dequeue(queue, &size, &value);
    check_int("dequeue after queue becomes empty", result, 0); // 应该返回 0 (失败)
    check_int("size remains zero", size, 0);                   // size 保持为 0

    // 打印最终测试结果汇总
    printf("\n Tests Passed: %d / %d\n", tests_passed, tests_run);
    return 0;
}