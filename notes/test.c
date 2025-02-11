#include <stdio.h>
#include <stdlib.h>

int func(int c[3][4])
{
    printf("%zu %zu\n", sizeof c, sizeof c[0]);
}
int main() {
    int arr[3][4] = {0}; // Initialize a 3x4 array

    func(arr);

    return 0;
}
