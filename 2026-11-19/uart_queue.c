#include <stdio.h>

#define UART_QUEUE_CAPACITY 8U

/*
 * 通用字节环形队列。
 *
 * head:
 *     下一次写入的位置，由生产者修改。
 *
 * tail:
 *     下一次读取的位置，由消费者修改。
 *
 * overflow_count:
 *     队列满时写入失败的累计次数。
 */
typedef struct
{
    unsigned char data[UART_QUEUE_CAPACITY];
    unsigned int head;
    unsigned int tail;
    unsigned int overflow_count;
} ByteQueue;

/*
 * 一个 UART 模块包含两个独立队列。
 *
 * rx_queue:
 *     保存已经接收、尚未被主程序读取的数据。
 *
 * tx_queue:
 *     保存业务程序已经提交、尚未发送的数据。
 */
typedef struct
{
    ByteQueue rx_queue;
    ByteQueue tx_queue;
} Uart;

/*
 * 计算下一个数组位置。
 *
 * 下标变化：
 * 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0
 */
static unsigned int queue_next_index(unsigned int index)
{
    return (index + 1U) % UART_QUEUE_CAPACITY;
}

/*
 * 初始化一个字节队列。
 *
 * head == tail 表示队列为空。
 */
static void queue_init(ByteQueue *queue)
{
    queue->head = 0U;
    queue->tail = 0U;
    queue->overflow_count = 0U;
}

/*
 * 判断队列是否为空。
 */
static int queue_is_empty(const ByteQueue *queue)
{
    return queue->head == queue->tail;
}

/*
 * 判断队列是否已满。
 *
 * 为了区分空和满，环形队列预留一个空槽。
 *
 * 物理容量为 8 字节，
 * 实际最多保存 7 字节。
 */
static int queue_is_full(const ByteQueue *queue)
{
    return queue_next_index(queue->head) == queue->tail;
}

/*
 * 返回队列中的有效字节数量。
 */
static unsigned int queue_count(const ByteQueue *queue)
{
    if (queue->head >= queue->tail)
    {
        return queue->head - queue->tail;
    }

    return UART_QUEUE_CAPACITY
           - queue->tail
           + queue->head;
}

/*
 * 返回队列中的剩余可用空间。
 */
static unsigned int queue_free_space(const ByteQueue *queue)
{
    return (UART_QUEUE_CAPACITY - 1U)
           - queue_count(queue);
}

/*
 * 向队列中写入一个字节。
 *
 * 返回值：
 * 1 表示成功；
 * 0 表示队列已满。
 */
static int queue_write(ByteQueue *queue,
                       unsigned char byte)
{
    unsigned int next_head =
        queue_next_index(queue->head);

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
 * 从队列中读取一个字节。
 *
 * 返回值：
 * 1 表示成功；
 * 0 表示队列为空。
 */
static int queue_read(ByteQueue *queue,
                      unsigned char *out_byte)
{
    if (queue->head == queue->tail)
    {
        return 0;
    }

    *out_byte = queue->data[queue->tail];
    queue->tail = queue_next_index(queue->tail);

    return 1;
}

/*
 * 初始化 UART 模块的 RX 和 TX 队列。
 */
void uart_init(Uart *uart)
{
    queue_init(&uart->rx_queue);
    queue_init(&uart->tx_queue);
}

/*
 * 模拟 UART 硬件接收到一个字节。
 *
 * 实际工程中通常由 UART RX 中断调用。
 *
 * RX生产者：UART接收中断
 * RX消费者：主程序
 */
int uart_receive_byte(Uart *uart,
                      unsigned char byte)
{
    return queue_write(&uart->rx_queue, byte);
}

/*
 * 主程序从 RX 队列中读取一个字节。
 */
int uart_read_byte(Uart *uart,
                   unsigned char *out_byte)
{
    return queue_read(&uart->rx_queue, out_byte);
}

/*
 * 业务程序提交一个待发送字节。
 *
 * TX生产者：业务程序
 * TX消费者：UART发送中断
 */
int uart_write_byte(Uart *uart,
                    unsigned char byte)
{
    return queue_write(&uart->tx_queue, byte);
}

/*
 * 模拟 UART TX 中断取得下一个待发送字节。
 *
 * 真实驱动中，取得字节后会把字节写入 UART
 * 发送数据寄存器。
 */
int uart_transmit_one(Uart *uart,
                      unsigned char *out_byte)
{
    return queue_read(&uart->tx_queue, out_byte);
}

/*
 * 打印普通字符和特殊字符。
 */
static void print_byte(unsigned char byte)
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
 * 打印 RX 和 TX 队列状态。
 */
static void uart_print_state(const Uart *uart)
{
    printf("RX: head=%u, tail=%u, count=%u, free=%u, overflow=%u\n",
           uart->rx_queue.head,
           uart->rx_queue.tail,
           queue_count(&uart->rx_queue),
           queue_free_space(&uart->rx_queue),
           uart->rx_queue.overflow_count);

    printf("TX: head=%u, tail=%u, count=%u, free=%u, overflow=%u\n",
           uart->tx_queue.head,
           uart->tx_queue.tail,
           queue_count(&uart->tx_queue),
           queue_free_space(&uart->tx_queue),
           uart->tx_queue.overflow_count);
}

int main(void)
{
    Uart uart;
    unsigned char byte;

    uart_init(&uart);

    /*
     * 第一部分：模拟 UART 接收。
     *
     * 硬件依次收到 A、B、C 和换行符。
     */
    const unsigned char rx_input[] =
    {
        'A',
        'B',
        'C',
        '\n'
    };

    unsigned int rx_count =
        sizeof(rx_input) / sizeof(rx_input[0]);

    printf("UART RX receives:\n");

    for (unsigned int i = 0U; i < rx_count; i++)
    {
        int success =
            uart_receive_byte(&uart, rx_input[i]);

        printf("receive ");

        print_byte(rx_input[i]);

        if (success)
        {
            printf(": stored\n");
        }
        else
        {
            printf(": RX queue full\n");
        }
    }

    printf("\nstate after receiving:\n");
    uart_print_state(&uart);

    /*
     * 第二部分：模拟主程序读取 RX 数据。
     */
    printf("\napplication reads RX queue:\n");

    while (uart_read_byte(&uart, &byte))
    {
        printf("read ");

        print_byte(byte);

        printf("\n");
    }

    printf("\nstate after reading RX queue:\n");
    uart_print_state(&uart);

    /*
     * 第三部分：业务程序提交待发送数据。
     */
    const unsigned char tx_output[] =
    {
        'O',
        'K',
        '\n'
    };

    unsigned int tx_count =
        sizeof(tx_output) / sizeof(tx_output[0]);

    printf("\napplication writes TX queue:\n");

    for (unsigned int i = 0U; i < tx_count; i++)
    {
        int success =
            uart_write_byte(&uart, tx_output[i]);

        printf("queue ");

        print_byte(tx_output[i]);

        if (success)
        {
            printf(": stored\n");
        }
        else
        {
            printf(": TX queue full\n");
        }
    }

    printf("\nstate before transmitting:\n");
    uart_print_state(&uart);

    /*
     * 第四部分：模拟 UART TX 中断逐字节发送。
     */
    printf("\nUART TX sends:\n");

    while (uart_transmit_one(&uart, &byte))
    {
        printf("send ");

        print_byte(byte);

        printf("\n");
    }

    printf("\nfinal state:\n");
    uart_print_state(&uart);

    /*
     * 最终确认两个队列都为空。
     */
    printf("RX empty: %s\n",
           queue_is_empty(&uart.rx_queue)
               ? "yes"
               : "no");

    printf("TX empty: %s\n",
           queue_is_empty(&uart.tx_queue)
               ? "yes"
               : "no");

    printf("RX full: %s\n",
           queue_is_full(&uart.rx_queue)
               ? "yes"
               : "no");

    printf("TX full: %s\n",
           queue_is_full(&uart.tx_queue)
               ? "yes"
               : "no");

    return 0;
}