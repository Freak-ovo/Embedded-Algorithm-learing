#include <stdio.h>

#define QUEUE_CAPACITY  5

// 入队：在尾部添加元素
int queue_enqueue(int queue[], int capacity, int *size, int value) {
    if (*size >= capacity) {
        return 0; // 队列满，入队失败
    }
    queue[*size] = value;
    (*size)++;
    return 1;
}

// 出队：从头部取出元素，并移动剩余数据
int queue_dequeue(int queue[], int *size, int *value) {
    if (*size <= 0) {
        return 0; // 队列空，出队失败
    }
    *value = queue[0]; // 取出队头
    for (int i = 0; i < *size - 1; i++) {
        queue[i] = queue[i + 1]; // 数据前移
    }
    (*size)--;
    return 1;
}

void queue_print(const int queue[], int size) {
    printf("queue: ");
    for (int i = 0; i < size; i++) {
        printf(" %d", queue[i]);
    }
    printf("\n");
}

int main() {
    int queue[QUEUE_CAPACITY];
    int size = 0;
    int value;

    // 1. 测试入队
    int values[] = {10, 20, 30};
    int values_count = sizeof(values) / sizeof(values[0]);

    printf("--- 开始入队 ---\n");
    for (int i = 0; i < values_count; i++) {
        int success = queue_enqueue(queue, QUEUE_CAPACITY, &size, values[i]);
        if (success) {
            printf("enqueue %d: success\n", values[i]);
        } else {
            printf("enqueue queue full\n");
        }
    }
    queue_print(queue, size);

    // 2. 测试出队
    printf("\n--- 开始出队 ---\n");
    if (queue_dequeue(queue, &size, &value)) {
        printf("dequeue value: %d\n", value);
    }
    queue_print(queue, size); // 此时应该只剩 20, 30

    return 0;
}