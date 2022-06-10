double OpArr(double A[], double B[], double C[], int n) {
    int i, j;
    double s1, s2, a, res;
#pragma omp parallel {
#pragma omp single
    calculo_a(A, n); /* funcion calculo a */
#pragma omp single
    calculo_b(B, n); /* funcion calculo b */
#pragma omp single
    calculo_c(C, n); /* funcion calculo c */
#pragma omp task private(i, j) depend(out \
                                      : A, B, s1) {
    for (i = 0; i < n; i++) { /* primer bucle for*/
        s1 = 0;
        for (j = 0; j < n; j++) s1 += A[i] * B[i];
        for (j = 0; j < n; j++) A[i] *= s1;
    }
}
#pragma omp task private(i, j) depend(out \
                                      : B, C, s2) {
for (i = 0; i < n; i++) { /* segundo bucle for */
    s2 = 0;
    for (j = 0; j < n; j++) s2 += B[i] * C[i];
    for (j = 0; j < n; j++) C[i] *= s2;
}
}
/* calculo final */
#pragma omp task single depend(out \
                               : a, s1, s2, res) {
a = s1 / s2;
res = 0;
}
#pragma omp task private(i) depend(out \
                                   : res, a, C) {
for (i = 0; i < n; i++) res += a * C[i];
return res;
}
}
}
