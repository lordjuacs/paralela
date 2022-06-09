double OpArr(double A[], double B[], double C[], int n) {
    int i, j;
    double s1, s2, a, res;
#pragma omp parallel
    {
#pragma omp sections
        {
#pragma omp section
            calculo_a(A, n); /* funcion calculo a */
#pragma omp section
            calculo_b(B, n); /* funcion calculo b */
#pragma omp section
            calculo_c(C, n); /* funcion calculo c */
        }

#pragma omp sections
        {
#pragma omp section
            for (i = 0; i < n; i++) { /* primer bucle for*/
                s1 = 0;
                for (j = 0; j < n; j++) s1 += A[i] * B[i];
                for (j = 0; j < n; j++) A[i] *= s1;
            }
#pragma omp section
            for (i = 0; i < n; i++) { /* segundo bucle for */
                s2 = 0;
                for (j = 0; j < n; j++) s2 += B[i] * C[i];
                for (j = 0; j < n; j++) C[i] *= s2;
            }
        }
/* calculo final */
#pragma omp single
        {
            a = s1 / s2;
            res = 0;
        }

#pragma omp for schedule(dynamic)
        for (i = 0; i < n; i++) res += a * C[i];
        return res;
    }
}

int main() {
    return 0;
}
