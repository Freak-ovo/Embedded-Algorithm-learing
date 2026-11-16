# Week 02 / Day 03：环形缓冲区基础

## 1. 今日目标

- [ ] 理解环形缓冲区的作用
- [ ] 理解 `read_index` 和 `write_index`
- [ ] 掌握下标回绕
- [ ] 实现写入 `write`
- [ ] 实现读取 `read`
- [ ] 判断缓冲区为空和已满
- [ ] 理解生产者与消费者模型
- [ ] 完成边界测试

---

## 2. 今日程序

```text
ring_buffer_basic.c
ring_buffer_wrap.c
ring_buffer_state.c
```

---

# 一、为什么需要环形缓冲区

普通数组队列出队时需要移动元素：

```c
for (int i = 0; i < size - 1; i++)
{
    queue[i] = queue[i + 1];
}
```

因此：

```text
入队：O(1)
出队：O(n)
```

环形缓冲区不移动数据，只更新读写下标：

```text
写入：O(1)
读取：O(1)
```

---

# 二、核心状态

```c
#define BUFFER_CAPACITY 5

int buffer[BUFFER_CAPACITY];
int read_index = 0;
int write_index = 0;
int size = 0;
```

变量职责：

```text
buffer       保存数据
read_index   下一次读取的位置
write_index  下一次写入的位置
size         当前有效元素数量
capacity     最大容量
```

必须满足：

```text
0 <= read_index < capacity
0 <= write_index < capacity
0 <= size <= capacity
```

---

# 三、空和满

## 空缓冲区

```c
size == 0
```

此时不能读取。

## 满缓冲区

```c
size == capacity
```

此时不能继续写入。

## 为什么不能只比较读写下标

下面两种状态都可能出现：

```text
read_index == write_index
```

情况一：

```text
size == 0
缓冲区为空
```

情况二：

```text
size == capacity
缓冲区已满
```

因此本阶段使用 `size` 区分空和满。

---

# 四、写入操作

## 函数接口

```c
int ring_buffer_write(int buffer[],
                      int capacity,
                      int *write_index,
                      int *size,
                      int value);
```

返回值：

```text
1：写入成功
0：缓冲区已满
```

## 核心代码

```c
int ring_buffer_write(int buffer[],
                      int capacity,
                      int *write_index,
                      int *size,
                      int value)
{
    if (*size >= capacity)
    {
        return 0;
    }

    buffer[*write_index] = value;

    *write_index = (*write_index + 1) % capacity;
    (*size)++;

    return 1;
}
```

## 执行顺序

```text
1. 检查缓冲区是否已满
2. 写入 buffer[write_index]
3. 更新 write_index
4. size 加 1
5. 返回成功
```

---

# 五、读取操作

## 函数接口

```c
int ring_buffer_read(const int buffer[],
                     int capacity,
                     int *read_index,
                     int *size,
                     int *value);
```

返回值：

```text
1：读取成功
0：缓冲区为空
```

## 核心代码

```c
int ring_buffer_read(const int buffer[],
                     int capacity,
                     int *read_index,
                     int *size,
                     int *value)
{
    if (*size <= 0)
    {
        return 0;
    }

    *value = buffer[*read_index];

    *read_index = (*read_index + 1) % capacity;
    (*size)--;

    return 1;
}
```

## 执行顺序

```text
1. 检查缓冲区是否为空
2. 读取 buffer[read_index]
3. 更新 read_index
4. size 减 1
5. 返回成功
```

---

# 六、下标回绕

数组容量：

```text
capacity = 5
```

合法下标：

```text
0、1、2、3、4
```

下标变化：

```text
0 → 1 → 2 → 3 → 4 → 0
```

实现：

```c
index = (index + 1) % capacity;
```

例如：

```text
index = 4
capacity = 5
```

计算：

```text
(4 + 1) % 5 = 0
```

因此下标回到数组开头。

---

# 七、状态推演

初始状态：

```text
read_index = 0
write_index = 0
size = 0
```

## 写入 10

写入位置：

```text
buffer[0] = 10
```

更新：

```text
write_index = 1
size = 1
```

## 写入 20

```text
buffer[1] = 20
write_index = 2
size = 2
```

## 写入 30

```text
buffer[2] = 30
write_index = 3
size = 3
```

当前状态：

```text
物理数组：10 20 30 ? ?
read_index = 0
write_index = 3
size = 3
```

## 读取一次

读取：

```text
buffer[read_index]
buffer[0] = 10
```

更新：

```text
read_index = 1
size = 2
```

当前有效数据：

```text
20 30
```

注意：

```text
buffer[0] 中可能仍然保存 10
但它已不属于有效数据
```

---

# 八、回绕过程

容量为 `5`，当前状态：

```text
read_index = 2
write_index = 4
size = 2
```

写入一个元素：

```text
写入 buffer[4]
write_index = (4 + 1) % 5
write_index = 0
```

下一次写入会从：

```text
buffer[0]
```

开始。

这就是“环形”的含义：

```text
数组物理上仍是连续的
逻辑下标到末尾后回到开头
```

---

# 九、物理顺序与逻辑顺序

假设物理数组：

```text
下标： 0   1   2   3   4
数据：60  70  30  40  50
```

状态：

```text
read_index = 2
write_index = 2
size = 5
```

逻辑读取顺序不是：

```text
60 70 30 40 50
```

而是从 `read_index` 开始：

```text
30 40 50 60 70
```

因此环形缓冲区中要区分：

```text
物理数组顺序
逻辑数据顺序
```

逻辑顺序由 `read_index` 和 `size` 决定。

---

# 十、与普通队列对比

| 项目 | 普通数组队列 | 环形缓冲区 |
|---|---|---|
| 写入位置 | `queue[size]` | `buffer[write_index]` |
| 读取位置 | `queue[0]` | `buffer[read_index]` |
| 出队后移动数据 | 是 | 否 |
| 写入复杂度 | `O(1)` | `O(1)` |
| 读取复杂度 | `O(n)` | `O(1)` |
| 状态变量 | `size` | `read_index`、`write_index`、`size` |

---

# 十一、生产者与消费者

环形缓冲区经常连接两个不同的执行过程：

```text
生产者 → 环形缓冲区 → 消费者
```

## 生产者

负责生成或接收数据：

```text
UART 接收中断
ADC 采样函数
SPI DMA 完成回调
传感器采集任务
```

生产者执行：

```c
ring_buffer_write(...);
```

## 消费者

负责处理数据：

```text
主循环
协议解析任务
滤波算法
日志处理线程
```

消费者执行：

```c
ring_buffer_read(...);
```

---

# 十二、通信中的应用

## UART

```text
UART 中断收到字节
        ↓
写入环形缓冲区
        ↓
主循环读取
        ↓
协议解析
```

## SPI 和 I²C

SPI、I²C 更常将完整事务放入请求队列：

```text
设备地址或片选
发送缓冲区
接收缓冲区
数据长度
读写方向
完成回调
```

真正的数据流也可以配合环形缓冲区或 DMA 缓冲区使用。

## Linux 驱动

类似思想可见于：

```text
字符设备接收缓冲区
串口 FIFO
kfifo
网络包队列
工作队列
生产者和消费者模型
```

实际内核代码还需要处理：

```text
并发
锁
等待与唤醒
中断上下文
内存屏障
```

---

# 十三、边界测试

## 空缓冲区读取

初始：

```text
size = 0
```

读取应满足：

```text
返回 0
size 不变
read_index 不变
输出变量不变
```

## 缓冲区写满

连续写入 `capacity` 个元素：

```text
size == capacity
```

继续写入应满足：

```text
返回 0
size 不变
write_index 不变
原数据不变
```

## 下标回绕

需要测试：

```text
write_index 从 capacity - 1 回到 0
read_index 从 capacity - 1 回到 0
```

## FIFO 顺序

写入：

```text
10、20、30、40、50
```

读取必须是：

```text
10、20、30、40、50
```

## 交替读写

测试过程：

```text
写入 10、20、30
读取 10、20
写入 40、50、60
读取剩余全部数据
```

最终读取顺序应为：

```text
30、40、50、60
```

---

# 十四、常见错误

## 1. 写入后没有更新下标

错误：

```c
buffer[*write_index] = value;
(*size)++;
```

缺少：

```c
*write_index = (*write_index + 1) % capacity;
```

结果是每次都覆盖同一个位置。

---

## 2. 读取后没有更新下标

错误：

```c
*value = buffer[*read_index];
(*size)--;
```

缺少：

```c
*read_index = (*read_index + 1) % capacity;
```

结果是每次都读取同一个位置。

---

## 3. 忘记取模

错误：

```c
(*write_index)++;
```

下标最终可能变成：

```text
capacity
```

导致越界。

正确：

```c
*write_index = (*write_index + 1) % capacity;
```

---

## 4. 先更新写下标再写入

错误：

```c
*write_index = (*write_index + 1) % capacity;
buffer[*write_index] = value;
```

这会跳过当前应写入的位置。

正确顺序：

```text
先写入当前 write_index
再更新 write_index
```

---

## 5. 先更新读下标再读取

错误：

```c
*read_index = (*read_index + 1) % capacity;
*value = buffer[*read_index];
```

会跳过当前队首数据。

正确顺序：

```text
先读取当前 read_index
再更新 read_index
```

---

## 6. 只用 `read_index == write_index` 判断空

当前实现中：

```text
read_index == write_index
```

可能表示空，也可能表示满。

必须同时使用：

```text
size
```

判断状态。

---

## 7. 失败操作仍修改状态

写入失败时不能修改：

```text
write_index
size
原数据
```

读取失败时不能修改：

```text
read_index
size
输出变量
```

---

# 十五、复杂度

| 操作 | 时间复杂度 | 额外空间 |
|---|---:|---:|
| 写入 | `O(1)` | `O(1)` |
| 读取 | `O(1)` | `O(1)` |
| 判断为空 | `O(1)` | `O(1)` |
| 判断已满 | `O(1)` | `O(1)` |
| 打印有效数据 | `O(n)` | `O(1)` |

环形缓冲区使用固定数组保存数据：

```text
存储空间：O(n)
```

---

# 十六、核心总结

环形缓冲区的五个关键状态：

```text
buffer
capacity
read_index
write_index
size
```

写入：

```text
检查是否已满
写入 buffer[write_index]
write_index 向前移动并回绕
size 加 1
```

读取：

```text
检查是否为空
读取 buffer[read_index]
read_index 向前移动并回绕
size 减 1
```

下标回绕：

```c
index = (index + 1) % capacity;
```

核心优势：

```text
不移动已有数据
写入和读取都是 O(1)
```

---

# 十七、复习问题

1. 普通数组队列的出队为什么是 `O(n)`？
2. 环形缓冲区为什么不需要移动数据？
3. `read_index` 指向什么位置？
4. `write_index` 指向什么位置？
5. `size` 的作用是什么？
6. 如何判断缓冲区为空？
7. 如何判断缓冲区已满？
8. 为什么读写下标需要取模？
9. 为什么必须先写入再更新 `write_index`？
10. 为什么必须先读取再更新 `read_index`？
11. 为什么 `read_index == write_index` 不能单独判断空？
12. 物理数组顺序和逻辑读取顺序有什么区别？
13. 环形缓冲区如何用于 UART 接收？
14. 什么是生产者和消费者？

---

# 十八、完成情况

- [ ] 完成 `ring_buffer_basic.c`
- [ ] 完成 `ring_buffer_wrap.c`
- [ ] 完成 `ring_buffer_state.c`
- [ ] 写入顺序正确
- [ ] 读取顺序正确
- [ ] 读写下标可以回绕
- [ ] 空缓冲区读取安全
- [ ] 满缓冲区写入安全
- [ ] 编译无警告
- [ ] AddressSanitizer 无报错

---

# 十九、Git 提交

## 查看改动

```bash
cd ~/projects/embedded-algorithm-learning

git status
git diff
```

## 添加当天文件

```bash
git add week02/day03
```

## 检查暂存内容

```bash
git diff --cached
```

确认没有提交：

```text
build 目录
可执行文件
目标文件
无关临时文件
```

## 提交

```bash
git commit -m "feat: complete week02 day03 ring buffer basics"
```

## 推送

```bash
git push
```

## 检查状态

```bash
git status
git log --oneline -5
```

预期：

```text
nothing to commit, working tree clean
```

项目根目录建议包含：

```gitignore
build/
*.o
*.out
```

---

