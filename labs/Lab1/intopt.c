#include <stdio.h>
#include <stdlib.h>

int main(){
    //printf("Hello word!\n");

    int m,n,b;
    scanf("%d%d", &m, &n);
    float c_arr[n-1];
    float b_arr[m];
    float** A = (float**)calloc(m, sizeof(float*));

    for (int i = 0; i < n; i++){
        scanf("%f", &c_arr[i]);
    }

    for (int i = 0; i < m; i++){
        A[i] = (float*)calloc(n, sizeof(int));
        for (int j = 0; j < n; j++){
            scanf("%f", &A[i][j]);
        }
    }

    for (int i = 0; i < m; i++){
        scanf("%f", &b_arr[i]);
    }

    printf("\nLinear Program:\n");
    printf("\nMax z =");
    for (int i = 0; i < n - 1; i ++){
        printf("%10.3f x%d + ", c_arr[i],  i);
    }
    printf("%10.3f x%d \n", c_arr[n-1], n-1);

    printf("\n");
    for (int i = 0; i < m; i++){
        for(int j = 0; j < n - 1; j++){
            printf("%10.3f x%d +  ", A[i][j], j);
        }
        printf("%10.3f x%d <= %10.3f\n", A[i][n-1], n-1, b_arr[i]);
    }
    printf("\n");


    for (int i = 0; i < m; i++){
        free(A[i]);
    }
    free(A);
    return 0;
    
}