#include <stdio.h>

#define UART_RX_CAPACITY    8U

typedef struct {
    unsigned char data[UART_RX_CAPACITY];
    unsigned int head;
    unsigned int tail;
    unsigned int overflow_count;
} UartRxQueue;

static unsigned int next_index(unsigned int index) {
    return (index + 1U) % UART_RX_CAPACITY;
}

void uart_rx_init(UartRxQueue *queue) {
    queue->head = 0U;
    queue->tail = 0U;
    queue->overflow_count = 0U;
}

int uart_rx_is_empty(const UartRxQueue *queue) {
    return queue->head == queue->tail;
}

int uart_rx_is_full(const UartRxQueue *queue) {
    return next_index(queue->head) == queue->tail;
}

unsigned int uart_rx_count(const UartRxQueue *queue) {
    if (queue -> head >= queue->tail) {
        return queue->head - queue->tail;
    
    }
    return UART_RX_CAPACITY - queue->tail + queue->head;
}

int uart_rx_receive_byte(UartRxQueue *queue, unsigned char byte) {
    unsigned int next_head = next_index(queue->head);

    if (next_head == queue->tail) {
        queue->overflow_count++;
        return 0;
    }
    queue->data[queue->head] = byte;
    queue->head = next_head;
    return 1;
}

int uart_rx_read_byte(UartRxQueue *queue, unsigned char *out_byte) {
    if (queue->head == queue->tail) {
        return 0;
    }
    *out_byte = queue->data[queue->tail];
    queue->tail = next_index(queue->tail);
    return 1;
}

int main() {
    UartRxQueue queue;
    unsigned char byte;
    
    uart_rx_init(&queue);

    const unsigned char input[] = {'A', 'B', 'C', '\n'};

    unsigned int input_count = sizeof(input) / sizeof(input[0]);

    for (unsigned int i =0U; i < input_count; i++) {
        int success = uart_rx_receive_byte(&queue, input[i]);
        printf("receive 0x%02x: %s\n", input[i], success ? "stored" : "dropped");
    }

    printf("queue bytes = %u\n", uart_rx_count(&queue));

    while (uart_rx_read_byte(&queue, &byte)) {
        printf("read 0x%02X\n", byte);
    }
    printf("overflow count = %u\n", queue.overflow_count);

    return 0;
}