# Week 02 / Day 01：固定数组栈

## 1. 今日学习目标

- [ ] 理解栈的后进先出特性
- [ ] 理解栈顶和栈底
- [ ] 使用固定数组保存栈元素
- [ ] 使用 `size` 记录栈中元素数量
- [ ] 实现压栈 `push`
- [ ] 实现出栈 `pop`
- [ ] 实现查看栈顶 `peek`
- [ ] 正确处理栈满和栈空
- [ ] 使用指针返回出栈结果
- [ ] 为栈设计边界测试
- [ ] 分析栈操作的时间复杂度

---

## 2. 今日程序

```text
stack_basic.c
stack_peek.c
stack_test.c
```

学习顺序：

```text
1. 学习栈的基本概念
2. 手动推演栈的状态变化
3. 编写 stack_basic.c
4. 学习 peek 与 pop 的区别
5. 编写 stack_peek.c
6. 学习栈的边界测试
7. 编写 stack_test.c
```

---

# 一、栈的基本概念

## 1. 什么是栈

栈是一种只能从同一端添加和删除元素的数据结构。

这个可以操作的一端称为：

```text
栈顶 top
```

另一端称为：

```text
栈底 bottom
```

栈遵循：

```text
后进先出
Last In, First Out
LIFO
```

最后进入栈中的元素，会最先离开栈。

---

## 2. 生活中的直觉理解

可以把栈想象成一摞盘子：

```text
最后放上去的盘子
最先被拿走
```

不能直接取出中间的盘子，否则需要先移开上面的盘子。

栈只允许操作栈顶。

---

## 3. 栈的基本操作

### 压栈 `push`

向栈顶添加一个元素。

```text
push(10)
push(20)
push(30)
```

结果：

```text
栈顶
 30
 20
 10
栈底
```

---

### 出栈 `pop`

读取并删除栈顶元素。

对上面的栈执行：

```text
pop()
```

取出：

```text
30
```

剩余：

```text
栈顶
 20
 10
栈底
```

---

### 查看栈顶 `peek`

读取栈顶元素，但不删除。

```text
peek()
```

得到：

```text
20
```

执行后栈的内容不变。

---

### 判断空栈

```text
size == 0
```

空栈中不能执行正常的 `pop` 或 `peek`。

---

### 判断满栈

```text
size == capacity
```

满栈中不能继续执行 `push`。

---

# 二、使用数组实现栈

## 1. 栈的存储结构

定义固定长度数组：

```c
#define STACK_CAPACITY 5

int stack[STACK_CAPACITY];
int size = 0;
```

含义：

```text
STACK_CAPACITY  栈的最大容量
stack           保存栈元素的数组
size            当前栈中元素数量
```

必须始终满足：

```text
0 <= size <= STACK_CAPACITY
```

---

## 2. `capacity` 和 `size`

### `capacity`

表示最多可以保存多少个元素。

```text
capacity = 5
```

### `size`

表示当前已经保存了多少个有效元素。

```text
size = 3
```

例如：

```text
下标：  0   1   2   3   4
数据： 10  20  30   ?   ?
状态：有效 有效 有效 空闲 空闲
```

此时：

```text
capacity = 5
size = 3
```

---

## 3. 栈顶位于哪里

当：

```text
size = 3
```

有效下标为：

```text
0、1、2
```

所以栈顶元素位于：

```c
stack[size - 1]
```

即：

```c
stack[2]
```

栈顶数据是：

```text
30
```

---

## 4. 下一次压栈位置

当：

```text
size = 3
```

下一次写入位置是：

```c
stack[size]
```

即：

```c
stack[3]
```

所以：

```text
栈顶元素位置：stack[size - 1]
下一次写入位置：stack[size]
```

注意这两个位置不同。

---

# 三、压栈操作 push

## 1. 函数接口

```c
int stack_push(int stack[],
               int capacity,
               int *size,
               int value);
```

参数含义：

```text
stack     栈数组
capacity  栈的最大容量
size      当前元素数量的地址
value     要压入栈中的数据
```

返回值：

```text
1：压栈成功
0：压栈失败
```

---

## 2. 压栈步骤

```text
1. 检查栈是否已满
2. 已满时返回失败
3. 将数据写入 stack[*size]
4. 让 size 加 1
5. 返回成功
```

伪代码：

```text
函数 stack_push(stack, capacity, size, value)

    如果 size 大于或等于 capacity：
        返回失败

    将 value 写入 stack[size]
    size 加 1

    返回成功
```

---

## 3. 压栈核心代码

```c
if (*size >= capacity)
{
    return 0;
}

stack[*size] = value;
(*size)++;

return 1;
```

---

## 4. 为什么先检查栈满

假设：

```text
capacity = 5
size = 5
```

此时有效下标为：

```text
0、1、2、3、4
```

如果继续执行：

```c
stack[*size] = value;
```

相当于：

```c
stack[5] = value;
```

这会造成数组越界。

所以压栈前必须检查：

```c
*size >= capacity
```

---

## 5. 为什么先写入，再增加 `size`

正确顺序：

```c
stack[*size] = value;
(*size)++;
```

假设：

```text
size = 3
```

执行过程：

```text
写入 stack[3]
size 从 3 变为 4
```

此时新栈顶位于：

```text
stack[4 - 1]
stack[3]
```

错误顺序：

```c
(*size)++;
stack[*size] = value;
```

执行后会跳过 `stack[3]`，错误写入 `stack[4]`。

---

# 四、出栈操作 pop

## 1. 函数接口

```c
int stack_pop(int stack[],
              int *size,
              int *value);
```

参数含义：

```text
stack  栈数组
size   当前元素数量的地址
value  用于返回出栈数据
```

返回值：

```text
1：出栈成功
0：出栈失败
```

---

## 2. 为什么需要 `value` 指针

函数返回值已经用于表示操作成功或失败：

```text
1：成功
0：失败
```

所以出栈的数据需要通过指针参数返回：

```c
int *value
```

调用：

```c
int result;
int success = stack_pop(stack, &size, &result);
```

函数内部：

```c
*value = stack[*size - 1];
```

执行后，出栈数据会保存到：

```c
result
```

---

## 3. 出栈步骤

```text
1. 判断栈是否为空
2. 空栈时返回失败
3. 读取当前栈顶元素
4. 让 size 减 1
5. 返回成功
```

伪代码：

```text
函数 stack_pop(stack, size, value)

    如果 size 等于 0：
        返回失败

    value 等于 stack[size - 1]
    size 减 1

    返回成功
```

---

## 4. 出栈核心代码

```c
if (*size <= 0)
{
    return 0;
}

*value = stack[*size - 1];
(*size)--;

return 1;
```

---

## 5. 为什么先读取，再减少 `size`

假设：

```text
size = 3
```

栈中元素：

```text
10 20 30
```

栈顶位置：

```text
size - 1 = 2
```

正确顺序：

```c
*value = stack[*size - 1];
(*size)--;
```

过程：

```text
读取 stack[2]，得到 30
size 从 3 变为 2
```

也可以先减少 `size`，再使用新的 `size` 读取：

```c
(*size)--;
*value = stack[*size];
```

但初学阶段统一使用：

```text
先读取 stack[size - 1]
再让 size 减 1
```

这样更容易直接理解栈顶位置。

---

## 6. 是否需要清零原栈顶

出栈前：

```text
数组物理内容：10 20 30 ? ?
size = 3
```

出栈后：

```text
数组物理内容：10 20 30 ? ?
size = 2
```

虽然数组中仍然保存着 `30`，但它已经不属于有效栈内容。

有效范围是：

```text
stack[0] 到 stack[size - 1]
```

因此当前有效数据只有：

```text
10 20
```

不需要清零原栈顶。

如果为了观察方便，可以写成：

```c
*value = stack[*size - 1];
stack[*size - 1] = 0;
(*size)--;
```

但清零不是栈逻辑正确性的必要条件。

---

# 五、查看栈顶 peek

## 1. 函数接口

```c
int stack_peek(const int stack[],
               int size,
               int *value);
```

参数含义：

```text
stack  栈数组，只读取
size   当前元素数量
value  用于返回栈顶数据
```

返回值：

```text
1：读取成功
0：空栈，读取失败
```

---

## 2. 核心代码

```c
if (size <= 0)
{
    return 0;
}

*value = stack[size - 1];

return 1;
```

---

## 3. `pop` 与 `peek` 的区别

| 操作 | 读取栈顶 | 删除栈顶 | 修改 `size` |
|---|---:|---:|---:|
| `pop` | 是 | 是 | `size--` |
| `peek` | 是 | 否 | 不修改 |

例如：

```text
栈内容：10 20 30
size = 3
```

执行 `peek`：

```text
返回 30
栈仍然是 10 20 30
size 仍为 3
```

执行 `pop`：

```text
返回 30
栈变成 10 20
size 变为 2
```

---

## 4. 为什么 `peek` 的数组使用 `const`

```c
const int stack[]
```

表示函数只读取数组，不允许修改数组元素。

`peek` 不应该改变：

```text
栈中的数据
size
```

因此使用 `const` 更符合函数职责。

---

# 六、手动推演

## 1. 初始状态

```text
capacity = 5
size = 0
stack = 空
```

---

## 2. 压入 10

```c
stack_push(stack, 5, &size, 10);
```

结果：

```text
stack[0] = 10
size = 1
```

栈：

```text
栈顶
10
栈底
```

---

## 3. 压入 20

```text
stack[1] = 20
size = 2
```

栈：

```text
栈顶
20
10
栈底
```

---

## 4. 压入 30

```text
stack[2] = 30
size = 3
```

栈：

```text
栈顶
30
20
10
栈底
```

---

## 5. 查看栈顶

```c
stack_peek(stack, size, &value);
```

结果：

```text
value = 30
size = 3
```

栈没有变化。

---

## 6. 执行出栈

```c
stack_pop(stack, &size, &value);
```

结果：

```text
value = 30
size = 2
```

剩余有效元素：

```text
10 20
```

---

## 7. 再次出栈

```text
value = 20
size = 1
```

---

## 8. 再次出栈

```text
value = 10
size = 0
```

此时栈为空。

这证明栈的顺序是：

```text
压栈：10 → 20 → 30
出栈：30 → 20 → 10
```

即：

```text
后进先出
```

---

# 七、栈的边界条件

## 1. 空栈

判断：

```c
size == 0
```

空栈不能执行：

```text
pop
peek
```

否则可能访问：

```c
stack[-1]
```

因为：

```text
size - 1 = -1
```

---

## 2. 满栈

判断：

```c
size == capacity
```

满栈不能继续执行：

```text
push
```

否则可能访问：

```c
stack[capacity]
```

这已经超过最后一个合法下标：

```text
capacity - 1
```

---

## 3. 栈状态必须保持一致

必须始终满足：

```text
0 <= size <= capacity
```

压栈成功：

```text
size 增加 1
```

压栈失败：

```text
size 不变
原栈内容不变
```

出栈成功：

```text
size 减少 1
返回原栈顶数据
```

出栈失败：

```text
size 不变
输出变量不应被错误修改
```

查看成功：

```text
返回栈顶数据
size 不变
```

查看失败：

```text
size 不变
```

---

# 八、三个实验安排

## 实验一：stack_basic.c

实现：

```c
stack_push()
stack_pop()
stack_print()
```

测试过程：

```text
压入 10
压入 20
压入 30
出栈一次
打印剩余栈内容
```

预期：

```text
push 10: success
push 20: success
push 30: success
stack: 10 20 30
pop: 30
stack: 10 20
```

---

## 实验二：stack_peek.c

在基础栈上增加：

```c
stack_peek()
```

测试：

```text
查看非空栈的栈顶
确认 size 不变
出栈一次
再次查看栈顶
查看空栈
```

---

## 实验三：stack_test.c

设计自动测试：

```text
空栈出栈
空栈查看
正常压栈
检查栈顶
压栈到满
满栈继续压栈
正常出栈
检查后进先出顺序
连续出栈到空
空栈再次出栈
```

每项操作需要检查：

```text
返回值
size
栈顶数据
弹出的数据
失败后状态是否保持不变
```

---

# 九、建议的函数接口

```c
int stack_push(int stack[],
               int capacity,
               int *size,
               int value);
```

```c
int stack_pop(int stack[],
              int *size,
              int *value);
```

```c
int stack_peek(const int stack[],
               int size,
               int *value);
```

```c
void stack_print(const int stack[],
                 int size);
```

---

# 十、复杂度分析

| 操作 | 时间复杂度 | 空间复杂度 |
|---|---:|---:|
| `push` | `O(1)` | `O(1)` |
| `pop` | `O(1)` | `O(1)` |
| `peek` | `O(1)` | `O(1)` |
| `is_empty` | `O(1)` | `O(1)` |
| `is_full` | `O(1)` | `O(1)` |
| `print` | `O(n)` | `O(1)` |

---

## 1. 为什么 push 是 O(1)

压栈只执行：

```text
一次满栈判断
一次数组写入
一次 size 更新
```

操作数量不会随着栈中元素数量增加。

所以：

```text
时间复杂度：O(1)
```

---

## 2. 为什么 pop 是 O(1)

出栈只执行：

```text
一次空栈判断
一次栈顶读取
一次 size 更新
```

不需要移动其他元素。

所以：

```text
时间复杂度：O(1)
```

---

## 3. 为什么栈比固定缓冲区删除更快

固定缓冲区删除中间元素需要移动后续数据：

```text
时间复杂度：O(n)
```

栈只允许删除最后一个有效元素：

```text
stack[size - 1]
```

不需要移动其他元素：

```text
时间复杂度：O(1)
```

限制操作位置，可以换来更高的操作效率。

---

# 十一、常见错误

## 1. 满栈后继续写入

错误：

```c
stack[*size] = value;
```

没有先判断：

```c
*size >= capacity
```

可能造成数组越界。

---

## 2. 空栈时访问栈顶

错误：

```c
*value = stack[*size - 1];
```

当：

```text
size = 0
```

会访问：

```c
stack[-1]
```

正确做法：

```c
if (*size <= 0)
{
    return 0;
}
```

---

## 3. 混淆栈顶位置和下一写入位置

```text
栈顶位置：stack[size - 1]
写入位置：stack[size]
```

只有在栈未满时，`stack[size]` 才是合法的下一写入位置。

---

## 4. 写成 `*size++`

错误：

```c
*size++;
```

它会移动指针。

正确：

```c
(*size)++;
```

出栈时：

```c
(*size)--;
```

---

## 5. 出栈时移动所有元素

不需要这样做：

```c
for (...)
{
    stack[i] = stack[i + 1];
}
```

栈只从数组末端删除数据。

出栈只需要：

```c
*value = stack[*size - 1];
(*size)--;
```

---

## 6. `peek` 修改了 size

错误：

```c
*value = stack[size - 1];
size--;
```

`peek` 只查看栈顶，不能删除元素。

正确：

```c
*value = stack[size - 1];
return 1;
```

---

# 十二、嵌入式中的栈

栈可以用于：

```text
函数调用
中断现场保存
表达式计算
括号匹配
状态回退
深度优先搜索
撤销操作
```

---

## 1. 函数调用栈

程序调用函数时，系统通常需要保存：

```text
返回地址
局部变量
函数参数
寄存器状态
```

函数返回时，最近调用的函数最先结束。

这符合：

```text
后进先出
```

例如：

```text
main 调用 function_a
function_a 调用 function_b
function_b 最先返回
function_a 随后返回
main 最后继续执行
```

---

## 2. 中断嵌套

嵌入式系统发生中断时，需要保存当前执行状态。

如果高优先级中断再次打断低优先级中断：

```text
主程序
→ 低优先级中断
→ 高优先级中断
```

恢复顺序是：

```text
高优先级中断返回
→ 低优先级中断继续
→ 主程序继续
```

这个顺序同样具有栈的特性。

实际 MCU 的栈通常由处理器栈指针和内存共同管理，不是本实验中的简单整数数组，但基本思想一致。

---

## 3. 栈溢出风险

固定数组栈容量有限。

如果不断压栈且不检查容量，可能发生：

```text
stack overflow
```

在嵌入式系统中，调用层级过深、递归过多或局部变量过大，也可能造成系统栈溢出。

可能后果：

```text
覆盖其他内存
破坏返回地址
HardFault
程序复位
不可预测行为
```

因此必须明确栈容量，并在软件栈中检查满栈状态。

---

# 十三、编译要求

编译时继续开启警告和 AddressSanitizer：

```bash
mkdir -p build
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    stack_basic.c \
    -o build/stack_basic
```

运行：

```bash
./build/stack_basic
```

其他程序使用相同方式：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    stack_peek.c \
    -o build/stack_peek
```

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    stack_test.c \
    -o build/stack_test
```

检查：

```text
没有编译错误
没有编译警告
没有 AddressSanitizer 报错
边界测试全部通过
```

---

# 十四、今日核心总结

## 1. 栈的规则

```text
后进先出
只操作栈顶
```

## 2. 栈的状态

```text
capacity：栈的最大容量
size：当前元素数量
栈顶：stack[size - 1]
下一写入位置：stack[size]
空栈：size == 0
满栈：size == capacity
```

## 3. 压栈

```c
if (*size >= capacity)
{
    return 0;
}

stack[*size] = value;
(*size)++;

return 1;
```

## 4. 出栈

```c
if (*size <= 0)
{
    return 0;
}

*value = stack[*size - 1];
(*size)--;

return 1;
```

## 5. 查看栈顶

```c
if (size <= 0)
{
    return 0;
}

*value = stack[size - 1];

return 1;
```

## 6. 操作复杂度

```text
push：O(1)
pop： O(1)
peek：O(1)
```

---

# 十五、复习问题

学习完成后应能回答：

1. 什么是后进先出？
2. 栈顶和栈底分别是什么？
3. `capacity` 和 `size` 有什么区别？
4. 为什么栈顶位于 `stack[size - 1]`？
5. 为什么下一次写入位置是 `stack[size]`？
6. 如何判断空栈？
7. 如何判断满栈？
8. 满栈继续压栈会发生什么？
9. 空栈执行出栈会访问哪个错误下标？
10. 为什么 `stack_pop()` 需要 `int *value`？
11. `pop` 和 `peek` 有什么区别？
12. 为什么出栈后不需要移动其他元素？
13. 为什么出栈后原位置不清零也可以？
14. 为什么 `push`、`pop` 和 `peek` 都是 `O(1)`？
15. 为什么栈适合保存函数调用状态？
16. 嵌入式系统发生栈溢出可能有什么后果？

---

# 十六、Day 01 完成情况

## 概念学习

- [ ] 理解栈的后进先出
- [ ] 理解栈顶和栈底
- [ ] 理解栈顶下标
- [ ] 理解栈空和栈满
- [ ] 理解 `pop` 和 `peek` 的区别
- [ ] 理解栈在嵌入式中的用途

## 代码实验

- [ ] 完成 `stack_basic.c`
- [ ] 完成 `stack_peek.c`
- [ ] 完成 `stack_test.c`
- [ ] 所有代码无编译警告
- [ ] AddressSanitizer 无报错
- [ ] 所有边界测试通过

---

# 十七、Git 提交

检查改动：

```bash
cd ~/projects/embedded-algorithm-learning

git status
git diff
```

添加文件：

```bash
git add week02/day01
```

检查暂存内容：

```bash
git diff --cached
```

提交：

```bash
git commit -m "feat: complete week02 day01 stack exercises"
```

推送：

```bash
git push
```

检查：

```bash
git status
git log --oneline -5
```

预期：

```text
nothing to commit, working tree clean
```

---

# 十八、下一步

完成概念学习后，按照以下顺序编写代码：

```text
1. stack_basic.c
2. stack_peek.c
3. stack_test.c
```

当前先开始：

```text
stack_basic.c
```