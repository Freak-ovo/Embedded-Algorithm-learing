#include <stdio.h>
#include <stdbool.h>

#define BUFFER_CAPACITY 5

/* 计算环形缓冲区的下一个索引。
 * 调用者需保证 capacity > 0，避免取模除零。 */
static inline int next_index(int index, int capacity) {
    return (index + 1) % capacity;
}

/**
 * @brief  向环形缓冲区写入一个元素
 * @param  buffer       缓冲区数组
 * @param  capacity     缓冲区容量（元素个数）
 * @param  write_index  写指针，写入后前移一位
 * @param  size         当前有效元素个数，写入成功后 +1
 * @param  value        待写入的值
 * @return true 写入成功；false 缓冲区已满或容量非法
 */
static bool ring_buffer_write(int *buffer, int capacity,
                              int *write_index, int *size, int value) {
    if (capacity <= 0 || *size >= capacity) {
        return false;                       /* 已满：拒绝写入，保护旧数据 */
    }

    buffer[*write_index] = value;
    *write_index = next_index(*write_index, capacity);
    (*size)++;

    return true;
}

/**
 * @brief  从环形缓冲区读取一个元素
 * @param  buffer      缓冲区数组（只读）
 * @param  capacity    缓冲区容量（元素个数）
 * @param  read_index  读指针，读取后前移一位
 * @param  size        当前有效元素个数，读取成功后 -1
 * @param  value       输出参数，保存读到的值
 * @return true 读取成功；false 缓冲区为空或容量非法
 */
static bool ring_buffer_read(const int *buffer, int capacity,
                             int *read_index, int *size, int *value) {
    if (capacity <= 0 || *size <= 0) {
        return false;                       /* 空：无可读数据 */
    }

    *value = buffer[*read_index];
    *read_index = next_index(*read_index, capacity);
    (*size)--;

    return true;
}

/* 按物理存储顺序打印缓冲区（含已失效的旧数据） */
static void print_physical(const int *buffer, int capacity) {
    printf("physical:");
    for (int i = 0; i < capacity; ++i) {
        printf(" %d", buffer[i]);
    }
    printf("\n");
}

/* 按逻辑顺序打印有效数据：从 read_index 开始，共 size 个 */
static void print_logical(const int *buffer, int capacity,
                          int read_index, int size) {
    printf("logical:");
    if (size == 0) {
        printf(" (empty)");
    } else {
        for (int i = 0; i < size; ++i) {
            printf(" %d", buffer[(read_index + i) % capacity]);
        }
    }
    printf("\n");
}

/* 打印完整状态：物理内容 + 逻辑内容 + 三个游标变量 */
static void print_state(const int *buffer, int capacity,
                        int read_index, int write_index, int size) {
    print_physical(buffer, capacity);
    print_logical(buffer, capacity, read_index, size);
    printf("read_index = %d, write_index = %d, size = %d\n",
           read_index, write_index, size);
}

int main(void) {
    int buffer[BUFFER_CAPACITY] = {0};
    int read_index  = 0;
    int write_index = 0;
    int size        = 0;
    int value       = 0;

    const int initial_values[] = {10, 20, 30, 40, 50};
    const int initial_count =
        (int)(sizeof(initial_values) / sizeof(initial_values[0]));

    printf("initial:\n");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    /* 依次写入 10~50（容量为 5，第 6 个起会失败） */
    for (int i = 0; i < initial_count; ++i) {
        if (!ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index,
                               &size, initial_values[i])) {
            printf("write %d failed: buffer full\n", initial_values[i]);
        }
    }

    printf("\nafter writing 10, 20, 30, 40, 50:\n");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    /* 读取两个元素（10、20） */
    for (int i = 0; i < 2; ++i) {
        if (ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index,
                             &size, &value)) {
            printf("\nread: %d\n", value);
        }
    }

    printf("\nafter reading two values:\n");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    /* 腾出空间后再写 60、70 */
    (void)ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 60);
    (void)ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 70);

    /* 注意：这里必须传 &read_index，而不是 &write_index */
    printf("\nread remaining values:");
    while (ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index,
                            &size, &value)) {
        printf(" %d", value);
    }
    printf("\n");

    printf("\nfinal state:\n");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    return 0;
}