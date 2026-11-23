# Week 02 / Day 05：UART 接收与环形缓冲区应用

## 1. 今日目标

- [ ] 理解 UART 接收中的生产者和消费者
- [ ] 使用环形缓冲区暂存接收字节
- [ ] 按顺序读取 UART 数据
- [ ] 根据换行符提取完整命令
- [ ] 处理缓冲区满和数据丢失
- [ ] 统计接收、处理和丢失的数据
- [ ] 理解中断与主循环的职责划分
- [ ] 复用已有环形缓冲区模块

---

## 2. 今日程序

```text
uart_rx_sim.c
command_parser.c
uart_rx_test.c
```

文件职责：

```text
uart_rx_sim.c
模拟 UART 逐字节接收

command_parser.c
从字节流中提取完整命令

uart_rx_test.c
测试正常接收、溢出和边界情况
```

本日继续复用：

```text
../day04/ring_buffer.h
../day04/ring_buffer.c
```

不再重复实现：

```text
ring_buffer_write()
ring_buffer_read()
ring_buffer_peek()
```

---

# 一、UART 数据接收模型

UART 数据通常逐字节到达：

```text
'T' → 'E' → 'M' → 'P' → '=' → '2' → '5' → '\n'
```

如果字符 `'\n'` 代表命令结束，那么完整命令为：

```text
TEMP=25
```

典型数据流程：

```text
UART 接收中断
      ↓
读取一个字节
      ↓
写入环形缓冲区
      ↓
主循环读取字节
      ↓
命令解析器
      ↓
得到完整命令
```

---

# 二、生产者与消费者

## 生产者

负责产生数据：

```text
UART 接收中断
DMA 接收完成回调
测试程序中的模拟接收函数
```

操作：

```c
ring_buffer_write(&uart_rx_buffer, byte);
```

## 消费者

负责读取并处理数据：

```text
主循环
协议解析任务
命令处理任务
```

操作：

```c
ring_buffer_read(&uart_rx_buffer, &byte);
```

数据关系：

```text
生产者 → 环形缓冲区 → 消费者
```

---

# 三、中断和主循环的职责

## UART 接收中断

中断中只做必要工作：

```text
读取硬件接收寄存器
把字节写入缓冲区
更新错误或丢失计数
快速退出中断
```

伪代码：

```c
void uart_rx_interrupt(void)
{
    int byte = uart_read_byte();

    if (!ring_buffer_write(&uart_rx_buffer, byte))
    {
        dropped_count++;
    }
}
```

## 主循环

主循环负责耗时操作：

```text
读取缓冲区
组合命令
解析参数
执行控制逻辑
打印日志
```

伪代码：

```c
while (1)
{
    int byte;

    if (ring_buffer_read(&uart_rx_buffer, &byte))
    {
        command_parser_feed((char)byte);
    }
}
```

核心原则：

```text
中断只收数据
主循环负责处理数据
```

---

# 四、为什么使用环形缓冲区

如果 UART 在主程序忙碌时收到数据，不能等待主程序立即处理。

环形缓冲区提供暂存空间：

```text
UART 到达速度暂时较快
        ↓
数据进入环形缓冲区
        ↓
主循环稍后按 FIFO 顺序处理
```

优点：

```text
写入 O(1)
读取 O(1)
不需要移动数组元素
保持接收顺序
适合固定内存系统
```

注意：

> 环形缓冲区只能缓冲暂时的速度差，不能解决消费者长期处理过慢的问题。

如果 UART 长期接收速度大于处理速度，缓冲区最终仍会写满。

---

# 五、字符与整数缓冲区

Day 04 的模块保存：

```c
int data[RING_BUFFER_CAPACITY];
```

UART 字节类型通常是：

```c
unsigned char
```

当前学习阶段可以把接收到的字符转换为 `int` 保存：

```c
ring_buffer_write(&uart_rx_buffer, (int)byte);
```

读取后再转换：

```c
int value;

ring_buffer_read(&uart_rx_buffer, &value);

char byte = (char)value;
```

正式 UART 模块更适合将缓冲区定义为：

```c
unsigned char data[UART_RX_CAPACITY];
```

本阶段先复用现有整数版模块，避免再次重复实现数据结构。

---

# 六、命令结束符

本实验使用：

```c
'\n'
```

表示一条命令结束。

输入：

```text
LED_ON\n
```

解析结果：

```text
LED_ON
```

输入：

```text
SET=25\nSTATUS\n
```

解析结果：

```text
第一条：SET=25
第二条：STATUS
```

如果串口终端发送：

```text
\r\n
```

则解析器通常忽略：

```c
'\r'
```

并使用：

```c
'\n'
```

作为结束标志。

---

# 七、命令解析状态

定义命令缓冲区：

```c
#define COMMAND_CAPACITY 32

char command[COMMAND_CAPACITY];
int command_length = 0;
```

变量职责：

```text
command
保存当前正在接收的命令

command_length
当前已经保存的字符数量
```

---

## 接收普通字符

```c
command[command_length] = byte;
command_length++;
```

---

## 接收到换行符

```c
command[command_length] = '\0';
```

形成 C 字符串。

例如接收到：

```text
'S' 'T' 'A' 'T' 'U' 'S'
```

添加字符串结束符后：

```text
'S' 'T' 'A' 'T' 'U' 'S' '\0'
```

此时可以：

```c
printf("%s\n", command);
```

处理完成后：

```c
command_length = 0;
```

开始接收下一条命令。

---

# 八、命令缓冲区边界

必须为字符串结束符预留一个位置。

如果：

```c
#define COMMAND_CAPACITY 32
```

最多只能保存：

```text
31 个有效字符
```

最后一个位置用于：

```c
'\0'
```

写入普通字符前判断：

```c
if (command_length < COMMAND_CAPACITY - 1)
{
    command[command_length] = byte;
    command_length++;
}
```

不能使用：

```c
command_length < COMMAND_CAPACITY
```

否则再添加 `'\0'` 时可能越界。

---

# 九、空命令

收到：

```text
\n
```

此时：

```text
command_length == 0
```

可以选择：

```text
忽略空命令
```

判断：

```c
if (byte == '\n')
{
    if (command_length > 0)
    {
        command[command_length] = '\0';
        process_command(command);
    }

    command_length = 0;
}
```

---

# 十、过长命令处理

如果命令超过 `COMMAND_CAPACITY - 1`，不能继续写数组。

可选策略：

```text
策略一：丢弃多余字符
策略二：清空整条命令
策略三：设置错误标志，等待换行后恢复
```

本实验采用：

```text
设置 overflow 标志
丢弃本条命令剩余字符
收到换行后恢复
```

状态：

```c
int command_overflow = 0;
```

字符过多时：

```c
command_overflow = 1;
```

收到换行时：

```c
if (command_overflow)
{
    printf("command too long\n");
}
else
{
    process_command(command);
}

command_length = 0;
command_overflow = 0;
```

---

# 十一、环形缓冲区溢出

如果 UART 接收缓冲区已满：

```c
ring_buffer_write()
```

返回：

```text
0
```

需要记录：

```c
int dropped_count = 0;
```

写入失败：

```c
if (!ring_buffer_write(&uart_rx_buffer, byte))
{
    dropped_count++;
}
```

本实验采用：

```text
拒绝新数据
保留缓冲区中已有数据
记录丢失字节数量
```

其他可能策略：

```text
覆盖最旧数据
扩大缓冲区
暂停接收端
使用硬件流控 RTS/CTS
提高消费者处理速度
```

---

# 十二、统计关系

定义：

```c
int received_count = 0;
int processed_count = 0;
int dropped_count = 0;
```

含义：

```text
received_count
总共到达了多少字节

processed_count
已经从环形缓冲区读取了多少字节

dropped_count
因为缓冲区满而丢失了多少字节
```

任意时刻应满足：

```text
received_count
=
processed_count
+ dropped_count
+ ring_buffer_size(&uart_rx_buffer)
```

示例：

```text
收到 20 字节
已经处理 12 字节
丢失 3 字节
缓冲区中剩余 5 字节
```

验证：

```text
20 = 12 + 3 + 5
```

这个关系可以发现数据是否无故丢失或重复处理。

---

# 十三、模拟 UART 接收

测试程序中不访问真实 UART 寄存器，而是使用字符串模拟：

```c
const char input[] = "TEMP=25\nSTATUS\n";
```

逐字节写入：

```c
for (int i = 0; input[i] != '\0'; i++)
{
    received_count++;

    if (!ring_buffer_write(&uart_rx_buffer,
                           (int)input[i]))
    {
        dropped_count++;
    }
}
```

这相当于 UART 依次收到每个字符。

---

# 十四、消费 UART 数据

```c
int value;

while (ring_buffer_read(&uart_rx_buffer, &value))
{
    char byte = (char)value;

    processed_count++;

    command_parser_feed(byte);
}
```

读取顺序必须与写入顺序一致。

---

# 十五、核心接口设计

```c
void uart_rx_init(void);
```

初始化 UART 接收状态。

```c
int uart_rx_receive_byte(char byte);
```

模拟接收一个字节。

返回：

```text
1：成功写入缓冲区
0：缓冲区已满
```

```c
int uart_rx_process_one(void);
```

从缓冲区读取并处理一个字节。

返回：

```text
1：成功处理一个字节
0：当前没有数据
```

```c
void command_parser_feed(char byte);
```

将一个字节交给命令解析器。

---

# 十六、程序安排

## uart_rx_sim.c

目标：

```text
模拟逐字节接收
写入环形缓冲区
主循环读取全部数据
验证接收顺序
```

示例输入：

```text
ABC\n
```

预期读取：

```text
A
B
C
换行符
```

---

## command_parser.c

目标：

```text
按字节组合命令
遇到换行符输出完整命令
忽略回车符
处理空命令
检测命令过长
```

示例输入：

```text
LED_ON\nSTATUS\n
```

预期：

```text
command: LED_ON
command: STATUS
```

---

## uart_rx_test.c

测试：

```text
初始化状态
单字节接收
多字节接收
FIFO 顺序
完整命令
连续多条命令
空命令
过长命令
缓冲区写满
数据丢失计数
统计关系
```

---

# 十七、复杂度

| 操作 | 时间复杂度 | 说明 |
|---|---:|---|
| 接收一个字节 | `O(1)` | 写入环形缓冲区 |
| 处理一个字节 | `O(1)` | 读取并更新解析状态 |
| 处理一条长度为 n 的命令 | `O(n)` | 每个字符处理一次 |
| 判断空或满 | `O(1)` | 检查 size |

缓冲区空间：

```text
O(RING_BUFFER_CAPACITY)
```

命令空间：

```text
O(COMMAND_CAPACITY)
```

两者都是固定容量，不使用动态内存。

---

# 十八、常见错误

## 1. 在中断中执行复杂解析

不推荐：

```text
UART 中断
→ 读取字节
→ 字符串比较
→ 执行命令
→ 打印日志
```

风险：

```text
中断执行时间过长
阻塞其他中断
增加实时延迟
丢失后续数据
```

推荐：

```text
中断只入队
主循环负责解析
```

---

## 2. 命令数组未预留结束符

错误：

```c
if (command_length < COMMAND_CAPACITY)
```

正确：

```c
if (command_length < COMMAND_CAPACITY - 1)
```

最后一个位置必须留给：

```c
'\0'
```

---

## 3. 忘记处理 `\r\n`

某些串口终端发送：

```text
\r\n
```

如果不忽略 `'\r'`，命令末尾可能多出一个回车字符。

处理：

```c
if (byte == '\r')
{
    return;
}
```

---

## 4. 缓冲区满后仍更新计数错误

收到一个字节时：

```text
received_count 一定增加
```

写入失败时：

```text
dropped_count 增加
buffer.size 不变
```

不能把丢失字节计入：

```text
processed_count
```

---

## 5. 消费速度长期小于接收速度

缓冲区只能延迟溢出，不能永久解决性能不足。

需要考虑：

```text
提高处理速度
增加缓冲区容量
使用 DMA
使用硬件流控
降低波特率
优化协议解析
```

---

# 十九、与真实 UART 驱动的对应关系

模拟代码：

```c
uart_rx_receive_byte(byte);
```

真实 MCU 中可能对应：

```text
UART RX 中断处理函数
HAL_UART_RxCpltCallback
USART 中断服务函数
DMA 接收完成回调
```

模拟主循环：

```c
while (uart_rx_process_one())
{
}
```

真实系统中可能对应：

```text
裸机 main() 主循环
FreeRTOS 串口处理任务
Linux 字符设备读取路径
协议解析线程
```

---

# 二十、核心总结

数据流：

```text
UART 接收
→ 环形缓冲区
→ 主循环读取
→ 命令解析
→ 命令执行
```

中断职责：

```text
读取字节
写入缓冲区
记录溢出
快速退出
```

主循环职责：

```text
读取字节
组合命令
解析命令
执行业务逻辑
```

统计关系：

```text
接收总数
=
已处理数量
+ 丢失数量
+ 缓冲区剩余数量
```

---

# 二十一、复习问题

1. 为什么 UART 接收适合使用环形缓冲区？
2. 生产者和消费者分别是谁？
3. 为什么中断中不适合执行复杂命令解析？
4. `'\n'` 在命令解析中有什么作用？
5. 为什么命令数组要为 `'\0'` 预留位置？
6. 为什么需要忽略 `'\r'`？
7. 缓冲区写满后本实验采用什么策略？
8. `received_count`、`processed_count` 和 `dropped_count` 分别表示什么？
9. 如何检查是否有数据无故消失？
10. 为什么环形缓冲区不能解决长期处理速度不足？
11. UART、SPI 和 I²C 使用队列时有什么区别？
12. 实际驱动为什么可能使用 DMA 和硬件流控？

---

# 二十二、完成情况

- [ ] 完成 `uart_rx_sim.c`
- [ ] 完成 `command_parser.c`
- [ ] 完成 `uart_rx_test.c`
- [ ] 使用 Day 04 环形缓冲区模块
- [ ] 没有重复实现环形缓冲区
- [ ] FIFO 顺序正确
- [ ] 能识别完整命令
- [ ] 能处理过长命令
- [ ] 能统计丢失字节
- [ ] 编译无警告
- [ ] AddressSanitizer 无报错
- [ ] 所有测试通过

---

# 二十三、Git 提交

## 1. 查看改动

```bash
cd ~/projects/embedded-algorithm-learning

git status
git diff
```

## 2. 添加 Day 05 文件

```bash
git add week02/day05
```

如果 Day 05 同时引用并修改了 Day 04 的公共模块：

```bash
git add week02/day04 week02/day05
```

## 3. 检查暂存内容

```bash
git diff --cached
```

确认提交：

```text
uart_rx_sim.c
command_parser.c
uart_rx_test.c
notes.md
```

确认没有提交：

```text
build 目录
可执行文件
目标文件
临时输出文件
```

## 4. 提交

```bash
git commit -m "feat: complete week02 day05 uart ring buffer exercises"
```

## 5. 推送

```bash
git push
```

## 6. 最终检查

```bash
git status
git log --oneline -5
```

预期：

```text
nothing to commit, working tree clean
```

项目根目录的 `.gitignore` 建议包含：

```gitignore
build/
*.o
*.out
```

---

# 二十四、下一步

代码学习顺序：

```text
1. uart_rx_sim.c
2. command_parser.c
3. uart_rx_test.c
```

