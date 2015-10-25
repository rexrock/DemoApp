#include <stdio.h>
#include <stdlib.h>


#define ARRAY_LENGTH 10
int array[ARRAY_LENGTH] = 
{2,5,3,7,9,8,4,1,0,6};

extern void init_array(int a[], int n)
{
    for(int i = 0; i < ARRAY_LENGTH; i++)
    {
        a[i] = rand();
    }
}

extern void print_array(int a[], int n)
{
    for(int i = 0; i < n; i++)
    {
        printf("%d, ", a[i]);
    }
    printf("\n");
}
