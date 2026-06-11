#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

double CalcPi(long n) {
    double h = 1.0 / (double) n;
    double sum = 0.0;
    for (long i = 0; i < n; i++) {
        double x = h * ((double) i + 0.5);  /* punto medio */
        sum += f(x);
    }
    return h * sum;
}

int main(int argc, char *argv[]) {
    long n = 2000000000L;          /* valor por defecto */
    if (argc == 2) n = atol(argv[1]);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    double pi = CalcPi(n);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1.0e6;

    printf("n = %ld\n", n);
    printf("PI = %.15f\n", pi);
    printf("TIME = %.6f\n", elapsed); 
    return 0;
}
