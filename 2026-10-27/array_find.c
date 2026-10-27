#include<stdio.h>

int find_value(const int number[], int length, int target)
{

    for (int i = 0; i < length; i++)
    {
        if (number[i] == target)
        {
            return number[i];
        }
    }


    return -1;
}

int main() {



    int number[] = {5, 8, 12, 3, 9};
    int length, target;


    length = sizeof(number) / sizeof(number[0]);

    target = 1;
    int index = find_value(number, length, target);


    if (index != -1)
    {
        printf("target = %d 找到了值 %d\n", target, index);
    }
    else
    {
        printf("target = %d 没有找到值\n", target);
    }

    return 0;
}