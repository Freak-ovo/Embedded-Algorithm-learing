#include <stdio.h>

#define RING_BUFFER_CAPACITY    5

typedef struct {
    int data[RING_BUFFER_CAPACITY];
    int read_index;
    int write_index;
    int size;
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

/* 逻辑视图打印：从读指针开始，打印size个有效元素 */
void ring_buffer_logical_print(const RingBuffer *buffer) {
    printf("logical:");
    for (int i = 0; i < buffer->size; i++) {
        int index = (buffer->read_index + i) % RING_BUFFER_CAPACITY;
        printf(" %d", buffer->data[index]);
    }
    printf("\n");
}

/* 物理数组原始打印 */
void ring_buffer_physical_print(const RingBuffer *rb) {
    printf("physical:");
    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        printf(" %d", rb->data[i]);
    }
    printf("\n");
}

/* 打印完整状态 */
void ring_buffer_print_state(const RingBuffer *rb) {
    ring_buffer_physical_print(rb);
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

static int tests_run = 0;
static int tests_passed = 0;
void check_int(const char *test_name, int actual, int expected) {
    tests_run++;
    if (actual == expected) {
        tests_passed++;
        printf("[PASS] %s\n", test_name);
    }
    else {
        printf("[FAIL] %s: expected = %d, actual = %d\n", test_name, expected, actual);
    }
}

int main() {
    RingBuffer rb;
    int value = -999;
    int result;
    ring_buffer_init(&rb);

    check_int("initial size", rb.size, 0);
    check_int("initial read index", rb.read_index, 0);
    check_int("initial write index", rb.write_index, 0);

    check_int("initial buffer is empty", ring_buffer_is_empty(&rb), 1);
    check_int("initial buffer is full", ring_buffer_is_full(&rb), 0);

    result = ring_buffer_peek(&rb, &value);
    check_int("peek empty buffer", result, 0);
    check_int("value unchanged after failed peek", value, -999);
    check_int("size unchanged after failed peek", rb.size, 0);

    result = ring_buffer_read(&rb, &value);
    check_int("read empty buffer", result, 0);
    check_int("value unchanged after failed read", value, -999);
    check_int("size unchanged after failed read", rb.size, 0);
    check_int("read index unchanged after failed read", rb.read_index, 0);

    printf("\nTests Passed: %d / %d\n", tests_passed, tests_run);
    print_separator();

    /* 写入测试 */
    result = ring_buffer_write(&rb, 10);
    check_int("first write succeeds", result, 1);
    check_int("first stored value", rb.data[0], 10);
    check_int("size after first write", rb.size, 1);
    check_int("read index after first write", rb.read_index, 0);
    check_int("write index after first write", rb.write_index, 1);

    /* 测试非空缓冲区peek */
    result = ring_buffer_peek(&rb, &value);
    check_int("peek succeeds ", result, 1);
    check_int("peek value", value, 10);
    check_int("read index unchanged after peek", rb.read_index, 0);

    /* 写满缓冲区 */
    ring_buffer_write(&rb, 20);
    ring_buffer_write(&rb, 30);
    ring_buffer_write(&rb, 40);
    ring_buffer_write(&rb, 50);

    check_int("size when full", rb.size, RING_BUFFER_CAPACITY);
    check_int("buffer reports full", ring_buffer_is_full(&rb), 1);
    check_int("full buffer is not empty", ring_buffer_is_empty(&rb), 0);
    check_int("write index wrapped to zero", rb.write_index, 0);
    check_int("indexes equal when full", rb.read_index == rb.write_index, 1);

    /* 测试缓冲区写入失败 */
    int old_size = rb.size;
    int old_write_index = rb.write_index;
    int old_data = rb.data[rb.write_index];
    result = ring_buffer_write(&rb, 60);
    check_int("write full buffer fails", result, 0);
    check_int("size unchanged after failed write", rb.size, old_size);
    check_int("write index unchanged after failed write", rb.write_index, old_write_index);
    check_int("data unchanged after failed write", rb.data[rb.write_index], old_data);

    /* 测试读取 */
    result = ring_buffer_read(&rb, &value);
    check_int("first read succeeds", result, 1);
    check_int("first read value", value, 10);
    check_int("size after first read", rb.size, 4);
    check_int("read index after first read", rb.read_index, 1);

    result = ring_buffer_read(&rb, &value);
    check_int("second read succeeds", result, 1);
    check_int("second read value", value, 20);
    check_int("size after second read", rb.size, 3);
    check_int("read index after second read", rb.read_index, 2);

    result = ring_buffer_write(&rb, 60);
    check_int("write 60 succeeds", result, 1);
    check_int("60 stored at index zero", rb.data[0], 60);
    check_int("write index after 60", rb.write_index, 1);

    result = ring_buffer_write(&rb, 70);
    check_int("write 70 succeeds", result, 1);
    check_int("70 stored at index one", rb.data[1], 70);
    check_int("write index after 70", rb.write_index, 2);
    check_int("size after wrapped writes", rb.size, 5);

    int expected_values[] = {30, 40, 50, 60, 70};
    int expected_count = sizeof(expected_values) / sizeof(expected_values[0]);

    for (int i = 0; i < expected_count; i++) {
        result = ring_buffer_read(&rb, &value);
        check_int("wrapped read succeeds", result, 1);
        check_int("wrapped read value", value, expected_values[i]);
    }

    check_int("size after all read", rb.size, 0);
    check_int("buffer empty after all reads", ring_buffer_is_empty(&rb), 1);
    check_int("buffer not full after all read", ring_buffer_is_full(&rb), 0);
    check_int("read and write indexes equal", rb.read_index == rb.write_index, 1);

    int old_read_index = rb.read_index;
    old_size = rb.size;

    result = ring_buffer_read(&rb, &value);
    check_int("final empty read fails", result, 0);
    check_int("final output unchanged", value, -999);
    check_int("final size unchanged", rb.size, old_size);
    check_int("final read index unchanged", rb.read_index, old_read_index);

    print_separator();
    printf("All tests: %d, Passed: %d\n", tests_run, tests_passed);
    return 0;
}
