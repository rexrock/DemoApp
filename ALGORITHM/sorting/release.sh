mkdir -p ./bin
gcc src/bubble.c -o ./bin/bubble -Iinclude/ --std=c99 
gcc src/bubble_ok.c -o ./bin/bubble_ok -Iinclude/ --std=c99
gcc src/quicksort.c -o ./bin/quicksort -Iinclude/ --std=c99
