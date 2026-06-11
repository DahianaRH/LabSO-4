#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Estructura para pasar argumentos al hilo */
typedef struct {
    long long *fib;
    int n;
} datos_fib_t;

/* Función que ejecutará el hilo trabajador */
void *generar_fibonacci(void *arg) {
    datos_fib_t *datos = (datos_fib_t *)arg;

    int n = datos->n;
    long long *fib = datos->fib;

    if (n > 0)
        fib[0] = 0;

    if (n > 1)
        fib[1] = 1;

    for (int i = 2; i < n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s <N>\\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        printf("N debe ser mayor que 0.\\n");
        return 1;
    }

    long long *fibonacci = (long long *)malloc(N * sizeof(long long));

    if (fibonacci == NULL) {
        printf("Error al reservar memoria.\\n");
        return 1;
    }

    datos_fib_t datos;
    datos.fib = fibonacci;
    datos.n = N;

    pthread_t trabajador;

    pthread_create(&trabajador, NULL,
                   generar_fibonacci, &datos);

    pthread_join(trabajador, NULL);

    printf("Secuencia de Fibonacci (%d elementos):\\n", N);

    for (int i = 0; i < N; i++) {
        printf("%lld ", fibonacci[i]);
    }

    printf("\\n");

    free(fibonacci);

    return 0;
}
