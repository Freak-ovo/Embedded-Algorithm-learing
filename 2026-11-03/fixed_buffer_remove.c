#include <stdio.h>

#define BUFFER_CAPACITY 5

int buffer_add(int buffer[], int capcity, int *size, int value) {
    
    if (*size >= capcity) {
        return 0;
    }

    buffer[*size] = value;
    (*size)++;
    return 1;
}

int buffer_remove(int buffer[], int *size, int index) {
    if (index < 0 || index >= *size) {
        return 0;
    }
    else {
        for (int i = index; i < *size -1; i++) {

            buffer[i] = buffer[i + 1];
        }
        (*size)--;
        return 1;
    }
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

    buffer_add(buffer, BUFFER_CAPACITY, &size, 10);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 20);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 30);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 40);
    buffer_add(buffer, BUFFER_CAPACITY, &size, 50);

    printf("initial: \n");
    buffer_print(buffer, size);

    int success = buffer_remove(buffer, &size, 2);

    if (success) {
        printf("remove index 2: success! \n");
    }
    else {
        printf("remove index 2: invalid index! \n");
    }

    buffer_print(buffer, size);

    success = buffer_remove(buffer, &size, 10);

    if (success) {
        printf("remove index 10 : success \n");
    }
    else {
        printf("remove index 10 : invalid index \n");
    }

    buffer_print(buffer, size);

    return 0;
}