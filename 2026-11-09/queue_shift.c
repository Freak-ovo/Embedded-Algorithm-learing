#include <stdio.h>

// 定义队列的最大容量为 5
#define QUEUE_CAPACITY  5

/**
 * 入队函数：向队列尾部添加元素
 * @param queue    底层数组
 * @param capacity 队列最大容量
 * @param size     指向当前元素个数的指针（需要修改实参，所以传指针）
 * @param value    要入队的值
 * @return         成功返回 1，队列满返回 0
 */
int queue_enqueue(int queue[], int capacity, int *size, int value) {

    /* 满队列保护：如果当前元素个数已达到容量上限，无法入队 */
    if (*size >= capacity) {
        return 0;
    }

    // 在数组末尾（即当前 size 的位置）存入新值
    queue[*size] = value;
    (*size)++; // 元素个数加 1
    return 1;
}

/**
 * 出队函数：从队列头部取出元素（线性移动实现）
 * @param queue  底层数组
 * @param size   指向当前元素个数的指针
 * @param value  用于保存出队值的指针（输出参数）
 * @return       成功返回 1，队列空返回 0
 */
int queue_dequeue(int queue[], int *size, int *value) {
    // 空队列保护：如果没有元素，无法出队
    if (*size <= 0) {
        return 0;
    }

    // 1. 保存队头元素（数组下标 0 的位置）到 value 指向的变量中
    *value = queue[0];

    // 2. 数据搬移：将所有后续元素向前移动一位，覆盖掉原来的队头
    for (int i = 0; i < *size - 1; i++) {
        queue[i] = queue[i + 1];
    }

    // 3. 元素个数减 1
    (*size)--;
    return 1;
}

/* 打印有效队列：只打印当前逻辑上存在的元素 */
void queue_print(const int queue[], int size) {
    printf("valid queue: ");

    for (int i = 0; i < size; i++) {
        printf(" %d", queue[i]);
    }
    printf("\n");
}

/* 打印物理数组：打印底层数组的所有内存位置（包括无效数据） */
void queue_print_physical(const int queue[], int capacity) {
    printf("physical array:");

    for (int i = 0; i < capacity; i++) {
        printf(" %d", queue[i]);
    }
    printf("\n");
}

/**
 * 调试版出队函数：详细打印每一步的数据搬移过程
 */
int queue_dequeue_debug(int queue[], int *size, int *value) {
    if (*size <= 0) {
        return 0;
    }

    // 保存队头值
    *value = queue[0];
    printf("save front value: %d\n", *value);

    // 循环搬移数据，并打印每一步的内存变化
    for (int i = 0; i < *size - 1; i++) {
        printf("----第 %d 轮 ---\n", i + 1);

        // 打印当前正在执行的移动操作
        printf("move queue[%d] = queue[%d]\n", i, i + 1);
        queue[i] = queue[i + 1];

        // 每移动一次，就打印一次整个物理数组的状态
        queue_print_physical(queue, QUEUE_CAPACITY);
    }

    (*size)--; // 更新逻辑大小

    return 1;
}

int main() {
    // 初始化底层数组，预设了一些数据
    int queue[QUEUE_CAPACITY] = {10, 20, 30, 40, 0};
    int size = 4; // 当前逻辑上有 4 个有效元素（10, 20, 30, 40）
    int value;    // 用于接收出队的值

    // 打印初始状态
    queue_print(queue, size);
    queue_print_physical(queue, QUEUE_CAPACITY);

    // 执行调试版出队，观察内部搬移过程
    int success = queue_dequeue_debug(queue, &size, &value);

    if (success) {
        printf("dequeue value: %d\n", value);
    }
    else {
        printf("dequeue failed: queue empty\n");
    }

    // 打印出队后的最终状态
    printf("after dequeue: \n");
    queue_print(queue, size);
    queue_print_physical(queue, QUEUE_CAPACITY);
    printf("size = %d\n", size);

    return 0;
}