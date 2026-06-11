#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

double fH;        /* h = 1/n, solo lectura en los hilos */
long   n;
int    T;

typedef struct {
    int  tid;
    long start;   /* inclusiva  */
    long end;     /* exclusiva  */
} ThreadArg;

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

void *worker(void *arg) {
    ThreadArg *a = (ThreadArg *) arg;
    double local_sum = 0.0;
    for (long i = a->start; i < a->end; i++) {
        double x = fH * ((double) i + 0.5);
        local_sum += f(x);
    }
    double *result = malloc(sizeof(double));
    *result = local_sum;
    return (void *) result;   /* retorno parcial via pthread_join */
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <num_hilos> [n]\n", argv[0]);
        return 1;
    }
    T = atoi(argv[1]);
    if (T <= 0) {
        fprintf(stderr, "Error: el numero de hilos debe ser > 0\n");
        return 1;
    }
    n = (argc >= 3) ? atol(argv[2]) : 2000000000L;

    fH = 1.0 / (double) n;

    pthread_t *threads = malloc(T * sizeof(pthread_t));
    ThreadArg *args    = malloc(T * sizeof(ThreadArg));

    /* Particion en bloques contiguos; el residuo se reparte de a 1 */
    long base = n / T;
    long rem  = n % T;
    long offset = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);   /* inicio: antes de crear hilos */

    for (int t = 0; t < T; t++) {
        long chunk = base + (t < rem ? 1 : 0);
        args[t].tid   = t;
        args[t].start = offset;
        args[t].end   = offset + chunk;
        offset += chunk;
        pthread_create(&threads[t], NULL, worker, &args[t]);
    }

    double total = 0.0;
    for (int t = 0; t < T; t++) {
        void *ret;
        pthread_join(threads[t], &ret);
        total += *(double *) ret;
        free(ret);
    }

    double pi = fH * total;       /* reduccion final */

    gettimeofday(&end, NULL);     /* fin: tras la reduccion */

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1.0e6;

    printf("Hilos = %d\n", T);
    printf("n = %ld\n", n);
    printf("PI = %.15f\n", pi);
    printf("TIME = %.6f\n", elapsed);   /* linea parseable */

    free(threads);
    free(args);
    return 0;
}
