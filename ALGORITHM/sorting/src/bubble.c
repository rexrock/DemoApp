#include "public.h"
void bubble_sort(int a[], int n)
{
    int count = 0;
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = 0; j < n - i -1; j++)
        {
            count++;
            if (a[j] > a[j + 1])
            {
                //fprintf(stderr, "---- : %d <--> %d\n", a[j], a[j+1]);
                int tmp = a[j + 1];
                a[j + 1] = a[j];
                a[j] = tmp;
            }
        }
    }
    fprintf(stderr, " >> %d\n", count);
}

int main()
{
    //init_array(array, ARRAY_LENGTH);
    //print_array(array, ARRAY_LENGTH);
    bubble_sort(array, ARRAY_LENGTH);
    print_array(array, ARRAY_LENGTH);
    return 0;
}
