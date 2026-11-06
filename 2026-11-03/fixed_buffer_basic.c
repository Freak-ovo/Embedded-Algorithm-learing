#include <stdio.h>

/* 

    
    int value = 10；    数据
    &value              value 的地址
    p = &value；        p 中保存value 的地址
    *p = 10；           访问 p 指向的数据
    &p                  指针变量 p 自己的地址



*/
#define BUFFER_CAPACITY 5

int buffer_add(int buffer[], int capcity, int *size, int value) {
    
    if (*size >= capcity) {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;
    return 1;
}


void buffer_print(const int buffer[], int size) {
    printf("buffer: ");

    for (int i = 0; i < size; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}
int main() {


    int buffer[BUFFER_CAPACITY];

    int size = 0;

    int values[] = {10, 20, 30, 40, 50, 60};

    int values_count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < values_count; i++) {
        int success = buffer_add(buffer, BUFFER_CAPACITY, &size, values[i]);
        
        if (success) {
            printf("add %d: success\n", values[i]);
        }
        else {
            printf("add %d: buffer full\n", values[i]);
        }
        
        buffer_print(buffer, size);
    }

    return 0;
}