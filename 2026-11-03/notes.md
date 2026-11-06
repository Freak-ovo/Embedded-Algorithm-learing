# Week 01 / Day 06：固定长度缓冲区

## 1. 今日学习目标

- [x] 理解固定长度缓冲区的基本结构
- [x] 区分缓冲区容量 `capacity` 和当前大小 `size`
- [x] 使用数组保存连续数据
- [x] 实现缓冲区添加操作
- [x] 判断缓冲区是否已满
- [x] 实现缓冲区删除操作
- [x] 删除元素后移动后续数据
- [x] 判断删除下标是否合法
- [x] 使用指针修改调用者的 `size`
- [x] 为添加和删除操作设计边界测试
- [x] 分析添加和删除操作的复杂度

---

## 2. 今日程序

```text
fixed_buffer_basic.c
fixed_buffer_remove.c
fixed_buffer_test.c
```

---

# 一、固定长度缓冲区基础

## 1. 什么是固定长度缓冲区

固定长度缓冲区本质上是一个容量固定的数组。

例如：

```c
int buffer[5];
```

这个缓冲区最多保存 5 个整数。

合法下标为：

```text
0、1、2、3、4
```

不能访问：

```c
buffer[5];
```

否则会发生数组越界。

---

## 2. 为什么嵌入式程序常用固定缓冲区

固定缓冲区的特点：

- 内存大小明确；
- 不需要动态内存分配；
- 数据连续存放；
- 访问速度快；
- 运行行为容易预测；
- 适合内存有限的嵌入式设备。

常见用途：

```text
ADC 采样数据
传感器测量结果
串口接收数据
待处理命令
最近若干次状态记录
```

---

## 3. `capacity` 和 `size`

固定缓冲区需要记录两个状态：

```c
int buffer[5];
int size = 0;
```

### `capacity`

表示数组最多可以保存多少个元素。

```text
capacity = 5
```

### `size`

表示当前已经保存了多少个有效元素。

```text
size = 0
```

例如：

```text
下标： 0   1   2   3   4
数据：10  20  30   ?   ?
状态：有效 有效 有效 空闲 空闲
```

此时：

```text
capacity = 5
size = 3
```

只有下面三个元素有效：

```c
buffer[0]
buffer[1]
buffer[2]
```

---

## 4. 核心关系

```text
容量：capacity
有效元素数量：size
有效下标：0 到 size - 1
下一次写入位置：buffer[size]
缓冲区为空：size == 0
缓冲区已满：size == capacity
```

必须始终满足：

```text
0 <= size <= capacity
```

如果出现：

```text
size > capacity
```

说明程序的状态管理已经出错。

---

# 二、程序一：fixed_buffer_basic.c

## 1. 程序目标

实现固定缓冲区的基本添加操作：

- 缓冲区未满时添加数据；
- 添加成功后更新 `size`；
- 缓冲区已满时拒绝添加；
- 打印当前有效数据。

---

## 2. 完整代码

```c
#include <stdio.h>

#define BUFFER_CAPACITY 5

int buffer_add(int buffer[],
               int capacity,
               int *size,
               int value)
{
    if (*size >= capacity)
    {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;

    return 1;
}

void buffer_print(const int buffer[], int size)
{
    printf("buffer:");

    for (int i = 0; i < size; i++)
    {
        printf(" %d", buffer[i]);
    }

    printf("\n");
}

int main(void)
{
    int buffer[BUFFER_CAPACITY];
    int size = 0;

    int values[] = {10, 20, 30, 40, 50, 60};
    int value_count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < value_count; i++)
    {
        int success = buffer_add(buffer,
                                 BUFFER_CAPACITY,
                                 &size,
                                 values[i]);

        if (success)
        {
            printf("add %d: success\n", values[i]);
        }
        else
        {
            printf("add %d: buffer full\n", values[i]);
        }

        buffer_print(buffer, size);
    }

    return 0;
}
```

---

## 3. 添加函数

函数接口：

```c
int buffer_add(int buffer[],
               int capacity,
               int *size,
               int value);
```

参数含义：

```text
buffer    缓冲区数组
capacity  缓冲区最大容量
size      当前有效元素数量的地址
value     本次需要添加的数据
```

返回值：

```text
1：添加成功
0：添加失败
```

---

## 4. 添加操作流程

```text
1. 判断缓冲区是否已满
2. 已满时返回 0
3. 未满时写入 buffer[*size]
4. 将有效元素数量加 1
5. 返回 1
```

核心代码：

```c
if (*size >= capacity)
{
    return 0;
}

buffer[*size] = value;
(*size)++;

return 1;
```

---

## 5. 为什么先检查容量

当：

```text
size = 5
capacity = 5
```

缓冲区合法下标只有：

```text
0、1、2、3、4
```

如果继续执行：

```c
buffer[*size] = value;
```

就相当于：

```c
buffer[5] = value;
```

这会造成数组越界。

因此必须先判断：

```c
if (*size >= capacity)
```

不能先写入，再检查容量。

---

## 6. 为什么先写入，再增加 `size`

正确顺序：

```c
buffer[*size] = value;
(*size)++;
```

假设：

```text
size = 3
```

执行过程：

```text
buffer[3] = value
size 从 3 变为 4
```

错误顺序：

```c
(*size)++;
buffer[*size] = value;
```

执行过程：

```text
size 先从 3 变为 4
然后写入 buffer[4]
```

这样会跳过：

```c
buffer[3]
```

所以添加操作必须遵循：

```text
先使用当前 size 写入
再让 size 加 1
```

---

## 7. 手动推演

初始：

```text
capacity = 5
size = 0
```

依次添加：

```text
10、20、30、40、50、60
```

过程：

```text
添加 10：写入 buffer[0]，size = 1
添加 20：写入 buffer[1]，size = 2
添加 30：写入 buffer[2]，size = 3
添加 40：写入 buffer[3]，size = 4
添加 50：写入 buffer[4]，size = 5
添加 60：缓冲区已满，添加失败，size 仍为 5
```

最终有效数据：

```text
10 20 30 40 50
```

---

## 8. 预期输出

```text
add 10: success
buffer: 10
add 20: success
buffer: 10 20
add 30: success
buffer: 10 20 30
add 40: success
buffer: 10 20 30 40
add 50: success
buffer: 10 20 30 40 50
add 60: buffer full
buffer: 10 20 30 40 50
```

---

## 9. 打印有效元素

打印函数：

```c
void buffer_print(const int buffer[], int size)
```

循环使用：

```c
for (int i = 0; i < size; i++)
```

而不是：

```c
for (int i = 0; i < capacity; i++)
```

原因是：

```text
capacity 表示总空间
size 表示当前有效数据量
```

打印函数只能读取有效数据。

参数使用：

```c
const int buffer[]
```

表示函数只读取数组，不修改数组内容。

---

## 10. 复杂度

### 单次添加

```text
时间复杂度：O(1)
空间复杂度：O(1)
```

原因：

- 只执行一次容量判断；
- 只写入一个元素；
- 只更新一次 `size`。

### 打印缓冲区

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

其中 `n` 是当前有效元素数量。

---

# 三、指针与 `size`

## 1. 为什么传入 `&size`

在 `main()` 中：

```c
int size = 0;
```

调用：

```c
buffer_add(buffer,
           BUFFER_CAPACITY,
           &size,
           10);
```

传入的是：

```c
&size
```

也就是 `size` 变量的地址。

函数参数：

```c
int *size
```

用于接收这个地址。

---

## 2. `size`、`&size` 和 `*size`

在 `main()` 中：

```text
size   当前有效元素数量
&size  size 变量的地址
```

在函数中：

```text
size   保存调用者 size 地址的指针
*size  访问调用者原来的 size 数据
```

关系：

```text
函数中的 size 指针 ──────> main 中的 size 变量
```

执行：

```c
(*size)++;
```

修改的是 `main()` 中原来的 `size`。

---

## 3. `p`、`*p` 和 `&p`

通用规则：

```text
p     指针中保存的地址
*p    指针指向位置中的数据
&p    指针变量本身的地址
```

例如：

```c
int value = 10;
int *p = &value;
```

执行：

```c
*p = 20;
```

改变的是：

```text
value 从 10 变为 20
```

执行：

```c
p = &other;
```

改变的是：

```text
p 保存的地址
```

执行：

```c
change_pointer(&p);
```

传递的是：

```text
指针变量 p 自己的地址
```

此时函数通常需要使用：

```c
int **pp
```

接收。

---

## 4. 为什么必须写 `(*size)++`

正确：

```c
(*size)++;
```

表示：

```text
指针指向的数据加 1
```

错误：

```c
*size++;
```

根据运算符优先级，它相当于：

```c
*(size++);
```

修改的是指针本身，而不是指针指向的数据。

同理，删除时应写：

```c
(*size)--;
```

---

# 四、程序二：fixed_buffer_remove.c

## 1. 程序目标

实现根据下标删除缓冲区元素：

- 判断下标是否合法；
- 删除指定位置的数据；
- 将后面的元素向前移动；
- 删除成功后让 `size` 减 1；
- 非法删除时保持原状态不变。

---

## 2. 完整代码

```c
#include <stdio.h>

#define BUFFER_CAPACITY 5

int buffer_add(int buffer[],
               int capacity,
               int *size,
               int value)
{
    if (*size >= capacity)
    {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;

    return 1;
}

int buffer_remove(int buffer[],
                  int *size,
                  int index)
{
    if (index < 0 || index >= *size)
    {
        return 0;
    }

    for (int i = index; i < *size - 1; i++)
    {
        buffer[i] = buffer[i + 1];
    }

    (*size)--;

    return 1;
}

void buffer_print(const int buffer[], int size)
{
    printf("buffer:");

    for (int i = 0; i < size; i++)
    {
        printf(" %d", buffer[i]);
    }

    printf("\n");
}

int main(void)
{
    int buffer[BUFFER_CAPACITY];
    int size = 0;

    buffer_add(buffer, BUFFER_CAPACITY, &size, 10);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 20);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 30);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 40);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 50);

    printf("initial:\n");
    buffer_print(buffer, size);

    int success = buffer_remove(buffer, &size, 2);

    if (success)
    {
        printf("remove index 2: success\n");
    }
    else
    {
        printf("remove index 2: invalid index\n");
    }

    buffer_print(buffer, size);

    success = buffer_remove(buffer, &size, 10);

    if (success)
    {
        printf("remove index 10: success\n");
    }
    else
    {
        printf("remove index 10: invalid index\n");
    }

    buffer_print(buffer, size);

    return 0;
}
```

---

## 3. 删除函数

函数接口：

```c
int buffer_remove(int buffer[],
                  int *size,
                  int index);
```

参数含义：

```text
buffer  缓冲区数组
size    当前有效元素数量的地址
index   需要删除的元素下标
```

返回值：

```text
1：删除成功
0：删除失败
```

---

## 4. 删除下标是否合法

假设：

```text
size = 5
```

合法下标为：

```text
0、1、2、3、4
```

非法下标包括：

```text
-1、5、6
```

判断条件：

```c
if (index < 0 || index >= *size)
{
    return 0;
}
```

这里使用：

```c
index >= *size
```

而不是：

```c
index > *size
```

因为当：

```text
index == size
```

也已经超出有效范围。

---

## 5. 删除中间元素

初始状态：

```text
下标： 0   1   2   3   4
数据：10  20  30  40  50
size = 5
```

删除：

```text
index = 2
```

也就是删除 `30`。

移动过程：

```c
buffer[2] = buffer[3];
buffer[3] = buffer[4];
```

结果：

```text
10 20 40 50
```

然后：

```c
(*size)--;
```

`size` 从 `5` 变成 `4`。

---

## 6. 元素移动循环

```c
for (int i = index; i < *size - 1; i++)
{
    buffer[i] = buffer[i + 1];
}
```

假设：

```text
size = 5
index = 2
```

循环过程：

```text
i = 2：buffer[2] = buffer[3]
i = 3：buffer[3] = buffer[4]
```

不能让 `i = 4`，否则会读取：

```c
buffer[5]
```

造成数组越界。

因此循环条件必须是：

```c
i < *size - 1
```

---

## 7. 删除第一个元素

初始：

```text
10 20 30 40 50
```

删除：

```text
index = 0
```

移动：

```text
buffer[0] = buffer[1]
buffer[1] = buffer[2]
buffer[2] = buffer[3]
buffer[3] = buffer[4]
```

结果：

```text
20 30 40 50
```

删除第一个元素需要移动后面的全部元素。

---

## 8. 删除最后一个元素

假设：

```text
size = 5
index = 4
```

循环条件：

```text
4 < 4
```

为假，因此不需要移动元素。

只需要：

```c
(*size)--;
```

结果：

```text
size 从 5 变为 4
```

删除最后一个元素时：

```text
移动次数为 0
```

---

## 9. 为什么最后一个物理位置不需要清零

删除 `30` 后，数组在内存中的内容可能为：

```text
10 20 40 50 50
```

但此时：

```text
size = 4
```

所以只有下面四个元素有效：

```text
10 20 40 50
```

最后一个位置中的旧数据已经位于有效范围之外。

缓冲区有效内容由下面的范围决定：

```text
buffer[0] 到 buffer[size - 1]
```

因此，最后一个位置不清零也不影响正确性。

这属于逻辑删除：

```text
通过减小 size，使最后一个位置失效
```

如果为了调试方便，可以清零：

```c
(*size)--;
buffer[*size] = 0;
```

但这不是删除操作正确性的必要条件。

---

## 10. 为什么先移动，再减小 `size`

正确顺序：

```c
for (int i = index; i < *size - 1; i++)
{
    buffer[i] = buffer[i + 1];
}

(*size)--;
```

移动元素时，需要使用删除前的有效长度。

如果提前执行：

```c
(*size)--;
```

循环可访问的范围会提前缩小，可能导致最后一个有效元素无法正确向前移动。

所以必须：

```text
先根据原 size 移动数据
再让 size 减 1
```

---

## 11. 预期输出

```text
initial:
buffer: 10 20 30 40 50
remove index 2: success
buffer: 10 20 40 50
remove index 10: invalid index
buffer: 10 20 40 50
```

---

## 12. 删除操作复杂度

删除最后一个元素：

```text
最好时间复杂度：O(1)
```

删除第一个元素：

```text
需要移动其余元素
最坏时间复杂度：O(n)
```

通常使用最坏情况表示：

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

---

# 五、程序三：fixed_buffer_test.c

## 1. 程序目标

对缓冲区的添加和删除操作进行完整测试。

覆盖以下情况：

```text
向空缓冲区添加
添加直到缓冲区满
满缓冲区继续添加
删除中间元素
删除第一个元素
删除最后一个元素
删除负数下标
删除越界下标
删除空缓冲区元素
```

---

## 2. 完整代码

```c
#include <stdio.h>

#define BUFFER_CAPACITY 5

static int tests_run = 0;
static int tests_passed = 0;

int buffer_add(int buffer[],
               int capacity,
               int *size,
               int value)
{
    if (*size >= capacity)
    {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;

    return 1;
}

int buffer_remove(int buffer[],
                  int *size,
                  int index)
{
    if (index < 0 || index >= *size)
    {
        return 0;
    }

    for (int i = index; i < *size - 1; i++)
    {
        buffer[i] = buffer[i + 1];
    }

    (*size)--;

    return 1;
}

void check_int(const char *test_name,
               int actual,
               int expected)
{
    tests_run++;

    if (actual == expected)
    {
        tests_passed++;
        printf("[PASS] %s\n", test_name);
    }
    else
    {
        printf("[FAIL] %s: expected = %d, actual = %d\n",
               test_name,
               expected,
               actual);
    }
}

int main(void)
{
    int buffer[BUFFER_CAPACITY];
    int size = 0;
    int result;

    /* 测试一：向空缓冲区添加元素 */
    result = buffer_add(buffer,
                        BUFFER_CAPACITY,
                        &size,
                        10);

    check_int("add to empty buffer", result, 1);
    check_int("size after first add", size, 1);
    check_int("value after first add", buffer[0], 10);

    /* 测试二：添加元素直到缓冲区满 */
    buffer_add(buffer, BUFFER_CAPACITY, &size, 20);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 30);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 40);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 50);

    check_int("size when buffer is full",
              size,
              BUFFER_CAPACITY);

    check_int("last element in full buffer",
              buffer[4],
              50);

    /* 测试三：满缓冲区继续添加 */
    result = buffer_add(buffer,
                        BUFFER_CAPACITY,
                        &size,
                        60);

    check_int("add to full buffer", result, 0);
    check_int("size unchanged after failed add", size, 5);
    check_int("last value unchanged after failed add",
              buffer[4],
              50);

    /* 测试四：删除中间元素 */
    result = buffer_remove(buffer, &size, 2);

    check_int("remove middle element", result, 1);
    check_int("size after middle removal", size, 4);
    check_int("shifted middle value", buffer[2], 40);
    check_int("shifted last value", buffer[3], 50);

    /* 测试五：删除负数下标 */
    result = buffer_remove(buffer, &size, -1);

    check_int("remove negative index", result, 0);
    check_int("size unchanged after negative index",
              size,
              4);

    /* 测试六：删除越界下标 */
    result = buffer_remove(buffer, &size, 10);

    check_int("remove out-of-range index", result, 0);
    check_int("size unchanged after invalid index",
              size,
              4);

    /* 测试七：删除第一个元素 */
    result = buffer_remove(buffer, &size, 0);

    check_int("remove first element", result, 1);
    check_int("size after first removal", size, 3);
    check_int("new first element", buffer[0], 20);

    /* 测试八：删除最后一个元素 */
    result = buffer_remove(buffer, &size, size - 1);

    check_int("remove last element", result, 1);
    check_int("size after last removal", size, 2);
    check_int("remaining last element", buffer[1], 40);

    /* 测试九：删除空缓冲区元素 */
    int empty_buffer[BUFFER_CAPACITY];
    int empty_size = 0;

    result = buffer_remove(empty_buffer, &empty_size, 0);

    check_int("remove from empty buffer", result, 0);
    check_int("empty size unchanged", empty_size, 0);

    printf("\nTests passed: %d/%d\n",
           tests_passed,
           tests_run);

    return 0;
}
```

---

## 3. 测试函数

```c
void check_int(const char *test_name,
               int actual,
               int expected);
```

参数含义：

```text
test_name  测试名称
actual     程序实际得到的值
expected   预期的正确值
```

判断：

```c
if (actual == expected)
```

结果相同：

```text
PASS
```

结果不同：

```text
FAIL
```

---

## 4. 测试统计

```c
static int tests_run = 0;
static int tests_passed = 0;
```

含义：

```text
tests_run     已执行测试数量
tests_passed  已通过测试数量
```

每次测试：

```c
tests_run++;
```

测试通过：

```c
tests_passed++;
```

最终输出：

```c
printf("Tests passed: %d/%d\n",
       tests_passed,
       tests_run);
```

本程序预期：

```text
Tests passed: 26/26
```

---

## 5. 为什么不能只检查返回值

例如：

```c
int result = buffer_add(buffer,
                        BUFFER_CAPACITY,
                        &size,
                        10);
```

只检查：

```c
check_int("add result", result, 1);
```

只能证明函数返回了成功，不能证明数据真的正确写入。

还需要检查：

```c
check_int("size after add", size, 1);
check_int("value after add", buffer[0], 10);
```

因此，每次操作通常要验证三个方面：

```text
1. 返回值
2. 状态变化
3. 数据内容
```

---

## 6. 成功操作的测试原则

操作成功后，需要验证：

```text
返回值是否表示成功
size 是否正确变化
数组内容是否正确变化
```

例如删除中间元素：

```text
返回值应该为 1
size 应该减 1
后面的元素应该向前移动
```

---

## 7. 失败操作的测试原则

操作失败时，需要验证：

```text
返回值表示失败
size 保持不变
原数据保持不变
```

例如满缓冲区继续添加：

```text
返回值应该为 0
size 仍然等于 capacity
已有数据不能被覆盖
```

非法删除也是一样：

```text
返回值应该为 0
size 不能变化
缓冲区内容不能变化
```

核心原则：

```text
成功时正确改变状态
失败时保持原状态
```

---

## 8. 空缓冲区测试

空缓冲区：

```c
int empty_buffer[BUFFER_CAPACITY];
int empty_size = 0;
```

尝试删除：

```c
buffer_remove(empty_buffer, &empty_size, 0);
```

判断：

```text
index = 0
size = 0
index >= size
0 >= 0
```

条件成立，所以返回失败。

注意：

```text
空缓冲区没有任何合法下标
```

即使数组物理容量为 5，也不能删除 `buffer[0]`，因为当前：

```text
size = 0
```

---

## 9. 边界测试总结

缓冲区添加操作至少需要测试：

```text
向空缓冲区添加
正常添加
添加到满
满后继续添加
```

缓冲区删除操作至少需要测试：

```text
删除第一个元素
删除中间元素
删除最后一个元素
删除负数下标
删除 index == size
删除更大的越界下标
删除空缓冲区元素
```

---

# 六、编译与运行

## 1. 创建构建目录

```bash
mkdir -p build
```

---

## 2. 编译基础程序

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    fixed_buffer_basic.c \
    -o build/fixed_buffer_basic
```

运行：

```bash
./build/fixed_buffer_basic
```

---

## 3. 编译删除程序

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    fixed_buffer_remove.c \
    -o build/fixed_buffer_remove
```

运行：

```bash
./build/fixed_buffer_remove
```

---

## 4. 编译测试程序

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    fixed_buffer_test.c \
    -o build/fixed_buffer_test
```

运行：

```bash
./build/fixed_buffer_test
```

检查：

- 没有编译错误；
- 没有编译警告；
- 没有 AddressSanitizer 报错；
- 所有测试显示 `[PASS]`；
- 最后显示 `Tests passed: 26/26`。

---

# 七、复杂度总结

| 操作 | 时间复杂度 | 空间复杂度 | 原因 |
|---|---:|---:|---|
| 添加元素 | `O(1)` | `O(1)` | 直接写入 `buffer[size]` |
| 删除最后一个元素 | `O(1)` | `O(1)` | 不需要移动元素 |
| 删除第一个元素 | `O(n)` | `O(1)` | 后面所有元素需要前移 |
| 删除中间元素 | `O(n)` | `O(1)` | 部分后续元素需要前移 |
| 打印缓冲区 | `O(n)` | `O(1)` | 需要访问全部有效元素 |
| 单次整数测试 | `O(1)` | `O(1)` | 只进行一次比较 |

删除操作通常按最坏情况表示：

```text
时间复杂度：O(n)
空间复杂度：O(1)
```

---

# 八、常见错误记录

## 1. 添加前没有检查容量

错误：

```c
buffer[*size] = value;
(*size)++;
```

如果缓冲区已满，会写入：

```c
buffer[capacity]
```

导致数组越界。

正确：

```c
if (*size >= capacity)
{
    return 0;
}
```

---

## 2. 先增加 `size` 再写入

错误：

```c
(*size)++;
buffer[*size] = value;
```

会跳过当前正确的写入位置。

正确：

```c
buffer[*size] = value;
(*size)++;
```

---

## 3. 忘记使用指针修改 `size`

错误接口：

```c
int buffer_add(int buffer[],
               int capacity,
               int size,
               int value);
```

这里的 `size` 是副本。

函数内部修改：

```c
size++;
```

不会影响调用者。

正确接口：

```c
int buffer_add(int buffer[],
               int capacity,
               int *size,
               int value);
```

调用：

```c
buffer_add(buffer, capacity, &size, value);
```

---

## 4. 写成 `*size++`

错误：

```c
*size++;
```

它移动的是指针。

正确：

```c
(*size)++;
```

删除时：

```c
(*size)--;
```

---

## 5. 删除时使用错误的下标判断

错误：

```c
if (index < 0 || index > *size)
```

它错误地允许：

```text
index == size
```

正确：

```c
if (index < 0 || index >= *size)
```

---

## 6. 删除时循环范围错误

错误：

```c
for (int i = index; i < *size; i++)
{
    buffer[i] = buffer[i + 1];
}
```

最后一次会访问：

```c
buffer[*size]
```

这已经超出有效范围，容量已满时还可能直接数组越界。

正确：

```c
for (int i = index; i < *size - 1; i++)
{
    buffer[i] = buffer[i + 1];
}
```

---

## 7. 删除前提前减少 `size`

错误：

```c
(*size)--;

for (int i = index; i < *size - 1; i++)
{
    buffer[i] = buffer[i + 1];
}
```

这会提前缩小移动范围，可能遗漏最后一个需要移动的元素。

正确顺序：

```text
先移动元素
再让 size 减 1
```

---

## 8. 使用 `capacity` 判断删除范围

错误思想：

```text
只要 index < capacity，就可以删除
```

实际应根据：

```text
index < size
```

原因：

```text
capacity 表示物理空间
size 表示当前有效数据
```

只能删除有效数据。

---

# 九、与嵌入式系统的联系

固定缓冲区可用于保存：

```text
固定数量的 ADC 采样值
待发送的数据帧
传感器历史数据
命令处理列表
故障码记录
```

连续数组删除存在一个缺点：

```text
删除前面元素时，需要移动大量数据
```

例如，删除第一个元素：

```text
移动次数约为 size - 1
```

如果缓冲区很大或者删除操作频繁，移动开销会增加。

后续学习队列和环形缓冲区时，可以通过维护读写下标避免频繁移动元素。

例如串口接收常使用环形缓冲区：

```text
不移动所有数据
只更新读指针和写指针
```

本日的固定缓冲区是理解环形缓冲区的基础。

---

# 十、今日核心总结

## 1. 缓冲区状态

```text
capacity：最大容量
size：当前有效元素数量
有效下标：0 到 size - 1
下一写入位置：buffer[size]
```

## 2. 添加操作

```c
if (*size >= capacity)
{
    return 0;
}

buffer[*size] = value;
(*size)++;

return 1;
```

## 3. 删除操作

```c
if (index < 0 || index >= *size)
{
    return 0;
}

for (int i = index; i < *size - 1; i++)
{
    buffer[i] = buffer[i + 1];
}

(*size)--;

return 1;
```

## 4. 指针作用

```text
传入 &size
函数使用 int *size 接收
通过 *size 访问原变量
通过 (*size)++ 或 (*size)-- 修改原变量
```

## 5. 测试原则

```text
检查返回值
检查 size
检查数组内容
```

失败操作还要确认：

```text
size 不变
原数据不变
```

---

# 十一、复习问题

复习时应能回答：

1. `capacity` 和 `size` 有什么区别？
2. 为什么缓冲区的有效下标由 `size` 决定？
3. 为什么下一次写入位置是 `buffer[size]`？
4. 为什么添加前要判断 `*size >= capacity`？
5. 为什么添加时先写入，再让 `size` 加 1？
6. 为什么函数参数使用 `int *size`？
7. `size`、`&size` 和 `*size` 分别表示什么？
8. 为什么必须写 `(*size)++`，不能写 `*size++`？
9. 删除元素后为什么要将后续元素向前移动？
10. 为什么合法删除下标必须满足 `index < size`？
11. 为什么 `index == size` 也属于非法下标？
12. 删除最后一个元素时为什么不需要移动？
13. 为什么删除后最后一个物理位置不清零也可以？
14. 为什么删除操作先移动元素，再减少 `size`？
15. 为什么添加操作是 `O(1)`？
16. 为什么删除第一个元素是 `O(n)`？
17. 为什么测试不能只检查函数返回值？
18. 操作失败后，哪些状态必须保持不变？

---

# 十二、Day 06 完成情况

- [x] 完成 `fixed_buffer_basic.c`
- [x] 完成 `fixed_buffer_remove.c`
- [x] 完成 `fixed_buffer_test.c`
- [x] 理解 `capacity` 和 `size`
- [x] 掌握缓冲区添加操作
- [x] 掌握缓冲区删除操作
- [x] 理解数组元素前移
- [x] 使用指针修改调用者变量
- [x] 完成满缓冲区测试
- [x] 完成空缓冲区测试
- [x] 完成非法下标测试
- [x] 完成复杂度分析

---

# 十三、Git 提交

检查当前改动：

```bash
cd ~/projects/embedded-algorithm-learning

git status
git diff
```

添加 Day 06 文件：

```bash
git add week01/day06
```

检查暂存内容：

```bash
git diff --cached
```

提交：

```bash
git commit -m "feat: complete day06 fixed buffer exercises"
```

推送：

```bash
git push
```

检查结果：

```bash
git status
git log --oneline -5
```

预期状态：

```text
nothing to commit, working tree clean
```

---

# 十四、下一步

下一阶段继续学习数组数据结构，并逐步过渡到：

```text
队列
栈
环形缓冲区
```

固定长度缓冲区的核心基础是：

```text
数组保存数据
size 记录有效数量
写入前检查容量
删除前检查下标
失败操作保持原状态
```