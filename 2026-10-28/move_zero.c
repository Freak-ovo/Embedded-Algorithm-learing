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
int move_zero(int numbers[], int length, int target)
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
    for (int j = write; j < length; j++)
    {
        numbers[write] = 0;
        write++;
    }
/*     while (write < length) {
        numbers[write] = 0;
        write++;
    
    } */
    return write;

}
int main()
{
    int numbers[] = {0, 1, 0, 3, 12};
    int length;
    length = sizeof(numbers) / sizeof(numbers[1]);

    int new_length = move_zero(numbers, length, 0);
    print_number(numbers, new_length);

}