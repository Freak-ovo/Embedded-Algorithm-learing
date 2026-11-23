#include <stdio.h>

/*
 * TX 队列物理容量为 8 字节。
 *
 * 由于需要预留一个空槽区分“队列为空”和“队列已满”，
 * 实际最多可以保存 7 个待发送字节。
 */
#define UART_TX_CAPACITY 8U

/*
 * UART TX 环形队列。
 *
 * data:
 *     保存等待发送的字节。
 *
 * head:
 *     生产者下一次写入的位置。
 *     在实际驱动中，生产者通常是业务程序。
 *
 * tail:
 *     消费者下一次读取的位置。
 *     在实际驱动中，消费者通常是 UART TX 中断。
 *
 * overflow_count:
 *     队列已满时，写入失败的累计次数。
 */
typedef struct
{
    unsigned char data[UART_TX_CAPACITY];
    unsigned int head;
    unsigned int tail;
    unsigned int overflow_count;
} UartTxQueue;

/*
 * 计算下一个数组下标。
 *
 * 当下标到达数组最后一个位置后，
 * 下一次会回到下标 0。
 *
 * 例如容量为 8：
 *
 * 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0
 */
static unsigned int next_index(unsigned int index)
{
    return (index + 1U) % UART_TX_CAPACITY;
}

/*
 * 初始化 TX 队列。
 *
 * head == tail 表示队列为空。
 * data 数组不需要清零，因为有效数据由 head 和 tail 决定。
 */
void uart_tx_init(UartTxQueue *queue)
{
    queue->head = 0U;
    queue->tail = 0U;
    queue->overflow_count = 0U;
}

/*
 * 判断 TX 队列是否为空。
 *
 * 返回值：
 *     1 表示队列为空；
 *     0 表示队列中存在待发送数据。
 */
int uart_tx_is_empty(const UartTxQueue *queue)
{
    return queue->head == queue->tail;
}

/*
 * 判断 TX 队列是否已满。
 *
 * 队列会预留一个空槽。
 * 如果 head 的下一个位置等于 tail，
 * 就表示不能继续写入。
 *
 * 返回值：
 *     1 表示队列已满；
 *     0 表示队列仍有空间。
 */
int uart_tx_is_full(const UartTxQueue *queue)
{
    return next_index(queue->head) == queue->tail;
}

/*
 * 计算当前等待发送的字节数量。
 *
 * 情况一：
 *     head >= tail
 *     数据没有跨过数组末尾。
 *
 * 情况二：
 *     head < tail
 *     数据已经发生回绕。
 */
unsigned int uart_tx_count(const UartTxQueue *queue)
{
    if (queue->head >= queue->tail)
    {
        return queue->head - queue->tail;
    }

    return UART_TX_CAPACITY
           - queue->tail
           + queue->head;
}

/*
 * 计算 TX 队列剩余可用空间。
 *
 * 因为预留了一个空槽，
 * 实际容量为 UART_TX_CAPACITY - 1。
 */
unsigned int uart_tx_free_space(const UartTxQueue *queue)
{
    return (UART_TX_CAPACITY - 1U)
           - uart_tx_count(queue);
}

/*
 * 业务程序向 TX 队列提交一个字节。
 *
 * 执行顺序：
 *     1. 计算 head 的下一个位置；
 *     2. 判断队列是否已满；
 *     3. 将字节写入当前 head；
 *     4. 更新 head。
 *
 * 返回值：
 *     1 表示写入成功；
 *     0 表示队列已满，写入失败。
 */
int uart_tx_write_byte(UartTxQueue *queue,
                       unsigned char byte)
{
    unsigned int next_head = next_index(queue->head);

    if (next_head == queue->tail)
    {
        queue->overflow_count++;
        return 0;
    }

    queue->data[queue->head] = byte;
    queue->head = next_head;

    return 1;
}

/*
 * UART 发送端从 TX 队列取出一个字节。
 *
 * 在真实驱动中，这个操作通常由 UART TX 空中断执行。
 *
 * 执行顺序：
 *     1. 判断队列是否为空；
 *     2. 读取当前 tail 位置的数据；
 *     3. 更新 tail。
 *
 * 返回值：
 *     1 表示成功取得一个字节；
 *     0 表示队列为空。
 */
int uart_tx_get_next_byte(UartTxQueue *queue,
                          unsigned char *out_byte)
{
    if (queue->head == queue->tail)
    {
        return 0;
    }

    *out_byte = queue->data[queue->tail];
    queue->tail = next_index(queue->tail);

    return 1;
}

/*
 * 打印一个字节。
 *
 * 换行符和回车符使用可见形式输出，
 * 便于观察发送顺序。
 */
void print_byte(unsigned char byte)
{
    if (byte == '\n')
    {
        printf("\\n");
    }
    else if (byte == '\r')
    {
        printf("\\r");
    }
    else
    {
        printf("%c", byte);
    }
}

/*
 * 打印 TX 队列当前状态。
 */
void uart_tx_print_state(const UartTxQueue *queue)
{
    printf("head = %u, tail = %u, count = %u, free = %u\n",
           queue->head,
           queue->tail,
           uart_tx_count(queue),
           uart_tx_free_space(queue));
}

int main(void)
{
    UartTxQueue tx_queue;
    unsigned char byte;

    uart_tx_init(&tx_queue);

    /*
     * 模拟业务程序提交待发送数据。
     *
     * 实际待发送顺序：
     *     O
     *     K
     *     换行符
     */
    const unsigned char output[] =
    {
        'O',
        'K',
        '\n'
    };

    unsigned int output_count =
        sizeof(output) / sizeof(output[0]);

    printf("application writes to TX queue:\n");

    /*
     * 业务程序是生产者。
     *
     * 生产者只修改 head，
     * 不修改 tail。
     */
    for (unsigned int i = 0U;
         i < output_count;
         i++)
    {
        int success =
            uart_tx_write_byte(&tx_queue, output[i]);

        printf("queue ");

        print_byte(output[i]);

        if (success)
        {
            printf(": stored\n");
        }
        else
        {
            printf(": queue full\n");
        }

        uart_tx_print_state(&tx_queue);
    }

    printf("\nUART transmitter sends:\n");

    /*
     * 模拟 UART TX 中断逐字节发送。
     *
     * 消费者只修改 tail，
     * 不修改 head。
     */
    while (uart_tx_get_next_byte(&tx_queue, &byte))
    {
        printf("send ");

        print_byte(byte);

        printf("\n");

        uart_tx_print_state(&tx_queue);
    }

    printf("\nfinal state:\n");

    uart_tx_print_state(&tx_queue);

    printf("empty = %s\n",
           uart_tx_is_empty(&tx_queue)
               ? "yes"
               : "no");

    printf("full = %s\n",
           uart_tx_is_full(&tx_queue)
               ? "yes"
               : "no");

    printf("overflow count = %u\n",
           tx_queue.overflow_count);

    return 0;
}