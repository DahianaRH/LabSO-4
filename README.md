# LabSO-4
Laboratorio 4 de Sistemas Operativos enfocado en API de Hilos

## Members
- Sebastian Andres Berrio Murillo, CC. 1066753315, sebastian.berriom@udea.edu.co.
- Sandy Dahiana Ruiz Higuita, CC. 1028031733, sandy.ruizh@udea.edu.co

## Video de sustentación:
link: https://youtu.be/sUv3-Pu4esg

## 1. Cálculo de π (PI)

### Programa Secuencial (`pi.c`)

* **`double f(double x)`**
    * **Descripción:** Evalúa numéricamente la función matemática $f(x) = \frac{4}{1 + x^2}$. Esta función representa la derivada de la función arcotangente, cuya integral en el intervalo $[0, 1]$ es exactamente igual a $\pi$.
    * **Parámetros:** `x` (`double`) - El punto en el eje de las abscisas donde se evaluará la función.
    * **Retorno:** (`double`) - El resultado de la evaluación.

* **`double CalcPi(long n)`**
    * **Descripción:** Realiza la aproximación del valor de Pi utilizando el método de integración numérica del punto medio (Suma de Riemann). Divide el intervalo $[0, 1]$ en `n` rectángulos y suma sus áreas.
    * **Parámetros:** `n` (`long`) - El número total de subdivisiones o rectángulos para la aproximación.
    * **Retorno:** (`double`) - El valor calculado de Pi.

* **`int main(int argc, char *argv[])`**
    * **Descripción:** Punto de entrada del programa ejecutable. Maneja los argumentos de la línea de comandos para configurar `n`, gestiona la medición del tiempo de ejecución mediante `gettimeofday`, invoca a `CalcPi` e imprime los resultados en un formato legible.

### Programa Paralelo (`pi_p.c`)

* **`double f(double x)`**
    * **Descripción:** Misma funcionalidad que en la versión secuencial; calcula el valor de la curva en un punto `x`.

* **`void *worker(void *arg)`**
    * **Descripción:** Rutina ejecutada por cada hilo secundario de la biblioteca POSIX Threads (`pthreads`). Procesa un subconjunto contiguo de rectángulos definido por los límites `start` y `end` asignados. Calcula una suma local acumulada para evitar la contención de memoria.
    * **Parámetros:** `arg` (`void *`) - Un puntero genérico que se transforma a `ThreadArg *`, el cual contiene el ID del hilo y sus rangos de iteración.
    * **Retorno:** (`void *`) - Un puntero a una zona de memoria dinámica (`malloc`) que almacena el resultado local de tipo `double`.

* **`int main(int argc, char *argv[])`**
    * **Descripción:** Orquestador principal del algoritmo paralelo. Lee el número de hilos `T` y opcionalmente `n`. Divide la carga de trabajo equitativamente entre los hilos (repartiendo el residuo de la división). Crea los hilos con `pthread_create`, espera su finalización con `pthread_join`, unifica los resultados parciales (reducción), calcula el tiempo total transcurrido y libera los recursos asignados.

## 2. Generador de Secuencia de Fibonacci

### 2.1. Documentación de las funciones desarrolladas

### Estructura `datos_fib_t`
**Propósito:** Agrupar los datos necesarios para que el hilo trabajador pueda generar la secuencia de Fibonacci.

**Campos:**
* `long long *fib`: puntero al arreglo compartido donde se almacenan los números de Fibonacci.
* `int n`: cantidad de elementos de la secuencia a generar.
  
**Uso:** La estructura es inicializada por el hilo principal y su dirección es enviada al hilo trabajador mediante `pthread_create()`.

---

## Función `generar_fibonacci`
```c
void *generar_fibonacci(void *arg)
```
**Propósito:** Generar los primeros N elementos de la secuencia de Fibonacci dentro del hilo trabajador.

**Parámetros:**
* `arg`: puntero genérico que contiene la dirección de una estructura `datos_fib_t`.

**Proceso:**

1. Convierte el argumento recibido al tipo `datos_fib_t`.
2. Obtiene el tamaño de la secuencia y el arreglo compartido.
3. Inicializa los dos primeros elementos de la secuencia.
4. Calcula iterativamente los valores restantes.
5. Almacena cada resultado en el arreglo compartido.
6. Finaliza la ejecución del hilo mediante `pthread_exit()`.
**Valor de retorno:**
* No retorna información adicional; finaliza con `pthread_exit(NULL)`.

---

## Función `main`
```c
int main(int argc, char *argv[])
```
**Propósito:** Coordinar la ejecución general del programa.

**Parámetros:**

* `argc`: cantidad de argumentos recibidos.
* `argv`: arreglo de cadenas que contiene los argumentos de entrada.
  
**Proceso:**

1. Verifica que el usuario haya suministrado el valor N.
2. Convierte el argumento recibido a entero.
3. Valida que N sea mayor que cero.
4. Reserva memoria dinámica para almacenar la secuencia.
5. Inicializa la estructura `datos_fib_t`.
6. Crea el hilo trabajador mediante `pthread_create()`.
7. Espera la finalización del trabajador mediante `pthread_join()`.
8. Imprime la secuencia generada.
9. Libera la memoria reservada con `free()`.
10. Finaliza el programa.

**Valor de retorno:**

* `0` si la ejecución fue exitosa.
* `1` en caso de error de entrada o asignación de memoria.

---

### 2.2. Problemas presentados durante el desarrollo y soluciones adoptadas

1.  **Desbordamiento numérico (Overflow):**
    * *Problema:* La secuencia de Fibonacci crece muy rápidamente. Debido a que los valores se almacenan utilizando variables de tipo `long long`, existe un límite máximo representable. A partir de aproximadamente F(93), los resultados exceden la capacidad de este tipo de dato y se produce un desbordamiento aritmético.
    * *Solución considerada:* Se evaluó implementar una validación para detectar el overflow y detener el cálculo cuando este ocurriera.
    * *Solución adoptada:* Finalmente se decidió no incorporar dicha validación. La razón es que el objetivo principal de la práctica era comparar el rendimiento de la implementación secuencial y la implementación con Pthreads para tamaños de     entrada grandes, como N = 200000. Si el programa detuviera la ejecución al detectar el overflow, ambos algoritmos terminarían alrededor del término 93 y no procesarían la totalidad de los elementos solicitados, haciendo imposible realizar una   comparación significativa de tiempos. Por este motivo se mantuvo la implementación original, aceptando que los valores posteriores al desbordamiento no son matemáticamente correctos, pero permitiendo evaluar el comportamiento temporal de los algoritmos sobre una carga de trabajo grande.

2.  **Pérdida de datos por retorno incorrecto de variables locales (Stack vs Heap):**
    * *Problema:* Inicialmente, al intentar devolver el resultado de la suma parcial desde el hilo (`worker`) usando la dirección de una variable local, el hilo principal (`main`) leía datos corruptos ("basura") debido a que la pila (stack) de la función del hilo se destruye al finalizar su ejecución.
    * *Solución:* Se reservó memoria dinámicamente en el *Heap* mediante `malloc(sizeof(double))` dentro de cada hilo para almacenar el resultado. El hilo principal recibe este puntero a través de `pthread_join`, lee el valor de forma segura y posteriormente libera la memoria con `free()`.

3.  **Condiciones de carrera en variables compartidas (Race Conditions):**
    * *Problema:* Si múltiples hilos intentaban actualizar de manera simultánea una única variable global `sum` en cada iteración del bucle, los accesos concurrentes provocaban inconsistencias, dando como resultado un valor erróneo de Pi y reduciendo drásticamente el rendimiento.
    * *Solución:* Se rediseñó el algoritmo para que cada hilo trabaje con su propia variable privada `local_sum`. No existe comunicación ni interferencia entre hilos durante el bucle principal. La consolidación se realiza al final de forma secuencial en el hilo principal (patrón de Reducción).

4.  **Desbalance de carga debido al residuo numérico (`n % T != 0`):**
    * *Problema:* Cuando el número de iteraciones `n` no es perfectamente divisible por el número de hilos `T`, se corre el riesgo de ignorar las últimas iteraciones restantes o de sobrecargar injustamente al último hilo, afectando la precisión matemática o la eficiencia.
    * *Solución:* Se implementó una técnica de distribución estática del residuo (`long rem = n % T`). Durante la inicialización de los rangos, los primeros hilos reciben un tamaño de bloque igual a `(n / T) + 1`, mientras que los hilos restantes reciben exactamente `n / T`. Esto cubre el 100% del espacio de iteración.

---
# 3. Pruebas realizadas

## Prueba 1: Generación correcta de la secuencia

**Entrada:**
```bash
./fibonacci 15
```

**Resultado esperado:**
```text
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```

**Resultado obtenido:**
La salida coincidió exactamente con la secuencia esperada.

**Conclusión:**
La implementación genera correctamente los primeros 15 términos de la secuencia de Fibonacci.

---

## Prueba 2: Validación de argumentos

**Entrada:**
```bash
./fibonacci
```

**Resultado esperado:**
Mostrar mensaje de uso.

**Resultado obtenido:**
El programa informó correctamente la forma de utilización.

**Conclusión:**
La validación de parámetros funciona correctamente.
---

## Prueba 3: Validación de valores no válidos

**Entrada:**
```bash
./fibonacci 0
```

**Resultado esperado:**
Mostrar mensaje indicando que N debe ser mayor que cero.

**Resultado obtenido:**
La validación se ejecutó correctamente.

**Conclusión:**
El programa maneja adecuadamente entradas inválidas.
---

## Prueba 4: Comparación de rendimiento

**Configuración:**
N = 200000

**Resultados obtenidos:**
* Tiempo secuencial: 0.004852 s
* Tiempo Pthreads: 0.018617 s

**Conclusión:**
La versión secuencial resultó más rápida debido a que el problema no aprovecha paralelismo real y la implementación con hilos introduce sobrecarga adicional.
---

* **Prueba de Consistencia Numérica:** Se ejecutaron de forma comparativa los ejecutables `pi_s` y `pi_p` bajo la misma configuración base (`n = 2000000000L`). Se comprobó que ambas alternativas retornaran exactamente el mismo valor de punto flotante de alta precisión (`PI = 3.141592653589793`), verificando que la lógica matemática no sufrió alteraciones por la descomposición paralela.
* **Prueba de Rendimiento y Escalabilidad (Speedup):** Se validó el software paralelo `pi_p` incrementando paulatinamente el número de hilos (`T = 1, 2, 4, 8`). Se constató experimentalmente que el tiempo reportado en la línea `TIME` disminuía de forma inversamente proporcional al número de hilos (aceleración casi lineal) hasta alcanzar el límite físico de núcleos de procesamiento del hardware.
* **Prueba de Robustez ante Entradas Inválidas:** Se evaluó el comportamiento del programa frente a errores del usuario, tales como omitir los parámetros requeridos o ingresar valores de hilos menores o iguales a cero (ej. `./pi_p 0`). El código interceptó adecuadamente estas anomalías, desplegando el mensaje instructivo en `stderr` y finalizando limpiamente con un código de salida `1`.

# 4. Conclusiones
1. La biblioteca Pthreads permite implementar concurrencia mediante la creación y administración de hilos dentro de un mismo proceso, compartiendo el mismo espacio de memoria.
2. El uso de una estructura de datos para transferir información al hilo trabajador simplifica el envío de múltiples parámetros y mejora la organización del código.
3. La función `pthread_join()` es fundamental para garantizar la sincronización entre hilos y evitar que el hilo principal acceda a resultados incompletos.
4. Tanto la versión secuencial como la versión con Pthreads presentan una complejidad temporal O(N), ya que cada término de la secuencia se calcula una sola vez.
5. El uso de hilos no garantiza una mejora de rendimiento. Cuando no existe paralelismo efectivo, la sobrecarga asociada a la creación y sincronización de hilos puede hacer que la solución concurrente sea más lenta que la secuencial.
6. Los resultados experimentales mostraron que la versión con Pthreads fue aproximadamente 3.83 veces más lenta que la versión secuencial para N = 200000.
7. El crecimiento acelerado de la secuencia de Fibonacci provoca desbordamiento en variables de tipo `long long`; sin embargo, mantener la implementación sin control de overflow permitió realizar una evaluación adecuada del rendimiento para tamaños de entrada grandes.

