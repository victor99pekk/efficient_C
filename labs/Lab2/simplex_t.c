#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1E-6


typedef struct simplex_t{
    int m; /* nbr constraints */
    int n; /* decision vars */
    int* var; /* [n + m + 1] 0,..n-1 are non basic */
    double** a; /*[m][n+1]; /* A */
    double* b; /*[m]; /* b */
    double* x; /*[n+1]; /* c */
    double* c;
    double y; /* y */
} simplex_t;


int init(int n, int m, int* var, 
  double** a, double* b, double* x, double* c, double y){

    simplex_t* simplex = (simplex_t*)malloc(sizeof(simplex_t));
    if (simplex == NULL){
        fprint("memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    simplex->m = m;
    simplex->n = n;
    simplex->a = a;
    simplex->b = b;
    simplex->x = x;
    simplex->c = c;
    simplex->y = y;
    if (simplex->var == NULL){
        simplex->var = (double*)malloc((m+n+1)*sizeof(double));
        for (int i = 0; i < n+m+1; i++){
            simplex->var[i] = i;
        }
    }
    int k = 0;
    for (int i = 1; i < m; i++){
        if (b[i] < b[k]){
            k = i;
        }
    }
    return k;
}

int select_nonbasic(simplex_t* s){

    for(int i = 0; i < s->n; i++){
        if (s->c[i] > EPSILON){
            return i;
        }
    }
    return -1;
}


int initial(simplex_t* s, int n, int m, int* var, 
    double** a, double* b, double* x, double* c, double y){
    int i,j,k;
    double w;
    k = init(n, m, var, a, b, x, c, y);
    return b[k] >= 0;
}

void pivot(simplex_t* s, int row, int col){
    double** a = s->a;
    double* b = s->b;
    double* c = s->c;
    int n = s->n;
    int m = s->m;
    int j, i, t;

    int t = s->var[col];
    s->var[col] = s->var[n+row];
    s->var[n+row] = t;
    s->y = s->y + c[col] * b[row] / a[row][col] ;
    
    for(i = 0; i < n; i++){
        if (i != col) c[i] = c[i] - c[col] * a[row][i] / a[row][i];
    }
    c[col] = - c[col] / a[row][col];

    for (i = 0; i < m; i++){
        if (i != row) b[i] = b[i] - a[i][col] * b[row] / a[row][col];
    }

    for (i = 0; i < m; i++){
        if (i != row){
            for (j = 0; j < n; j++){
                if (j != col) a[i][j] -= a[i][col]*a[row][j] / a[row][col];
            }
        }
    }

    for (i = 0; i < m; i++){
        if (i != row) a[i][col] = -a[i][col] / a[row][col];
    }

    for(i = 0; i < n; i++){
        if(i != col) a[row][i] = a[row][i] / a[row][col];
    }

    b[row] = b[row] / a[row][col];
    a[row][col] = 1 / a[row][col];
}


double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, double* var, int h){
    simplex_t s;
    int i, row, col;
    if (!initial(&s, n, m, var, a, b, x, c, y)){
        s.var = NULL;
        return NAN;
    }
    col = select_nonbasic(&s);
    while (col > 0)
    {
        row = -1;
        for (i = 0; i < m; i++){
            if (a[i][col] > EPSILON &&
                row < 0 || (b[i] / a[i][col]) < (b[row] / a[row][col]))
                row = i;
        }
        if (row < 0){
            s.var = NULL;
            return INFINITY;
        }
        pivot(&s, row, col);
    }
    
    if (h == 0)
    {
        for (i = 0; i < n; i++){
            if(s.var[i] < n) x[s.var[n+i]] = 0;
        }
        for (i = 0; i < m; i++){
            if (s.var[n+i] < n) x[s.var[n+i]] = s.b[i];
        }
        s.var = NULL;
    }else{
        for (i = 0; i < n; i++){
            if(s.var[i] < n) x[i] = 0;
        }
        for (i = n; i < m+n; i++){
            if (s.var[n+i] < n) x[i] = s.b[i-n];
        }
    }
    return s.y;
}

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y, double* var, int h){
    return xsimplex(m, n, a, b, c, x, y, var, h);
}