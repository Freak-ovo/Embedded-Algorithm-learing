#include <stdio.h>

#define RING_BUFFER_CAPACITY    5

typedef struct {
    int data[RING_BUFFER_CAPACITY];  /* 环形数组 */
    int read_index;                  /* 下次读取位置 */
    int write_index;                 /* 下一次写入位置 */
    int size;                        /* 当前有效元素数量 */
} RingBuffer;

/*
 * buffer 传值：传入结构体副本，函数内修改不影响外部原变量
 * buffer 传指针(RingBuffer*)：传入结构体地址，直接操作原始结构体
 */
void ring_buffer_init(RingBuffer *buffer) {
    buffer->read_index = 0;
    buffer->write_index = 0;
    buffer->size = 0;

    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        buffer->data[i] = 0;
    }
}

/* 判断空队列 */
int ring_buffer_is_empty(const RingBuffer *buffer) {
    return buffer->size == 0;
}
/* 判断满 */
int ring_buffer_is_full(const RingBuffer *buffer) {
    return buffer->size >= RING_BUFFER_CAPACITY;
}
/* 获取有效元素数量 */
int ring_buffer_size(const RingBuffer *buffer) {
    return buffer->size;
}

/* 写入函数，成功返回1，满返回0 */
int ring_buffer_write(RingBuffer *buffer, int value) {
    if (ring_buffer_is_full(buffer)) {
        return 0;
    }

    buffer->data[buffer->write_index] = value;
    buffer->write_index = (buffer->write_index + 1) % RING_BUFFER_CAPACITY;
    buffer->size++;
    return 1;
}

/* 读取函数，成功返回1，空返回0，读到的值存入*value */
int ring_buffer_read(RingBuffer *buffer, int *value) {
    if (ring_buffer_is_empty(buffer)) {
        return 0;
    }
    *value = buffer->data[buffer->read_index];

    buffer->read_index = (buffer->read_index + 1) % RING_BUFFER_CAPACITY;
    buffer->size--;
    return 1;
}

/* 查看队首，不移动读指针 */
int ring_buffer_peek(const RingBuffer *buffer, int *value) {
    if (ring_buffer_is_empty(buffer)) {
        return 0;
    }
    *value = buffer->data[buffer->read_index];
    return 1;
}

void ring_buffer_state(const RingBuffer *buffer) {
    printf("read_index = %d, write_index = %d, size = %d \n", buffer->read_index, buffer->write_index, buffer->size);
}

/* 物理数组原始打印 */
void ring_buffer_print_physical(const RingBuffer *rb) {
    printf("physical:");
    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        printf(" %d", rb->data[i]);
    }
    printf("\n");
}

/* 逻辑视图打印：从读指针开始，打印size个有效元素 */
void ring_buffer_logical_print(const RingBuffer *rb) {
    printf("logical:");
    for (int i = 0; i < rb->size; i++) {
        int index = (rb->read_index + i) % RING_BUFFER_CAPACITY;
        printf(" %d", rb->data[index]);
    }
    printf("\n");
}

/* 打印完整状态 */
void ring_buffer_print_state(const RingBuffer *rb) {
    ring_buffer_print_physical(rb);
    ring_buffer_logical_print(rb);

    printf("read_index = %d\n", rb->read_index);
    printf("write_index = %d\n", rb->write_index);
    printf("size = %d\n", rb->size);

    printf("empty = %s\n", ring_buffer_is_empty(rb) ? "yes" : "no");
    printf("full = %s\n", ring_buffer_is_full(rb) ? "yes" : "no");
}

void print_separator(void) {
    printf("--------------------------------\n");
}

int main() {
    RingBuffer rb;
    int value;

    ring_buffer_init(&rb);
    printf("initial state\n");
    ring_buffer_print_state(&rb);
    print_separator();

    int values[] = {10, 20, 30, 40, 50};
    int values_count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < values_count; i++) {
        int success = ring_buffer_write(&rb, values[i]);
        if (success) {
            printf("write %d : success\n", values[i]);
        } else {
            printf("write %d : buffer full\n", values[i]);
        }
        ring_buffer_print_state(&rb);
        print_separator();
    }

    for (int i = 0; i < 2; i++) {
        int success = ring_buffer_read(&rb, &value);
        if (success) {
            printf("read: %d\n", value);
        } else {
            printf("read : buffer empty\n");
        }
        ring_buffer_print_state(&rb);
        print_separator();
    }

    int new_values[] = {60, 70};
    int new_value_count = sizeof(new_values) / sizeof(new_values[0]);

    for (int i = 0; i < new_value_count; i++) {
        int success = ring_buffer_write(&rb, new_values[i]);
        if (success) {
            printf("write %d : success\n", new_values[i]);
        } else {
            printf("write %d : buffer full\n", new_values[i]);
        }
        ring_buffer_print_state(&rb);
        print_separator();
    }

    printf("final state\n");
    ring_buffer_print_state(&rb);

    return 0;
}
