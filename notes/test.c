#include <stdio.h>
#include <stdlib.h>

int func(int c[3][4])
{
    printf("%zu %zu\n", sizeof c, sizeof c[0]);
}
int main() {
    
    int c = 87 + 10;

    printf("%c\n", c);

    return 0;
}
