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


int main() {


    int number[] = {1, 2, 3, 4, 5};
    int length;
    length = sizeof(number) / sizeof(number[0]);

    int left = 0;
    int right = length - 1;

    int temp;
    //print_number(number, length);
    /* 自己写 */
/*     for (int i = 0; i < length; i++)
    {
        if (left < right)    return 0;
        {
            temp = number[left];
            number[left] = number[right];
            number[right] = temp;
            left++;
            right--;
        }
    }
 */
    print_number(number, length);

    /* 优化 */
    printf(" 优化后\n");

    while(left < right)
    {
        temp = number[left];
        number[left] = number[right];
        number[right] = temp;
        left++;
        right--;
    }
    print_number(number, length);

    return 0;
}