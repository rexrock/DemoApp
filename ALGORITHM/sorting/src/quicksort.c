#include <stdio.h>
#include "public.h"

void QuickSort(int s[],int low,int high) {
    int i;
    int last;
    if(low<high) {
        last=low

        //把小于基准元与大于基准元的分开，last记录它们分开的界限
        for(i=low+1;i<=high;i++) {
            if(s[i]<s[low]) {
                swap(s,++last,i);
            }
        }

        swap(s,last,low);
        QuickSort(s,low,last-1);
        QuickSort(s,last+1,high);
    }
}

int main() {
    //init_array(array, ARRAY_LENGTH);
    //print_array(array, ARRAY_LENGTH);
    QuickSort(array, 0, ARRAY_LENGTH - 1);
    //print_array(array, ARRAY_LENGTH);
    return 0;
}

