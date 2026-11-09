# Week 02 / Day 02：基础数组队列

## 1. 今日目标

- [ ] 理解队列的先进先出
- [ ] 掌握入队 `enqueue`
- [ ] 掌握出队 `dequeue`
- [ ] 掌握查看队首 `peek`
- [ ] 处理空队列和满队列
- [ ] 理解出队时元素前移
- [ ] 完成队列边界测试

---

## 2. 今日程序

```text
queue_basic.c
queue_shift.c
queue_test.c
```

---

# 一、队列基本概念

队列遵循：

```text
先进先出
First In, First Out
FIFO
```

依次入队：

```text
10 → 20 → 30
```

出队顺序：

```text
10 → 20 → 30
```

队列两端：

```text
队首 front：数据离开的位置
队尾 rear：数据进入的位置
```

当前数组实现：

```text
队首位置       = queue[0]
队尾位置       = queue[size - 1]
下一次入队位置 = queue[size]
```

---

# 二、队列状态

```c
#define QUEUE_CAPACITY 5

int queue[QUEUE_CAPACITY];
int size = 0;
```

变量含义：

```text
capacity：最大容量
size：当前有效元素数量
```

状态判断：

```text
空队列：size == 0
满队列：size == capacity
```

必须始终满足：

```text
0 <= size <= capacity
```

---

# 三、入队 enqueue

## 函数接口

```c
int queue_enqueue(int queue[],
                  int capacity,
                  int *size,
                  int value);
```

返回值：

```text
1：入队成功
0：队列已满
```

## 核心代码

```c
int queue_enqueue(int queue[],
                  int capacity,
                  int *size,
                  int value)
{
    if (*size >= capacity)
    {
        return 0;
    }

    queue[*size] = value;
    (*size)++;

    return 1;
}
```

## 执行过程

```text
入队前：size = 2
队列：10 20

写入：queue[2] = 30
更新：size = 3

入队后：10 20 30
```

## 关键点

```text
先检查容量
再写入 queue[size]
最后让 size 加 1
```

满队列时：

```text
返回 0
size 不变
原数据不变
```

---

# 四、出队 dequeue

## 函数接口

```c
int queue_dequeue(int queue[],
                  int *size,
                  int *value);
```

返回值：

```text
1：出队成功
0：队列为空
```

出队数据通过：

```c
int *value
```

返回。

## 核心代码

```c
int queue_dequeue(int queue[],
                  int *size,
                  int *value)
{
    if (*size <= 0)
    {
        return 0;
    }

    *value = queue[0];

    for (int i = 0; i < *size - 1; i++)
    {
        queue[i] = queue[i + 1];
    }

    (*size)--;

    return 1;
}
```

## 执行过程

出队前：

```text
10 20 30 40
size = 4
```

保存队首：

```text
value = 10
```

元素前移：

```text
queue[0] = queue[1]
queue[1] = queue[2]
queue[2] = queue[3]
```

移动后的物理数组：

```text
20 30 40 40
```

更新：

```text
size = 3
```

有效队列：

```text
20 30 40
```

---

## 为什么先保存队首？

必须先执行：

```c
*value = queue[0];
```

再移动元素。

如果先移动，原来的 `queue[0]` 会被覆盖，队首数据将丢失。

---

## 为什么循环是 `i < *size - 1`？

循环中读取：

```c
queue[i + 1]
```

假设：

```text
size = 4
```

最后一次合法移动：

```c
queue[2] = queue[3]
```

所以最大 `i` 是 `2`：

```c
i < size - 1
```

如果使用：

```c
i < size
```

最后会读取：

```c
queue[size]
```

可能造成数组越界。

---

# 五、查看队首 peek

## 函数接口

```c
int queue_peek(const int queue[],
               int size,
               int *value);
```

## 核心代码

```c
int queue_peek(const int queue[],
               int size,
               int *value)
{
    if (size <= 0)
    {
        return 0;
    }

    *value = queue[0];

    return 1;
}
```

## `peek` 与 `dequeue`

| 操作 | 读取队首 | 删除队首 | 修改 `size` |
|---|---:|---:|---:|
| `peek` | 是 | 否 | 否 |
| `dequeue` | 是 | 是 | `size--` |

---

# 六、完整状态推演

依次执行：

```text
enqueue 10
enqueue 20
enqueue 30
peek
dequeue
peek
```

状态变化：

```text
初始：
队列为空，size = 0

enqueue 10：
队列 = 10，size = 1

enqueue 20：
队列 = 10 20，size = 2

enqueue 30：
队列 = 10 20 30，size = 3

peek：
返回 10
队列不变，size = 3

dequeue：
返回 10
队列 = 20 30，size = 2

peek：
返回 20
队列不变，size = 2
```

---

# 七、队列与栈对比

| 项目 | 栈 | 队列 |
|---|---|---|
| 顺序 | 后进先出 | 先进先出 |
| 添加位置 | 栈顶 | 队尾 |
| 删除位置 | 栈顶 | 队首 |
| 第一次取出 | 最后加入的数据 | 最先加入的数据 |

输入：

```text
10、20、30
```

栈的输出：

```text
30、20、10
```

队列的输出：

```text
10、20、30
```

---

# 八、复杂度

| 操作 | 时间复杂度 | 原因 |
|---|---:|---|
| `enqueue` | `O(1)` | 直接写入队尾 |
| `peek` | `O(1)` | 直接读取 `queue[0]` |
| `dequeue` | `O(n)` | 后续元素需要前移 |
| `print` | `O(n)` | 遍历全部有效元素 |

单次操作额外空间：

```text
O(1)
```

当前出队效率较低：

```text
dequeue = O(n)
```

后续环形缓冲区通过移动读写下标，将出队优化为：

```text
O(1)
```

---

# 九、边界测试

## 入队测试

```text
向空队列入队
正常连续入队
入队直到满
满队列继续入队
```

满队列入队失败后检查：

```text
返回值为 0
size 不变
队尾数据不变
```

## 出队测试

```text
空队列出队
正常出队
连续出队到空
空队列再次出队
```

空队列出队失败后检查：

```text
返回值为 0
size 不变
输出变量不变
```

## FIFO 测试

入队：

```text
10、20、30、40、50
```

出队必须是：

```text
10、20、30、40、50
```

---

# 十、常见错误

## 1. 满队列继续写入

错误：

```c
queue[*size] = value;
```

正确：

```c
if (*size >= capacity)
{
    return 0;
}
```

---

## 2. 空队列读取队首

错误：

```c
*value = queue[0];
```

没有先判断 `size`。

正确：

```c
if (*size <= 0)
{
    return 0;
}
```

---

## 3. 先移动再保存队首

错误：

```c
for (...)
{
    queue[i] = queue[i + 1];
}

*value = queue[0];
```

正确顺序：

```text
先保存 queue[0]
再移动元素
```

---

## 4. 移动循环多执行一次

错误：

```c
i < *size
```

正确：

```c
i < *size - 1
```

---

## 5. 提前减少 `size`

错误顺序：

```text
size 减 1
移动元素
```

正确顺序：

```text
保存队首
移动元素
size 减 1
```

---

## 6. 写成 `*size++`

错误：

```c
*size++;
```

正确：

```c
(*size)++;
```

出队后：

```c
(*size)--;
```

---

# 十一、嵌入式应用

队列适合处理按到达顺序执行的数据：

```text
串口接收字节
通信命令
传感器采样结果
按键事件
日志消息
待发送数据帧
任务请求
```

例如命令到达顺序：

```text
LED_ON
STATUS
LED_OFF
```

处理顺序也应为：

```text
LED_ON
STATUS
LED_OFF
```

当前数组队列需要移动元素，不适合高频数据流。

后续环形缓冲区将改为：

```text
数据不移动
只更新读写下标
```

---

# 十二、核心总结

```text
队列：先进先出
队首：queue[0]
队尾：queue[size - 1]
下一入队位置：queue[size]
```

入队：

```text
检查满队列
写入 queue[size]
size 加 1
```

出队：

```text
检查空队列
保存 queue[0]
后续元素前移
size 减 1
```

查看队首：

```text
检查空队列
读取 queue[0]
不修改 size
```

---

# 十三、复习问题

1. 队列为什么是先进先出？
2. 队首和队尾分别位于哪里？
3. 下一次入队位置为什么是 `queue[size]`？
4. 出队前为什么先保存 `queue[0]`？
5. 为什么移动循环是 `i < size - 1`？
6. 为什么出队后最后一个物理位置不需要清零？
7. `peek` 与 `dequeue` 有什么区别？
8. 为什么 `enqueue` 是 `O(1)`？
9. 为什么当前的 `dequeue` 是 `O(n)`？
10. 环形缓冲区将解决什么问题？

---

# 十四、完成情况

- [ ] 完成 `queue_basic.c`
- [ ] 完成 `queue_shift.c`
- [ ] 完成 `queue_test.c`
- [ ] 编译无警告
- [ ] AddressSanitizer 无报错
- [ ] 所有自动测试通过