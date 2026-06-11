#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s <N>\\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    long long *fib = malloc(N * sizeof(long long));

    struct timeval inicio, fin;

    gettimeofday(&inicio, NULL);

    if (N > 0) fib[0] = 0;
    if (N > 1) fib[1] = 1;

    for (int i = 2; i < N; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    gettimeofday(&fin, NULL);

    double tiempo =
        (fin.tv_sec - inicio.tv_sec) +
        (fin.tv_usec - inicio.tv_usec)/1e6;

    printf("TIME = %.6f\\n", tiempo);

    free(fib);

    return 0;
}
