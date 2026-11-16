#include <stdio.h>
#include <assert.h>

#define BUFFER_CAPACITY 5

/* ---------- 核心操作 ---------- */

int ring_buffer_write(int buffer[], int capacity,
                      int *write_index, int *size, int value) {
    if (*size >= capacity) return 0;

    buffer[*write_index] = value;
    *write_index = (*write_index + 1) % capacity;
    (*size)++;
    return 1;
}

int ring_buffer_read(const int buffer[], int capacity,
                     int *read_index, int *size, int *value) {
    if (*size <= 0) return 0;

    *value = buffer[*read_index];
    *read_index = (*read_index + 1) % capacity;
    (*size)--;
    return 1;
}

/* ---------- 打印工具 ---------- */

void print_physical_buffer(const int buffer[], int capacity) {
    printf("physical  :");
    for (int i = 0; i < capacity; i++) printf(" %3d", buffer[i]);
    printf("\n");
}

/* 按逻辑顺序打印（从 read_index 起，共 size 个） */
void print_logical(const int buffer[], int capacity,
                   int read_index, int size) {
    printf("logical   :");
    for (int i = 0; i < size; i++) {
        int idx = (read_index + i) % capacity;
        printf(" %3d", buffer[idx]);
    }
    if (size == 0) printf(" (empty)");
    printf("\n");
}

void print_state(const int buffer[], int capacity,
                 int read_index, int write_index, int size) {
    print_physical_buffer(buffer, capacity);
    print_logical(buffer, capacity, read_index, size);
    printf("state     : read=%d write=%d size=%d  (%s)\n",
           read_index, write_index, size,
           size == 0 ? "EMPTY" :
           size == capacity ? "FULL" : "partial");
    printf("---------------------------------------------\n");
}

/* ---------- 主程序：完整走一遍题目里的场景 ---------- */

int main(void) {
    int buffer[BUFFER_CAPACITY] = {0};
    int read_index = 0;
    int write_index = 0;
    int size = 0;

    printf("=== 1. 初始状态 ===\n");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 2. 写入 10, 20, 30 ===\n");
    for (int i = 10; i <= 30; i += 10) {
        int ok = ring_buffer_write(buffer, BUFFER_CAPACITY,
                                   &write_index, &size, i);
        printf("  write(%d) -> %s\n", i, ok ? "ok" : "FULL");
    }
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 3. 读一次，应得 10 ===\n");
    int value;
    int ok = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    printf("  read -> %s, value=%d\n", ok ? "ok" : "EMPTY", value);
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 4. 再写 40,50 使 size=4，再读 20 ===\n");
    for (int i = 40; i <= 50; i += 10) {
        int ok = ring_buffer_write(buffer, BUFFER_CAPACITY,
                                    &write_index, &size, i);
        printf("  write(%d) -> %s\n", i, ok ? "ok" : "FULL");
    }
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);
    
    ring_buffer_read (buffer, BUFFER_CAPACITY, &read_index,  &size, &value);
    printf("  read -> %d\n", value);
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);
    /* 此时 read=2, write=0, size=3，数据逻辑顺序应为 30 40 50 */

    printf("=== 5. 写 60,70 触发回绕并填满 ===\n");
    for (int i = 60; i <= 70; i += 10) {
        int ok = ring_buffer_write(buffer, BUFFER_CAPACITY,
                                    &write_index, &size, i);
        printf("  write(%d) -> %s\n", i, ok ? "ok" : "FULL");
    }    /* 应写满：size == capacity */
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 6. 满时再写，应失败且状态不变 ===\n");
    int saved_w = write_index, saved_s = size;
    int saved_b0 = buffer[0], saved_b1 = buffer[1];
    ok = ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 99);
    assert(ok == 0);
    assert(write_index == saved_w && size == saved_s);
    assert(buffer[0] == saved_b0 && buffer[1] == saved_b1);
    printf("  write(99) -> %s，状态未变 ✓\n", ok ? "ok" : "FULL");
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 7. 依次读出全部，应为 30 40 50 60 70 ===\n");
    while (size > 0) {
        ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
        printf("  read -> %d\n", value);
    }
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    printf("=== 8. 空时再读，应失败且状态不变 ===\n");
    int saved_r = read_index, saved_s2 = size;
    int dummy = -1;
    ok = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &dummy);
    assert(ok == 0);
    assert(read_index == saved_r && size == saved_s2);
    assert(dummy == -1);           /* 输出参数不能被改动 */
    printf("  read() -> %s，状态和输出均未变 ✓\n", ok ? "ok" : "EMPTY");

    return 0;
}