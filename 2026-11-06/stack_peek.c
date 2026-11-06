#include <stdio.h>
#define STACK_CAPACITY  5



int stack_push(int stack[], int capacity, int *size, int value) {

    if (*size > capacity) {
        return 0;
    }
    stack[*size] = value;
    (*size)++;

    return 1;
}


int stack_pop(int stack[], int *size, int *value) {
    if (*size <= 0) {
        return 0;
    }

    *value = stack[*size - 1];
    (*size)--;

    return 1;
}

int stack_peek(const int stack[], int size, int *value) {
    if (size <= 0) {
        return 0;
    }
    *value = stack[size - 1];

    return 1;

}



void stack_print(const int stack[], int size) {
    printf("stack:");

    for (int i = 0; i < size; i++){
        printf(" %d", stack[i]);
    }

    printf("\n");
}
int main() {



    int stack[STACK_CAPACITY];
    int size = 0;
    int value;
    int values[] = {10, 20, 30};
    int value_count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < value_count; i++) {

        int success = stack_push(stack, STACK_CAPACITY, &size, values[i]);

        if (success) {
            printf("push %d success\n",values[i]);
        }
        else {
            printf("push %d: stack full\n",values[i]);
        }

        stack_print(stack, size);
    }
    


    int popped_value;
    int success = stack_pop(stack, &size, &popped_value);

    if (success) {
        printf("pop: %d\n", popped_value);
    }
    else {
        printf("pop: stack empty\n");
    }

    printf("afte pop: \n");
    stack_print(stack, size);
    printf("size = %d\n", size);

    success = stack_peek(stack, size, &value);


    if (success) {
        printf("peek again: %d\n", value);
    }
    else {
        printf("peek again: stack empty\n");
    }



    return 0;
}