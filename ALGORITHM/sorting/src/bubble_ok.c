#include "public.h" 

void bubble_sort_ok(int a[], int n)
{
    int count = 0;
    int pos = 0;
    int low = 0;
    int high = n - 1;
    while(low < high)
    {
        for(int i = low; i < high; i++)
        {
            count++;
            if (a[i] > a[i + 1])
            {
                swap(a, i, i + 1);
                pos = i;
            }
        }
        high = pos;

        for(int j = high; j > low; j--)
        {
            count++;
            if (a[j] < a[j - 1])
            {
                swap(a, j - 1, j);
                pos = j;
            }
        }
        low = pos;
        
        //fprintf(stderr, "low=%d high=%d\n", low, high);
    }
    fprintf(stderr, " >> the number of swap is %d\n", count);
}

int main()
{
    //init_array(array, ARRAY_LENGTH);
    //print_array(array, ARRAY_LENGTH);
    bubble_sort_ok(array, ARRAY_LENGTH);
    //print_array(array, ARRAY_LENGTH);
    return 0;
}
