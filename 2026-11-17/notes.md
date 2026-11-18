# Week 02 / Day 04：模块化环形缓冲区

## 1. 今日目标

- [ ] 使用结构体封装环形缓冲区状态
- [ ] 理解结构体变量、结构体指针和成员访问
- [ ] 使用 `.h` 与 `.c` 分离接口和实现
- [ ] 实现初始化、写入、读取和查看
- [ ] 编写演示程序观察回绕过程
- [ ] 编写测试程序验证边界情况
- [ ] 理解代码复用和模块化

---

## 2. 工程文件

```text
week02/day04/
├── ring_buffer.h
├── ring_buffer.c
├── ring_buffer_demo.c
├── ring_buffer_test.c
└── notes.md
```

文件职责：

```text
ring_buffer.h
定义结构体、容量和函数接口

ring_buffer.c
实现环形缓冲区功能

ring_buffer_demo.c
调用公共模块，观察运行过程

ring_buffer_test.c
调用公共模块，自动检查正确性
```

核心原则：

```text
环形缓冲区功能只实现一次
demo 和 test 只调用，不复制实现
```

---

# 一、结构体封装

## 1. 原来的分散状态

Day 03 使用多个独立变量：

```c
int data[RING_BUFFER_CAPACITY];
int read_index;
int write_index;
int size;
```

这些变量共同表示一个环形缓冲区，应封装到一个结构体中。

---

## 2. RingBuffer 结构体

```c
typedef struct
{
    int data[RING_BUFFER_CAPACITY];
    int read_index;
    int write_index;
    int size;
} RingBuffer;
```

成员职责：

```text
data
保存物理数据

read_index
下一次读取的位置

write_index
下一次写入的位置

size
当前有效元素数量
```

状态必须满足：

```text
0 <= read_index < RING_BUFFER_CAPACITY
0 <= write_index < RING_BUFFER_CAPACITY
0 <= size <= RING_BUFFER_CAPACITY
```

---

## 3. 创建结构体变量

```c
RingBuffer rb;
```

现在一个 `rb` 就包含环形缓冲区的全部状态：

```text
rb.data
rb.read_index
rb.write_index
rb.size
```

还可以创建多个独立实例：

```c
RingBuffer uart_rx;
RingBuffer uart_tx;
RingBuffer adc_samples;
```

三个实例互不影响。

---

# 二、`.`、`&` 和 `->`

## 1. 结构体变量使用 `.`

```c
RingBuffer rb;

rb.size = 0;
rb.read_index = 0;
```

---

## 2. 获取结构体地址

```c
&rb
```

表示：

```text
结构体变量 rb 的地址
```

调用需要修改结构体的函数时传入：

```c
ring_buffer_init(&rb);
ring_buffer_write(&rb, 10);
```

---

## 3. 结构体指针使用 `->`

函数参数：

```c
RingBuffer *rb
```

表示 `rb` 是指向 `RingBuffer` 的指针。

访问成员：

```c
rb->size
rb->read_index
rb->data[0]
```

下面两种写法等价：

```c
rb->size
```

```c
(*rb).size
```

记忆：

```text
结构体变量：使用 .
结构体指针：使用 ->
```

---

## 4. 本模块中合法的指针形式

```c
RingBuffer *rb
```

允许函数修改原环形缓冲区。

```c
const *ingBuffer *rb
```

函数只能读取环形缓冲区。

`*`c
int *out_value
```

函数通过这个指针返回读*的数据。

```c
*out_value = rb->data[rb->read_index];
```

将读取结果写入调用者的变量。*
---

# 三、头文件 ring_buffer.h

头文件保存*

```text
容量定义
结构体定义
函数声明
```

完整内*：

```c
#ifndef RING_BUFFER_H
#def*ne RING_BUFFER_H

#define RING_BUF*ER*CAPACITY 5

typedef struct
{
    i*t data[RING_BUFFER_CAPACITY];
    *nt read_index;
    int write_index*
    int size;
} RingBuffer;

void*ring_buffer_init(RingBuffer *rb);
*int ring_buffer_is_empty(const Rin*Buffer *rb);

int ring_buffer_is_f*ll(const RingBuffer *rb);

int rin*_buffer_size(const RingBuffer *rb)*

int ring_buffer_write(RingBuffer**rb, int value);

int ring_buffer_*ead(RingBuffer *rb,
              *      int *out_value);

int ring_b*ffer_peek(const RingBuffer *rb,
  *                  int *out_value);*
#endif
```

---

## 1. 头文件保护

```*
#ifndef RING_BUFFER_H
#define RIN*_BUFFER_H

/* 头文件内容 */

#endif
```*
作用：

```text
防止同一个头文件被重复包含
避免结构体和*数声明重复定义
```

---

# 四、实现文件 ring_bu*fer.c

```c
#include "ring_buffer.*"
```

使用双引号表示包含项目自己的头文件。

---

##*1. 初始化

```c
void ring_buffer_init*RingBuffer *rb)
{
    rb->read_ind*x = 0;
    rb->write_index = 0;
  * rb->size = 0;
}
```

初始化后：

```te*t
read_index = 0
write_index = 0
s*ze = 0
缓冲区为空
```

不必清零整个数组。

原因：

*``text
有效数据由 read_index 和 size 决定
*缓冲区不会读取数组中的旧值
```

---

## 2. 判断为空*
```c
int ring_buffer_is_empty(con*t RingBuffer *rb)
{
    return rb-*size == 0;
}
```

返回：

```text
1：为*
0：非空
```

---

## 3. 判断已满

```c
i*t ring_buffer_is_full(const RingBu*fer *rb)
{
    return rb->size >= *ING_BUFFER_CAPACITY;
}
```

返回：

`*`text
1：已满
0：未满
```

---

## 4. 获取*前大小

```c
int ring_buffer_size(con*t RingBuffer *rb)
{
    return rb-*size;
}
```

---

## 5. 写入数据

```c*int ring_buffer_write(RingBuffer **b, int value)
{
    if (ring_buffe*_is_full(rb))
    {
        return*0;
    }

    rb->data[rb->write_i*dex] = value;

    rb->write_index*=
        (rb->write_index + 1) % *ING_BUFFER_CAPACITY;

    rb->size*+;

    return 1;
}
```

执行顺序：

``*text
检查是否已满
→ 写入 data[write_index]*→ 更新 write_index
→ size 加 1
```

写*失败时不能修改：

```text
data
write_index*size
```

---

## 6. 读取数据

```c
in* ring_buffer_read(RingBuffer *rb,
*                    int *out_value*
{
    if (ring_buffer_is_empty(rb*)
    {
        return 0;
    }

 *  *out_value = rb->data[rb->read_i*dex];

    rb->read_index =
      * (rb->read_index + 1) % RING_BUFFE*_CAPACITY;

    rb->size--;

    r*turn 1;
}
```

执行顺序：

```text
检查是否*空
→ 读取 data[read_index]
→ 更新 read_*ndex
→ size 减 1
```

读取失败时不能修改：

`*`text
read_index
size
out_value 指向*变量
```

---

## 7. 查看队首

```c
int *ing_buffer_peek(const RingBuffer **b,
                     int *out_v*lue)
{
    if (ring_buffer_is_empt*(rb))
    {
        return 0;
    *

    *out_value = rb->data[rb->re*d_index];

    return 1;
}
```

`p*ek` 与 `read` 的区别：

| 操作 | 返回数据 | 更*读下标 | 修改 size |
|---|---:|---:|---*|
| `peek` | 是 | 否 | 否 |
| `read` * 是 | 是 | 减 1 |

---

## 8. ring_bu*fer.c 完整内容

```c
#include "ring_bu*fer.h"

void ring_buffer_init(Ring*uffer *rb)
{
    rb->read_index = *;
    rb->write_index = 0;
    rb-*size = 0;
}

int ring_buffer_is_em*ty(const RingBuffer *rb)
{
    ret*rn rb->size == 0;
}

int ring_buff*r_is_full(const RingBuffer *rb)
{
*   return rb->size >= RING_BUFFER_*APACITY;
}

int ring_buffer_size(c*nst RingBuffer *rb)
{
    return r*->size;
}

int ring_buffer_write(R*ngBuffer *rb, int value)
{
    if *ring_buffer_is_full(rb))
    {
   *    return 0;
    }

    rb->data[*b->write_index] = value;

    rb->*rite_index =
        (rb->write_in*ex + 1) % RING_BUFFER_CAPACITY;

 *  rb->size++;

    return 1;
}

in* ring_buffer_read(RingBuffer *rb,
*                    int *out_value*
{
    if (ring_buffer_is_empty(rb*)
    {
        return 0;
    }

 *  *out_value = rb->data[rb->read_i*dex];

    rb->read_index =
      * (rb->read_index + 1) % RING_BUFFE*_CAPACITY;

    rb->size--;

    r*turn 1;
}

int ring_buffer_peek(co*st RingBuffer *rb,
               *     int *out_value)
{
    if (rin*_buffer_is_empty(rb))
    {
      * return 0;
    }

    *out_value =*rb->data[rb->read_index];

    ret*rn 1;
}
```

---

# 五、状态变化

## 1. *始状态

```text
read_index = 0
write_*ndex = 0
size = 0
```

---

## 2. *入 10、20、30、40、50

物理数组：

```text
1* 20 30 40 50
```

状态：

```text
rea*_index = 0
write_index = 0
size = *
```

此时：

```text
read_index == w*ite_index
size == capacity
```

表示*冲区已满。

---

## 3. 读取 10、20

状态：

`*`text
read_index = 2
write_index =*0
size = 3
```

物理数组仍然可能是：

```tex*
10 20 30 40 50
```

逻辑有效数据：

```t*xt
30 40 50
```

---

## 4. 再写入 60*70

物理数组：

```text
60 70 30 40 50
*``

状态：

```text
read_index = 2
wr*te_index = 2
size = 5
```

逻辑读取顺序：*
```text
30 40 50 60 70
```

---

* 六、物理顺序与逻辑顺序

物理数组顺序：

```text
按 d*ta[0] 到 data[capacity - 1] 排列
```
*逻辑顺序：

```text
从 read_index 开始
读取 *ize 个有效元素
到达末尾后回到下标 0
```

逻辑位置计算：*
```c
int index =
    (rb->read_in*ex + i) % RING_BUFFER_CAPACITY;
``*

例如：

```text
物理数组：60 70 30 40 50*read_index = 2
size = 5
```

访问下标：*
```text
2 → 3 → 4 → 0 → 1
```

逻辑*序：

```text
30 40 50 60 70
```

--*

# 七、demo 与 test 的区别

## ring_buf*er_demo.c

作用：

```text
调用公共模块
打印每*步状态
帮助观察下标回绕
```

不重新实现：

```text
*ing_buffer_write
ring_buffer_read
*ing_buffer_peek
```

---

## ring_*uffer_test.c

作用：

```text
调用公共模块
*较实际结果和预期结果
自动输出 PASS 或 FAIL
```

重*测试：

```text
初始化
空读
空 peek
正常写入
写满*满后继续写
正常读取
下标回绕
FIFO 顺序
失败后状态保持不变
*``

同样不重复实现核心功能。

---

# 八、编译方式

#* 编译 demo

```bash
mkdir -p build

*cc -std=c11 -Wall -Wextra -Wpedant*c -g \
    -fsanitize=address \
  * -fno-omit-frame-pointer \
    rin*_buffer.c \
    ring_buffer_demo.c*\
    -o build/ring_buffer_demo
``*

运行：

```bash
./build/ring_buffer*demo
```

---

## 编译 test

```bash*gcc -std=c11 -Wall -Wextra -Wpedan*ic -g \
    -fsanitize=address \
 *  -fno-omit-frame-pointer \
    ri*g_buffer.c \
    ring_buffer_test.* \
    -o build/ring_buffer_test
`*`

运行：

```bash
./build/ring_buffe*_test
```

注意：

```text
ring_buffe*.c 没有 main()
不能单独编译成可执行程序
```

它必须* `demo.c` 或 `test.c` 一起参与链接。

---
*# 九、复杂度

| 操作 | 时间复杂度 | 额外空间 |
|--*|---:|---:|
| 初始化 | `O(1)` | `O(1)* |
| 写入 | `O(1)` | `O(1)` |
| 读取 |*`O(1)` | `O(1)` |
| 查看 | `O(1)` | *O(1)` |
| 判断为空 | `O(1)` | `O(1)` |*| 判断已满 | `O(1)` | `O(1)` |

结构体中的固*数组占用：

```text
O(n)
```

这里的 `n` 是*冲区容量。

---

# 十、常见错误

## 1. 忘记传入结构*地址

错误：

```c
ring_buffer_init(rb)*
```

正确：

```c
ring_buffer_init(&*b);
```

---

## 2. 混淆 `.` 和 `->`
*结构体变量：

```c
rb.size
```

结构体指针：

*``c
rb->size
```

---

## 3. 先更新下标*访问数据

错误写入：

```c
rb->write_index *
    (rb->write_index + 1) % RING_*UFFER_CAPACITY;

rb->data[rb->writ*_index] = value;
```

正确顺序：

```te*t
先写当前位置
再更新写下标
```

错误读取：

```c
r*->read_index =
    (rb->read_index*+ 1) % RING_BUFFER_CAPACITY;

*out*value = rb->data[rb->read_index];
*``

正确顺序：

```text
先读取当前位置
再更新读下标
*``

---

## 4. 忘记取模

错误：

```c
rb-*write_index++;
```

到达数组末尾后会变成非法下标*

正确：

```c
rb->write_index =
    *rb->write_index + 1) % RING_BUFFER*CAPACITY;
```

---

## 5. 只通过下标判断空*满

下面两种状态都可能出现：

```text
read_inde* == write_index
```

空状态：

```text*size == 0
```

满状态：

```text
size *= RING_BUFFER_CAPACITY
```

因此必须结合*`size` 判断。

---

## 6. 失败操作仍修改状态

*冲区满时，写入失败不能修改：

```text
data
write*index
size
```

缓冲区空时，读取失败不能修改：

`*`text
read_index
size
输出变量
```

--*

## 7. 在多个文件中重复实现函数

错误组织：

```te*t
ring_buffer.c 实现一次
demo.c 再实现一次
*est.c 再实现一次
```

问题：

```text
代码重复*修改时容易不一致
测试可能没有测试正式实现
```

正确组织：

*``text
ring_buffer.c 只实现一次
demo.c * test.c 调用公共模块
```

---

# 十一、嵌入式应*

结构体版环形缓冲区可以用于：

```text
UART 接收字*
UART 待发送数据
ADC 采样结果
按键和编码器事件
日志消息*通信数据帧
```

示例：

```c
RingBuffer ua*t_rx;
RingBuffer uart_tx;

ring_bu*fer_init(&uart_rx);
ring_buffer_in*t(&uart_tx);
```

UART 接收中断：

```c*ring_buffer_write(&uart_rx, receiv*d_byte);
```

主循环：

```c
int byte;*
if (ring_buffer_read(&uart_rx, &b*te))
{
    parse_byte(byte);
}
```*
实际驱动还需要考虑：

```text
中断与主循环并发
临界区
*或原子操作
内存屏障
溢出计数
错误处理
```

---

# 十*、核心总结

结构体定义：

```c
typedef struct*{
    int data[RING_BUFFER_CAPACITY];
    int read_index;
    int wri*e_index;
    int size;
} RingBuffe*;
```

调用方法：

```c
RingBuffer rb;
*nt value;

ring_buffer_init(&rb);
*ing_buffer_write(&rb, 10);
ring_bu*fer_read(&rb, &value);
```

关键状态：

```text
read_index
下一次读取位置

write_index
下一次写入位置

size
当前有效元素数量
```

回绕公式：

```c
index = (index + 1) % RING_BUFFER_CAPACITY;
```

模块化结构：

```text
.h：接口
.c：实现
demo.c：演示
test.c：测试
```

---

# 十三、复习问题

1. 为什么要使用结构体封装环形缓冲区？
2. `RingBuffer rb` 和 `RingBuffer *rb` 有什么区别？
3. `.` 和 `->` 分别在什么情况下使用？
4. 为什么函数调用时传入 `&rb`？
5. `int *out_value` 的作用是什么？
6. 为什么 `peek` 使用 `const RingBuffer *`？
7. 为什么写入时先写数据再更新下标？
8. 为什么读取时先读数据再更新下标？
9. 为什么必须使用取模运算？
10. 为什么物理数组顺序不等于逻辑顺序？
11. 为什么空和满不能只通过下标判断？
12. 为什么核心函数只应实现一次？
13. `demo.c` 和 `test.c` 的职责有什么区别？
14. 为什么 `ring_buffer.c` 不能单独链接为可执行文件？

---

# 十四、完成情况

- [ ] 创建 `ring_buffer.h`
- [ ] 完成 `ring_buffer.c`
- [ ] 完成 `ring_buffer_demo.c`
- [ ] 完成 `ring_buffer_test.c`
- [ ] 理解结构体和结构体指针
- [ ] 理解 `.h` 与 `.c` 的职责
- [ ] 读写顺序符合 FIFO
- [ ] 读写下标能够回绕
- [ ] 所有测试通过
- [ ] 编译无警告
- [ ] AddressSanitizer 无报错

---

# 十五、Git 提交

## 1. 查看改动

```bash
cd ~/projects/embedded-algorithm-learning

git status
git diff
```

## 2. 添加 Day 04 文件

```bash
git add week02/day04
```

## 3. 检查暂存内容

```bash
git diff --cached
```

确认提交内容包括：

```text
ring_buffer.h
ring_buffer.c
ring_buffer_demo.c
ring_buffer_test.c
notes.md
```

确认没有提交：

```text
build 目录
可执行文件
目标文件
临时文件
```

## 4. 提交

```bash
git commit -m "feat: complete week02 day04 ring buffer module"
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

# 十六、下一步

下一阶段不再重复实现环形缓冲区。

直接使用当前模块完成实际应用：

```text
UART 接收模拟
字符流缓存
按行提取命令
缓冲区溢出统计
```