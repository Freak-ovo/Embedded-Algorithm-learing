#include <stdio.h>

// 环形缓冲区容量，最多存放 RING_BUFFER_CAPACITY 个有效数据
#define RING_BUFFER_CAPACITY    5

// 环形缓冲区结构体
typedef struct {
    int data[RING_BUFFER_CAPACITY];  // 存储数据的数组
    int read_index;                  // 下一次读取的位置
    int write_index;                 // 下一次写入的位置
    int size;                        // 当前缓冲区里面有效元素数量
} RingBuffer;

/*
 * 结构体访问两种方式：
 * 1. 结构体变量：使用 . 访问成员
 *    struct RingBuffer buffer;
 *    buffer.size = 0;
 *    buffer.read_index = 0;
 *
 * 2. 结构体指针：使用 -> 访问成员
 *    buffer_ptr->size 等价于 (*buffer_ptr).size
 *    struct RingBuffer *buffer_ptr = &buffer;
 *    buffer_ptr->size = 0;
 *    buffer_ptr->read_index = 0;
 */

/*
 * 函数参数说明：
 * 如果传 RingBuffer buffer：传入的是结构体副本，函数内修改不会影响main里面原变量
 * 如果传 RingBuffer *buffer：传入结构体地址，函数内通过指针修改main里原始结构体
 */
void ring_buffer_init(RingBuffer *buffer) {
    buffer->read_index = 0;
    buffer->write_index = 0;
    buffer->size = 0;

    // 把数组全部清零
    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        buffer->data[i] = 0;
    }
}

/* 判断缓冲区是否为空 */
int ring_buffer_is_empty(const RingBuffer *buffer) {
    return buffer->size == 0;
}

/* 判断缓冲区是否已满 */
int ring_buffer_is_full(const RingBuffer *buffer) {
    return buffer->size >= RING_BUFFER_CAPACITY;
}

/* 获取当前有效元素个数 */
int ring_buffer_size(const RingBuffer *buffer) {
    return buffer->size;
}

/*
 * 写入一个数据
 * 返回值：1 写入成功；0 缓冲区满，写入失败
 */
int ring_buffer_write(RingBuffer *buffer, int value) {
    if (ring_buffer_is_full(buffer)) {
        return 0;
    }

    buffer->data[buffer->write_index] = value;
    // 写索引向后走，取模实现环形绕回
    buffer->write_index = (buffer->write_index + 1) % RING_BUFFER_CAPACITY;
    buffer->size++;
    return 1;
}

/*
 * 读取队首数据
 * value：传出参数，存放读到的数据
 * 返回值：1 读取成功；0 缓冲区空，读取失败
 */
int ring_buffer_read(RingBuffer *buffer, int *value) {
    if (ring_buffer_is_empty(buffer)) {
        return 0;
    }
    *value = buffer->data[buffer->read_index];

    // 读索引向后走，取模实现环形绕回
    buffer->read_index = (buffer->read_index + 1) % RING_BUFFER_CAPACITY;
    buffer->size--;
    return 1;
}

/*
 * 查看队首数据（peek，偷看）
 * 只拿数据，**不移动读指针、不删除元素**
 * 返回值：1 成功；0 空
 */
int ring_buffer_peek(const RingBuffer *buffer, int *value) {
    if (ring_buffer_is_empty(buffer)) {
        return 0;
    }
    *value = buffer->data[buffer->read_index];
    return 1;
}

/* 打印缓冲区核心索引状态：读索引、写索引、有效数量 */
void ring_buffer_state(const RingBuffer *buffer) {
    printf("read_index = %d, write_index = %d, size = %d \n", buffer->read_index, buffer->write_index, buffer->size);
}

/* 打印缓冲区【逻辑上有效数据】，从读指针开始输出 */
void ring_buffer_printf(const RingBuffer *buffer) {
    printf("buffer:");
    for (int i = 0; i < buffer->size; i++) {
        // 计算逻辑下标，环形取模
        int index = (buffer->read_index + i) % RING_BUFFER_CAPACITY;
        printf(" %d", buffer->data[index]);
    }
    printf("\n");
}

int main() {
    RingBuffer buffer;
    // 初始化缓冲区
    ring_buffer_init(&buffer);

    printf("inital:\n");
    ring_buffer_init(&buffer);
    ring_buffer_printf(&buffer);
    ring_buffer_state(&buffer);

    // 依次写入 10、20、30
    ring_buffer_write(&buffer, 10);
    ring_buffer_write(&buffer, 20);
    ring_buffer_write(&buffer, 30);

    printf("\nafter writing 10, 20, 30:\n");
    ring_buffer_printf(&buffer);
    ring_buffer_state(&buffer);

    int value;
    // peek 偷看队首
    int success = ring_buffer_peek(&buffer, &value);
    if (success) {
        printf("\npeek: %d\n", value);
    } else {
        printf("\npeek: buffer empty\n");
    }
    printf("after peek:\n");
    ring_buffer_printf(&buffer);
    ring_buffer_state(&buffer);

    // read 取出队首
    success = ring_buffer_read(&buffer, &value);
    if (success) {
        printf("\nread: %d\n", value);
    } else {
        printf("\nread: buffer empty\n");
    }
    printf("after read:\n");
    ring_buffer_printf(&buffer);
    ring_buffer_state(&buffer);

    // 打印当前有效元素数量
    printf("\ncurrent size: %d\n", ring_buffer_size(&buffer));

    return 0;
}
