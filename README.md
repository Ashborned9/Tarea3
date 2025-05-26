# Tarea3

## Descripción

GraphQuest es un pequeño juego de exploración de laberintos basado en un grafo. Cada sala (nodo) tiene:

- **ID**, nombre y descripción.  
- **Ítems** coleccionables (nombre, peso, valor).  
- **Vecinos** en las cuatro direcciones: Norte, Sur, Este y Oeste.  
- Un flag **FINAL** que indica la salida del laberinto

El jugador dispone de un tiempo inicial de 10 unidades, que se consume al:

1. Recoger o descartar ítems (–1 tiempo cada vez).  
2. Avanzar de sala en sala:  
El objetivo es llegar a la sala marcada como salida ("Si" en el CSV) antes de agotar el tiempo, recogiendo tantos ítems de valor como sea posible.

---

## Requisitos

- GCC (o cualquier compilador C con soporte `-lm`)  
- Biblioteca estándar C, más los módulos:
  - `tdas/list.h` / `list.c`  
  - `tdas/heap.h` / `heap.c`  
  - `tdas/extra.h` / `extra.c`  
(Vienen dentro del codigo)
---

## Cómo compilar y ejecutar

1. Abre tu terminal en la carpeta raíz del proyecto (donde esté `tarea3_old.c` y la carpeta `tdas/`).  
2. Compila con:
   ```bash
   gcc -o GraphQuest tarea3_old.c tdas/extra.c tdas/list.c tdas/heap.c -I. -lm
   o
   gcc -o Tarea3 tarea3_old.c tdas/extra.c tdas/list.c tdas/heap.c -I. -lm

3. Ejecutar ./GraphQuest o ./Tarea3 respectivamente

## Menu Principal

Al iniciar el programa, automáticamente se carga el laberinto desde el CSV y se inicia la partida en la sala 1.
Cabe destacar que las funciones de cargar laberinto e iniciar partida , no estan separadas y arrancan juntas al entrar al programa.

## Menu Opciones

Tras la carga, cada turno muestra:
*Estado Actual* 
(En las indicaciones pide que se muestren las opcines para moverse
pero lo encuentro repetitivo y es mas ordenado que aparezca al apretar 
**Avanzar a la siguiente etapa**)

-Habitación y descripción
-Tiempo restante
-Puntaje acumulado
-Peso total del inventario
-Lista de ítems recogidos

*Opciones*
-Recoger Ítem(s)
-Descartar Ítem(s)
-Avanzar a la siguiente etapa
-Reiniciar Partida
-Salir


## Funciones

Comienza: ves “Habitación Actual: Entrada principal”.

Eliges 1) Recoger Ítem(s). Si la sala no tiene ítems recibes “No hay ítems en esta habitación.”

Eliges 3) Avanzar a la siguiente etapa, seleccionas “Sur” (opción 2) para ir a “Cocina”.

Es eleccion propia elegir si tomar los items o no, pero ten en cuenta que cada accion cuesta tiempo.

Si te quedas sin tiempo te saldrá un mensaje de derrota.
Por el contrario y si llegas a la salida, te saldra

¡¡VICTORIA!! Has escapado con vida.
Puntaje final: 42
Peso total transportado: 8
Inventario final:
  - Linterna mágica (valor: 5, peso: 2)
  - Libro antiguo (valor: 10, peso: 3)

## Problemas conocidos

La funcion de apretartecla para continuar puede fallar, por lo tanto si no avanza aprete una tecla hasta que pase a la siguiente pestaña
si no ocurre luego de (2 a 4) simplemente reinicie el programa.
