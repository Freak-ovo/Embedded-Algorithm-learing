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

int remove_value(int numbers[], int length, int target)
{
    int write = 0;
    for (int read = 0; read < length; read++)
    {
        if (numbers[read] != target)
        {
            numbers[write] = numbers[read];
            write++;
        }
    }
    return write;
}


int main() 
{


    int number[] = {3, 2, 2, 3, 4, 2};
    int length;
    
    length = sizeof(number) / sizeof(number[1]);
    int target = 2;

    printf("before: ");
    print_number(number, length);

    printf("afte: ");


    int new_length = remove_value(number, length, target);
    print_number(number, new_length);
    
    return 0;
}