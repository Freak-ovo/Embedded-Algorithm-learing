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

int binary_search(const int numbers[], int length, int target) {

    int left = 0;
    int right = length - 1;

    while (left <= right) {
        
        int middle =left + (right - left) / 2;

        printf("left = %d, right = %d, middle = %d, value = %d\n", 
                left, right, middle, numbers[middle]);       
        if (numbers[middle] == target) {
        
            return middle;
        }
        else if (numbers[middle] > target) {
            right = middle - 1;

        }
        else  if (numbers[middle] < target){
            
            left = middle + 1;
        
        }
    }
    return  -1;
}

int main(void) {

    int numbers[] = {2, 5, 8, 12, 16, 23, 56, 72, 91};

    int length;

    length = sizeof(numbers) / sizeof(numbers[0]);


    print_number(numbers, length);

    int target = 56;
    int index = binary_search(numbers, length, target);

    if (index >= 0) {

        printf("target %d found at index %d\n", target, index);
        printf("value = %d, index = %d\n", numbers[index], index);
    }
    else {
        printf("target %d not found\n", target);
    
    }
}