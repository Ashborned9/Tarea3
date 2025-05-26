#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include <string.h>
#include <math.h>
#define MAX_ESCENARIOS 17

typedef struct {
    char nombre[64];
    int peso;
    int valor;
} Item;
typedef struct {
    int id;
    char NombreS[64];
    char DescripcionS[256];
    List *Items;
    int Vecinos[4];
    int FINAL;
} Habitacion;

Habitacion Habitaciones[MAX_ESCENARIOS];
typedef struct {
    int norte;
    int sur;
    int este;
    int oeste;
} Cardinales;

typedef struct{
    int Tiempo_Vida;
    int Puntaje_Total;
    int PesoTTL;
    Habitacion *Habitaciones;
    List *Items;
    int posicionActual;
} Player;

void *ItemCreate(char* nombreItm, int PesoItm, int ValorItm){
  Item *new_Item = malloc(sizeof(Item));
    if (new_Item == NULL) {
        perror("Error al crear el item");
        return NULL;
    }
    new_Item->peso = PesoItm;
    new_Item->valor = ValorItm;
    strcpy(new_Item->nombre, nombreItm);
    return new_Item;
}

// Función para leer una línea del archivo CSV
void leerEscenarios()
{
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char **fields;
    fields = leer_linea_csv(archivo, ',');
    
    free(fields);
    // Leer las habitaciones del archivo CSV
    while ((fields = leer_linea_csv(archivo, ',')) != NULL) {
        Habitacion nuevaHabitacion;
        nuevaHabitacion.id = atoi(fields[0]);
        strcpy(nuevaHabitacion.NombreS, fields[1]);
        strcpy(nuevaHabitacion.DescripcionS, fields[2]);

        nuevaHabitacion.Items = list_create();
        nuevaHabitacion.FINAL = (fields[8][0] == 'S' || fields[8][0] == 's');
        // Procesar los items de la habitación
        List *listaItems = split_string(fields[3], ";");
        for (char *itemStr = list_first(listaItems); 
             itemStr != NULL; 
             itemStr = list_next(listaItems))
        {
            List *ValoresItems = split_string(itemStr, ",");
            char *nombreItem = list_first(ValoresItems);
            int pesoItem     = atoi(list_next(ValoresItems));
            int valorItem    = atoi(list_next(ValoresItems));
            
            Item *nuevoItem = ItemCreate(nombreItem, pesoItem, valorItem);
            list_pushBack(nuevaHabitacion.Items, nuevoItem);
            
            list_clean(ValoresItems);
            free(ValoresItems);
        }
        list_clean(listaItems);
        free(listaItems);
        // Procesar los vecinos de la habitación
        nuevaHabitacion.Vecinos[0] = atoi(fields[4]);
        nuevaHabitacion.Vecinos[1] = atoi(fields[5]);
        nuevaHabitacion.Vecinos[2] = atoi(fields[6]);
        nuevaHabitacion.Vecinos[3] = atoi(fields[7]);
        Habitaciones[nuevaHabitacion.id] = nuevaHabitacion;
        free(fields);
    }
    // Cerrar el archivo
    fclose(archivo);
    printf("Habitaciones cargadas correctamente.\n");
    
}

void recogerItem(Player *player) {
    // Verificar si hay items en la habitación actual
    Habitacion *act = &player->Habitaciones[player->posicionActual];
    List *itms = act->Items;
    
    if (list_first(itms) == NULL) {
        printf("No hay ítems en esta habitación.\n");
        presioneTeclaParaContinuar();
        limpiarPantalla();
        return;
    }
    // Mostrar los items disponibles en la habitación
    printf("Items disponibles en esta Sala:\n");
    for (Item *aux = list_first(itms); aux != NULL; aux = list_next(itms)) {
        printf("Nombre: %s, Peso: %d, Valor: %d\n", 
               aux->nombre, 
               aux->peso, 
               aux->valor);
    }
    // Solicitar al jugador el nombre del item a recoger
    printf("Ingrese el nombre del item a recoger: ");
    char nombreItem[64];
    scanf(" %63[^\n]", nombreItem);
    // Buscar el item en la lista de items de la habitación
    for(Item *aux = list_first(itms); aux != NULL; aux = list_next(itms)) {
        if (strcmp(aux->nombre, nombreItem) == 0) {
            if (player->PesoTTL + aux->peso <= 10) { // Suponiendo un peso máximo de 10
                list_pushBack(player->Items, aux);
                list_popCurrent(itms);
                player->PesoTTL += aux->peso; // Actualizar el peso total del jugador
                player->Puntaje_Total += aux->valor; // Actualizar el puntaje total del jugador
                player->Tiempo_Vida -= 1; // Restar tiempo de vida al recoger un item
                // Verificar si el tiempo de vida se ha agotado
                if (player->Tiempo_Vida <= 0) {
                    printf("¡AMBICIOSO! ¡Se ha acabado tu tiempo! Has perdido.\n");
                    presioneTeclaParaContinuar();
                    exit(0);
                }
                else {
                    printf("Felicidades! Has obtenido un %s.\n", aux->nombre);
                }

            } else {
                printf("No puedes recoger el item, te pesa mucho el cuerpo.\n");
            }
            return;
        }
    }
    printf("Item no encontrado en la sala.\n");
}
// Función para descartar un item del inventario del jugador
void descartarItem(Player *player) {
    // Verificar si el jugador tiene items en su inventario
    if (list_first(player->Items) == NULL) {
        printf("No tienes items en tu inventario.\n");
        presioneTeclaParaContinuar();
        limpiarPantalla();
        return;
    }
    // Mostrar los items en el inventario del jugador
    printf("Items en tu inventario:\n");
    for (Item *aux = list_first(player->Items); aux != NULL; aux = list_next(player->Items)) {
        printf("Nombre: %s, Peso: %d, Valor: %d\n", 
               aux->nombre, 
               aux->peso, 
               aux->valor);
    }
    printf(" ========= C U I D A D O =========\n");
    printf("Recuerda que al descartar un item, NO PUEDES volver a recogerlo.\n");
    printf("Esta acción te quita tiempo de vida.\n");
    printf("===================================\n");
    // Solicitar al jugador el nombre del item a descartar
    printf("Ingrese el nombre del item a descartar: ");
    char nombreItem[64];
    scanf(" %63[^\n]", nombreItem);
    // Buscar el item en el inventario del jugador y eliminarlo si se encuentra
    for(Item *aux = list_first(player->Items); aux != NULL; aux = list_next(player->Items)) {
        if (strcmp(aux->nombre, nombreItem) == 0) {
            list_popCurrent(player->Items);
            player->PesoTTL -= aux->peso;
            player->Puntaje_Total -= aux->valor;
            player->Tiempo_Vida -= 1; // Restar tiempo de vida al descartar un item
            
            // Verificar si el tiempo de vida se ha agotado
            if (player->Tiempo_Vida <= 0) {
                printf(" ¿QUE HACES? ¡Se ha acabado tu tiempo! Has perdido.\n");
                presioneTeclaParaContinuar();
                exit(0);
            }
            else {
                printf("Has descartado el item %s.\n", aux->nombre);
            }
            return;
        }
    }
    printf("Item no encontrado en tu inventario.\n");
}
void AvanzarEtapa(Player *jugador) {
    // Verificar si el jugador tiene items en su inventario
    int idHabitacionActual = jugador->posicionActual;
    int idHabitacionSiguiente = -1;
    // Calcular el coste de avanzar basado en el peso total del jugador
    int coste = (int)ceil(((double)jugador->PesoTTL + 1) / 10.0);
    jugador->Tiempo_Vida -= coste;
    // Verificar si el tiempo de vida se ha agotado
    if (jugador->Tiempo_Vida <= 0) {
        printf(" Se ha acabado tu tiempo al avanzar! Has perdido.\n");
        printf("Saliendo del juego...\n");
        presioneTeclaParaContinuar();
        exit(0);
    }
    // Mostrar las puertas disponibles en la habitación actual
    printf("Las puertas disponibles son:\n");
    printf("==========================================\n");
    if (Habitaciones[idHabitacionActual].Vecinos[0] != -1) {
        printf("Norte ");
    }
    if (Habitaciones[idHabitacionActual].Vecinos[1] != -1) {
        printf("Sur ");
    }
    if (Habitaciones[idHabitacionActual].Vecinos[2] != -1) {
        printf("Este ");
    }
    if (Habitaciones[idHabitacionActual].Vecinos[3] != -1) {
        printf("Oeste ");
    }
    printf("\n");
    printf("==========================================\n");
    printf("Seleccione la dirección a la que desea avanzar:\n");
    printf("1) Norte\n");
    printf("2) Sur\n");
    printf("3) Este\n");
    printf("4) Oeste\n");
    
    int opcion;
    scanf("%d", &opcion);
    // Validar la opción ingresada
    // Si la opción es válida, asignar el id de la habitación siguiente
    switch (opcion) {
        case 1:
            idHabitacionSiguiente = Habitaciones[idHabitacionActual].Vecinos[0];
            break;
        case 2:
            idHabitacionSiguiente = Habitaciones[idHabitacionActual].Vecinos[1];
            break;
        case 3:
            idHabitacionSiguiente = Habitaciones[idHabitacionActual].Vecinos[2];
            break;
        case 4:
            idHabitacionSiguiente = Habitaciones[idHabitacionActual].Vecinos[3];
            break;
        default:
            printf("Opción no válida.\n");
            return;
    }
    // Verificar si la habitación siguiente es válida
    if (idHabitacionSiguiente != -1) {
        jugador->posicionActual = idHabitacionSiguiente;
        Habitacion *act = &jugador->Habitaciones[jugador->posicionActual];
        printf("Te has movido a la habitación: %s\n", act->NombreS);
        // Mostrar si es la habitación final
        if (act->FINAL) {
            printf("\n¡¡VICTORIA!! Has escapado con vida.\n");
            printf("Puntaje final: %d\n", jugador->Puntaje_Total);
            printf("Peso total transportado: %d\n", jugador->PesoTTL);
            printf("Inventario final:\n");
            for (Item *it = list_first(jugador->Items); it; it = list_next(jugador->Items)) {
                printf("  - %s (valor: %d, peso: %d)\n", it->nombre, it->valor, it->peso);
            }
            presioneTeclaParaContinuar();
            exit(0);
        }
    } else {
        printf("No puedes avanzar en esa dirección.\n");
    }
}


void IniciarJugador(Player *jugador) {
    // Inicializar el jugador
    jugador->Tiempo_Vida = 10;
    jugador->Puntaje_Total = 0;
    jugador->PesoTTL = 0;
    jugador->Items = list_create();
    jugador->Habitaciones = Habitaciones;
    jugador->posicionActual = 1;
}

void MostrarEstadoActual(Player *jugador)
{
    // Mostrar el estado actual del jugador
    printf("=========================================\n");
    printf("Estado Actual:\n");
    printf("==========================================\n");
    // Mostrar la habitación actual y sus detalles
    Habitacion *act = &jugador->Habitaciones[jugador->posicionActual]; 
    printf("Habitación Actual: %s\n", act->NombreS);
    printf("Descripción: %s\n", act->DescripcionS);
    printf("Tiempo de Vida: %d\n", jugador->Tiempo_Vida);
    printf("Puntaje Total: %d\n", jugador->Puntaje_Total);
    printf("Peso Total: %d\n", jugador->PesoTTL);
    printf("Items Recogidos:\n");
    for (Item *it = list_first(jugador->Items); it != NULL; it = list_next(jugador->Items)) {
        printf("- %s (Peso: %d, Valor: %d)\n", it->nombre, it->peso, it->valor);
    }
}

void reiniciarPartida(Player *jugador) {
    // Reiniciar el jugador a su estado inicial
    leerEscenarios();
    IniciarJugador(jugador);
    printf(" Partida reiniciada. ¡Buena suerte!\n");
}
int main() {
    // Inicializar el juego
    leerEscenarios();
    Player jugador;
    IniciarJugador(&jugador);
    // Mostrar el menu de opciones
    while (jugador.Tiempo_Vida > 0) {
        MostrarEstadoActual(&jugador);
        puts("========================================");
        puts("Seleccione una opción:");
        puts("========================================");
        puts("1) Recoger Items");
        puts("2) Descartar Items");
        puts("3) Avanzar a la siguiente etapa");
        puts("4) Reiniciar Partida");
        puts("5) Salir");
        puts("========================================");
        
        printf("Ingrese su opción: \n");
        char opcion;
        scanf(" %c", &opcion);
        switch (opcion) {
        case '1':
            // Recoger Items
            limpiarPantalla();
            recogerItem(&jugador);
          break;
        case '2':
            // Descartar Items
            limpiarPantalla();
            descartarItem(&jugador);
          break;
        case '3':
            // Avanzar a la siguiente etapa
            limpiarPantalla();
            AvanzarEtapa(&jugador);
          break;
        case '4':
           // reiniciar partida
            limpiarPantalla();
            printf("Reiniciando partida...\n");
            reiniciarPartida(&jugador);
            presioneTeclaParaContinuar();
            limpiarPantalla();
          break;
        case '5':
            printf("Saliendo del juego...\n");
            jugador.Tiempo_Vida = 0;
            break;
        default:
        printf("Opción no válida. Intente nuevamente.\n");
        }
        presioneTeclaParaContinuar();
        limpiarPantalla();
  } 
  return 0;
}
