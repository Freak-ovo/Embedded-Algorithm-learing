/**
 * @file ring_buffer.c
 * @brief 环形缓冲区（Ring Buffer）实现与测试
 * @date 2026-11-16
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │ 学习要点：环形缓冲区（Circular / Ring Buffer）               │
 * ├─────────────────────────────────────────────────────────────┤
 * │ 1. 用一段固定大小的数组 + 两个“游标”实现一个“首尾相接”的队列。│
 * │ 2. 两个游标：                                                 │
 * │      read_index  —— 下一个要“读”的位置（队头）                │
 * │      write_index —— 下一个要“写”的位置（队尾）                │
 * │ 3. 游标到达数组末尾后，通过 % capacity 回绕到 0，               │
 * │    就像把数组首尾连成一个环，故称“环形”。                      │
 * │ 4. 额外用 size 记录“当前有效元素个数”，                       │
 * │    用于区分“空”与“满”两种 read_index == write_index 的情况。 │
 * │ 5. 这是一个“满即失败”的缓冲区（不覆盖旧数据），               │
 * │    写入失败返回 false，由调用者决定如何处理。                 │
 * └─────────────────────────────────────────────────────────────┘
 *
 * 本版修复记录（相对初版）：
 *   1) 第四组“满缓冲区写入失败”的快照改为在写入前保存，
 *      否则断言比较的是“失败后”与“失败后”，无法发现副作用。
 *   2) 第六组补上 size 不变断言，消除 old_size 未使用警告。
 *   3) main 中调用 print_state，避免 static 函数未使用警告，
 *      同时便于观察游标移动。
 *   4) ring_buffer_is_full 增加 capacity > 0 防御。
 *   5) print_logical 增加 capacity <= 0 防御，避免除零。
 */

#include <stdio.h>
#include <stdbool.h>

/* ================= 宏定义与全局变量 ================= */

/* 缓冲区容量：5 个 int 槽位。
 * 注意：这里容量是“编译期常量”，便于声明栈上数组 buffer[BUFFER_CAPACITY]。
 * 真实项目中可改为动态分配（malloc）以支持运行时可变容量。 */
#define BUFFER_CAPACITY 5

/* 测试计数器：用于最后汇总 “通过数 / 总数”，是简易测试框架的雏形 */
static int tests_run = 0;    /* 已运行的断言数 */
static int tests_passed = 0; /* 已通过的断言数 */

/* ================= 辅助函数 ================= */

/**
 * @brief 计算环形缓冲区的下一个索引（处理回绕）
 * @param index    当前索引
 * @param capacity 缓冲区容量
 * @return 下一个索引位置
 *
 * 数学原理：
 *   (index + 1) % capacity
 *   - 当 index 还没到末尾时，结果就是 index + 1；
 *   - 当 index == capacity-1 时，(capacity-1+1)%capacity == 0，
 *     自动“绕回”到数组开头，这就是环形的核心。
 *
 * 小技巧：如果 capacity 是 2 的幂，可用 (index+1) & (capacity-1) 代替取模，
 *        速度更快（位运算），这也是很多内核里环形队列的常见写法。
 *
 * 注意：调用者需保证 capacity > 0，否则 % 会除零。
 *       本文件所有调用点都已在进入前检查 capacity。
 */
static inline int next_index(int index, int capacity) {
    return (index + 1) % capacity;
}

/* ================= 核心读写操作 ================= */

/**
 * @brief 向环形缓冲区写入一个元素
 * @param buffer       缓冲区数组
 * @param capacity     缓冲区容量
 * @param write_index  写指针（传入指针以便修改）
 * @param size         当前有效元素个数
 * @param value        待写入的值
 * @return true 写入成功；false 缓冲区已满或容量非法
 *
 * 为什么 write_index 和 size 用“指针”传入？
 *   C 语言函数参数是“值传递”。若想让函数内部修改调用者的变量，
 *   必须传变量地址（即指针）。read_index 也一样。
 *   buffer 本身传的是数组首地址，所以函数内可直接通过下标读写元素。
 *
 * 写入流程：
 *   1) 检查能不能写（容量合法、未满）；
 *   2) 把 value 放到 write_index 位置；
 *   3) write_index 前移一位并回绕；
 *   4) 有效元素数 +1。
 */
static bool ring_buffer_write(int *buffer, int capacity,
                              int *write_index, int *size, int value) {
    /* 容量非法或缓冲区已满，拒绝写入。
     * 注意这里 *size >= capacity 用 “>=” 更稳妥，
     * 即便由于 bug 导致 size 越界，也能防御性返回失败。 */
    if (capacity <= 0 || *size >= capacity) {
        return false;
    }

    buffer[*write_index] = value;                      /* 1) 落盘数据 */
    *write_index = next_index(*write_index, capacity); /* 2) 写游标前移+回绕 */
    (*size)++;                                         /* 3) 有效计数 +1 */

    return true;
}

/**
 * @brief 从环形缓冲区读取一个元素
 * @param buffer      缓冲区数组（只读）
 * @param capacity    缓冲区容量
 * @param read_index  读指针（传入指针以便修改）
 * @param size        当前有效元素个数
 * @param value       输出参数，保存读到的值
 * @return true 读取成功；false 缓冲区为空或容量非法
 *
 * 注意几点：
 *   - buffer 声明为 const int *：函数承诺不修改缓冲区内容，
 *     这是“接口契约”的表达，也便于调用者传入 const 数据。
 *   - value 用指针传入：这是典型的“输出参数”用法。
 *     C 函数只有一个返回值，想额外“返回”数据就要靠指针。
 *   - 读取失败时**不修改** *value、*read_index、*size，
 *     保证失败不产生副作用（下面的测试专门验证了这一点）。
 */
static bool ring_buffer_read(const int *buffer, int capacity,
                             int *read_index, int *size, int *value) {
    /* 容量非法或缓冲区为空，拒绝读取 */
    if (capacity <= 0 || *size <= 0) {
        return false;
    }

    *value = buffer[*read_index];                      /* 1) 取出队头数据 */
    *read_index = next_index(*read_index, capacity);   /* 2) 读游标前移+回绕 */
    (*size)--;                                         /* 3) 有效计数 -1 */

    return true;
}

/* ================= 状态打印函数 ================= */

/**
 * @brief 按物理存储顺序打印缓冲区（含已失效的旧数据）
 *
 * “物理顺序”= 数组下标 0,1,2,...,capacity-1 的原始内容。
 * 环形缓冲区是“逻辑上连续、物理上可能断开”的：
 * 已经读走的槽位里其实还残留旧值，直到被新数据覆盖，
 * 这里一并打印出来，便于观察“环形游标”的移动效果。
 */
static void print_physical(const int *buffer, int capacity) {
    printf("physical:");
    for (int i = 0; i < capacity; ++i) {
        printf(" %d", buffer[i]);
    }
    printf("\n");
}

/**
 * @brief 按逻辑顺序打印有效数据：从 read_index 开始，共 size 个
 *
 * “逻辑顺序”= 用户视角下真正有效的元素，从队头 read_index 开始数 size 个。
 * 因为可能跨过数组末尾回绕，所以用 (read_index + i) % capacity 计算实际下标。
 *
 * 例如容量 5、read_index=3、size=4 时：
 *   逻辑下标 0 -> 物理 3
 *   逻辑下标 1 -> 物理 4
 *   逻辑下标 2 -> 物理 0   <- 这里发生回绕
 *   逻辑下标 3 -> 物理 1
 *
 * 防御：capacity <= 0 时不能做 % 运算，直接打印提示。
 */
static void print_logical(const int *buffer, int capacity,
                          int read_index, int size) {
    printf("logical:");
    if (capacity <= 0) {
        printf(" (invalid capacity)");   /* 防御除零 */
    } else if (size == 0) {
        printf(" (empty)");
    } else {
        for (int i = 0; i < size; ++i) {
            printf(" %d", buffer[(read_index + i) % capacity]);
        }
    }
    printf("\n");
}

/**
 * @brief 打印完整状态：物理内容 + 逻辑内容 + 三个游标变量
 *
 * 调试环形缓冲区时，“三个变量” 通常指：
 *   read_index, write_index, size
 * 只看其中一两个往往无法判断状态，三者一起看才最直观。
 */
static void print_state(const int *buffer, int capacity,
                        int read_index, int write_index, int size) {
    print_physical(buffer, capacity);
    print_logical(buffer, capacity, read_index, size);
    printf("read_index = %d, write_index = %d, size = %d\n",
           read_index, write_index, size);
}

/* ================= 状态判断函数 ================= */

/**
 * @brief 判断缓冲区是否为空
 * @param size 当前有效元素个数
 * @return 1 为空，0 非空
 *
 * 只用 size 判定，简单直接；
 * 若不维护 size，需要额外用“牺牲一个槽位”或“标志位”区分空满。
 */
int ring_buffer_is_empty(int size) {
    return size == 0;
}

/**
 * @brief 判断缓冲区是否已满
 * @param size     当前有效元素个数
 * @param capacity 缓冲区容量
 * @return 1 已满，0 未满或容量非法
 *
 * 修复：增加 capacity > 0 防御。
 * 否则 size=0、capacity=0 时会错误地返回“满”。
 * size 用 >= 而非 ==，与写入函数的防御保持一致。
 */
int ring_buffer_is_full(int size, int capacity) {
    return capacity > 0 && size >= capacity;
}

/* ================= 测试辅助函数 ================= */

/**
 * @brief 整数断言检查
 * @param test_name 测试名称
 * @param actual    实际值
 * @param expected  期望值
 *
 * 这是最简易的“单元测试框架”：
 *   - 每调用一次，就代表一条断言被运行；
 *   - 相等则 PASS，否则打印期望值和实际值，方便定位。
 * 真实工程中可用 assert 或 CUnit / Unity 等框架替代。
 */
void check_int(const char *test_name, int actual, int expected) {
    tests_run++;
    if (actual == expected) {
        tests_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        printf("[FAIL] %s: expected = %d, actual = %d\n", test_name, expected, actual);
    }
}

/* ================= 主函数（测试用例） ================= */

/**
 * 测试组织思路：按“生命周期”分 6 组
 *   1) 初始空状态
 *   2) 空缓冲区读取失败（边界：读空）
 *   3) 写满缓冲区
 *   4) 满缓冲区写入失败（边界：写满）
 *   5) 读取全部数据（含 read_index 回绕）
 *   6) 再次读取空缓冲区（回到空状态，验证状态一致）
 *
 * 这是一套比较完整的边界测试，重点覆盖：
 *   - 空/满 边界
 *   - 失败操作不产生副作用
 *   - 游标回绕
 *   - size 的正确增减
 */
int main() {
    /* 环形缓冲区的“四个状态变量”：
     *   buffer[]    —— 存放数据
     *   read_index  —— 队头（下一个要读的位置）
     *   write_index —— 队尾（下一个要写的位置）
     *   size        —— 当前有效元素个数
     * 初始时全部为 0，表示空缓冲区。 */
    int buffer[BUFFER_CAPACITY] = {0};
    int read_index = 0;
    int write_index = 0;
    int size = 0;

    /* --- 第一组：测试初始空状态 --- */
    check_int("initial size", size, 0);
    check_int("initial buffer is empty", ring_buffer_is_empty(size), 1);
    check_int("initial buffer is not full", ring_buffer_is_full(size, BUFFER_CAPACITY), 0);
    /* 空缓冲区时，读写游标应指向同一位置。
     * 注意：满缓冲区时二者也会相等（见后面第三组），
     * 所以仅靠“游标相等”无法区分空/满 —— 这正是 size 存在的意义。 */
    check_int("initial indexes are equal", read_index == write_index, 1);

    /* --- 第二组：空缓冲区读取失败 --- */
    int value = -999;  /* 用一个明显的“哨兵值”便于检测是否被错误改写 */
    int result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("read empty buffer", result, 0);              /* 读取应失败 */
    check_int("size unchanged after failed read", size, 0); /* 失败无副作用：size */
    check_int("read index unchanged after failed read", read_index, 0); /* 失败无副作用：游标 */
    check_int("output unchanged after failed read", value, -999); /* 失败无副作用：输出参数 */

    /* --- 第三组：写满缓冲区 --- */
    /* 连续写入 10,20,30,40,50；容量为 5，写满后 write_index 回绕到 0。 */
    ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 10);
    ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 20);
    ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 30);
    ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 40);
    ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 50);

    check_int("size when buffer is full", size, BUFFER_CAPACITY);
    check_int("full buffer is full", ring_buffer_is_full(size, BUFFER_CAPACITY), 1);
    check_int("full buffer is not empty", ring_buffer_is_empty(size), 0);
    /* 满时读写游标也相等 —— 与空状态“长得一样”，必须靠 size 区分 */
    check_int("indexes equal when full", read_index == write_index, 1);

    /* 打印一次完整状态，既便于观察游标，也让 print_state 被实际使用。
     * 此时应看到：physical 全部有值、logical 为 10 20 30 40 50。 */
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    /* --- 第四组：满缓冲区写入失败 --- */
    /* 关键修复：快照必须在“调用写入之前”保存。
     * 若放在调用之后，比较的就是“失败后的状态”与“失败后的状态”，
     * 即使写入函数失败时错误修改了状态，断言也发现不了。 */
    int old_value = buffer[write_index];
    int old_write_index = write_index;
    int old_size = size;

    result = ring_buffer_write(buffer, BUFFER_CAPACITY, &write_index, &size, 60);

    check_int("write full buffer", result, 0); /* 写入应失败 */
    check_int("size unchanged after failed write", size, old_size);
    check_int("write index unchanged after failed write", write_index, old_write_index);
    /* 用保存下来的 old_write_index 取数据，避免拿“当前游标”去比较 */
    check_int("data unchanged after failed write", buffer[old_write_index], old_value);

    /* --- 第五组：读取全部数据 --- */
    /* 依次读出，验证 FIFO 顺序（先进先出），并观察 read_index 逐步回绕 */
    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("first read succeeds", result, 1);
    check_int("first read value", value, 10);              /* FIFO：先写先读 */
    check_int("size after first read", size, 4);           /* size 减 1 */
    check_int("read index after first read", read_index, 1);/* 读游标前进 */

    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("second read succeeds", result, 1);
    check_int("second read value", value, 20);

    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("third read succeeds", result, 1);
    check_int("third read value", value, 30);

    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("fourth read succeeds", result, 1);
    check_int("fourth read value", value, 40);

    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("fifth read succeeds", result, 1);
    check_int("fifth read value", value, 50);

    /* 读完第 5 个（下标 4）后，read_index 回绕到 0 */
    check_int("read index wrapped to zero", read_index, 0);
    check_int("indexes equal when empty", read_index == write_index, 1);
    check_int("buffer empty after all reads", ring_buffer_is_empty(size), 1);

    /* 再打印一次状态：此时 logical 应为 (empty)，
     * 但 physical 仍保留旧值，直观展示“物理残留、逻辑已空”。 */
    print_state(buffer, BUFFER_CAPACITY, read_index, write_index, size);

    /* --- 第六组：再次读取空缓冲区 --- */
    /* 再次回到空状态，验证“读空失败 + 无副作用”在完整循环后依然成立 */
    value = -999;
    int old_read_index = read_index;
    old_size = size;
    result = ring_buffer_read(buffer, BUFFER_CAPACITY, &read_index, &size, &value);
    check_int("read empty buffer again", result, 0);
    check_int("final read index unchanged", read_index, old_read_index);
    check_int("final size unchanged", size, old_size);   /* 修复：补上 size 断言 */
    check_int("final output unchanged", value, -999);

    /* 打印汇总：通过数 / 总数 */
    printf("\nTests Passed: %d/%d\n", tests_passed, tests_run);

    return 0;
}