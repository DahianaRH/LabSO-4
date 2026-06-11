# LabSO-4
Laboratorio 4 de Sistemas Operativos enfocado en API de Hilos

## Members
- Sebastian Andres Berrio Murillo, CC. 1066753315, sebastian.berriom@udea.edu.co.
- Sandy Dahiana Ruiz Higuita, CC. 1028031733, sandy.ruizh@udea.edu.co

## Video de sustentación:
link: https://youtu.be/sUv3-Pu4esg

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

## Problema: Desbordamiento numérico (Overflow)
La secuencia de Fibonacci crece muy rápidamente. Debido a que los valores se almacenan utilizando variables de tipo `long long`, existe un límite máximo representable. A partir de aproximadamente F(93), los resultados exceden la capacidad de este tipo de dato y se produce un desbordamiento aritmético.

### Solución considerada
Se evaluó implementar una validación para detectar el overflow y detener el cálculo cuando este ocurriera.

### Solución adoptada
Finalmente se decidió no incorporar dicha validación.
La razón es que el objetivo principal de la práctica era comparar el rendimiento de la implementación secuencial y la implementación con Pthreads para tamaños de entrada grandes, como N = 200000.
Si el programa detuviera la ejecución al detectar el overflow, ambos algoritmos terminarían alrededor del término 93 y no procesarían la totalidad de los elementos solicitados, haciendo imposible realizar una comparación significativa de tiempos.
Por este motivo se mantuvo la implementación original, aceptando que los valores posteriores al desbordamiento no son matemáticamente correctos, pero permitiendo evaluar el comportamiento temporal de los algoritmos sobre una carga de trabajo grande.

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

# 4. Conclusiones
1. La biblioteca Pthreads permite implementar concurrencia mediante la creación y administración de hilos dentro de un mismo proceso, compartiendo el mismo espacio de memoria.
2. El uso de una estructura de datos para transferir información al hilo trabajador simplifica el envío de múltiples parámetros y mejora la organización del código.
3. La función `pthread_join()` es fundamental para garantizar la sincronización entre hilos y evitar que el hilo principal acceda a resultados incompletos.
4. Tanto la versión secuencial como la versión con Pthreads presentan una complejidad temporal O(N), ya que cada término de la secuencia se calcula una sola vez.
5. El uso de hilos no garantiza una mejora de rendimiento. Cuando no existe paralelismo efectivo, la sobrecarga asociada a la creación y sincronización de hilos puede hacer que la solución concurrente sea más lenta que la secuencial.
6. Los resultados experimentales mostraron que la versión con Pthreads fue aproximadamente 3.83 veces más lenta que la versión secuencial para N = 200000.
7. El crecimiento acelerado de la secuencia de Fibonacci provoca desbordamiento en variables de tipo `long long`; sin embargo, mantener la implementación sin control de overflow permitió realizar una evaluación adecuada del rendimiento para tamaños de entrada grandes.
