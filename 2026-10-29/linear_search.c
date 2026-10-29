#include<stdio.h>


void print_number(const int number[], int length) 
{
    printf("number: ");
    
    for (int i = 0; i < length; i++)
    {
        printf(" %d", number[i]);

    }

    printf("\n");

}

int liner_search(const int numbers[], int length, int target) {

    
    for (int i = 0; i < length; i++) {

        if (numbers[i] == target) {

            return i;

        }

    }

    return -1;





}

int main() {

    int numbers[] = {5, 8, 12, 3, 9};
    int length;
    int target = 12;

    length = sizeof(numbers) / sizeof(numbers[0]);

    print_number(numbers, length);

    int index = liner_search(numbers, length, 12);

    if (target == index) {

    printf("target  %d found at index %d\n", numbers[index], index);

    }
    else {
    printf("target  %d not found n", target);
    
    }

    return 0;
}