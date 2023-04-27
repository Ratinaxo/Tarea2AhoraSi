#include "Map.h"
#include "stack.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  char accion;  // 'a' para agregar item, 'e' para eliminar item, 'p' para agregar ph
  int puntos;   // cantidad de puntos de habilidad agregados o eliminados
  void *dato;   // item agregado o eliminado, o NULL si se agregaron puntos de habilidad
} Accion;

typedef struct {

  char *nombre;
  int ph;
  int cantItems;
  Map *inventario;
  Stack *accionJ;
  //char ultimaAccion; 
} jugador;

int is_equal_string(void *key1, void *key2) {
  if (strcmp((char *)key1, (char *)key2) == 0)
    return 1;
  return 0;
}

int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

void menuTexto(int *opcion) {

  printf("\nElija una opcion para continuar:\n");
  printf("\n1. Crear perfil.\n");
  printf("2. Mostrar perfil.\n");
  printf("3. Agregar item.\n");
  printf("4. Eliminar item.\n");
  printf("5. Agregar puntos de habilidad.\n");
  printf("6. Mostrar jugadores con item especifico.\n");
  printf("7. Deshacer última acción.\n");
  printf("8. Importar datos desde un archivo CSV.\n");
  printf("9. Exportar datos a un archivo CSV.\n");
  // en proceso
  printf("0. Salir del menu.\n");
  /*
    Usar string para comprobar que lo ingresa el usuario es un numero valido
    para nuestro menu
  */
  char comprobar[2];
  while (1) {
    scanf("%s", comprobar);

    if (isdigit(comprobar[0]) && strlen(comprobar) == 1)
    // ver que sea un numero de un digito
    {

      *opcion = atoi(comprobar);
      // ya cuando se comprueba se actualiza opcion
      return;
    }
    printf("Entrada invalida, ingrese una opcion valida entre 0 y 9\n");
  }
} // Importado de Tarea 1. LISTO

void crearPerfil(Map *jugadores) {

  jugador *aux = (jugador *)malloc(sizeof(jugador));
  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();

  aux->nombre = (char *)malloc(sizeof(char) * (strlen(nombre) + 1));
  if(aux->nombre == NULL) exit(EXIT_FAILURE);
  
  strcpy(aux->nombre, nombre);
  aux->cantItems = 0;
  aux->ph = 0;
  aux->inventario = createMap(is_equal_string);
  aux->accionJ = stack_create();

  insertMap(jugadores, aux->nombre, aux);
} // LISTO

void mostrarPerfil(Map *jugadores) {
  
  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *info = searchMap(jugadores, nombre);

  if (info == NULL) {
    printf("\nEl jugador no se encuentra registrado\n");
    return;
  }

  printf("\nNombre: %s\n", info->nombre);
  printf("Puntos de Habilidad: %i\n", info->ph);
  printf("\n****Items de %s****\n", info->nombre);

  
  setSortFunction(info->inventario, lower_than_string);
  Map *invJugador = info->inventario;
  //setSortFunction(info->inventario, lower_than_string); // intento de ordenar el mapa de items

  
  char *item = (char *)firstMap(invJugador); 
  // llamamos al primer item que hay en el mapa de inventario del jugador
  if (item == NULL) {
    printf("El jugador no tiene items\n");
    return;
  }
  while (item != NULL) {
    printf("- %s\n", item); 
    item = nextMap(invJugador);
  }
  printf("\n******************\n");
} // LISTO

void agregarItem(Map *jugadores) {
  char item[101], nombre[101]; 
  char *itemD;
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *aux = searchMap(jugadores, nombre);

  if (aux == NULL) {
    puts("El jugador buscado no se encuentra registrado.\n");
    return;
  }
  printf("\nIngrese el nombre del item: ");
  scanf("%[^\n]%*c", item);
  itemD = (char *)malloc(sizeof(char) * (strlen(item) + 1));
  strcpy(itemD, item);
  insertMap(aux->inventario, itemD, itemD);
  aux->cantItems++; // aqui en teoria se agrega un item al mapa de inventario

  Accion *accion = (Accion *)malloc(sizeof(Accion));
  accion->accion = 'a';
  accion->puntos = 0;
  accion->dato = itemD;
  stack_push(aux->accionJ, accion);
  // probando
  
} // LISTO

void eliminarItem(Map *jugadores){
  
  char item[101], nombre[101];
  
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s", nombre);
  getchar();
  
  jugador *aux = searchMap(jugadores, nombre);
  if (aux == NULL){
    puts("El jugador no se encuentra registrado.\n");
    return;
  }
  
  printf("\nIngrese el nombre del item: ");
  scanf(" %[^\n]", item);
  getchar();
  void *itemAux = searchMap(aux->inventario, item);
  if (itemAux == NULL){
    puts("El item no se encuentra en el inventario del jugador.");
    return;
  }
  
  eraseMap(aux->inventario, item);
  aux->cantItems--;
  
  Accion *accion = (Accion *)malloc(sizeof(Accion));
  accion->accion = 'e';
  accion->puntos = 0;
  accion->dato = item;
  stack_push(aux->accionJ, accion);
  
  printf("El item fue eliminado del inventario del jugador");
} // LISTO

void agregarPH(Map *jugadores) {
  
  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *aux = searchMap(jugadores, nombre);
  
  if (aux == NULL){
    puts("El jugador no se encuentra registrado.\n");
    return;
  }
  
  int ptsH;
  printf("\nIngrese cuantos puntos de habilidad desea ingresar: ");
  scanf("%i", &ptsH);
  aux->ph += ptsH;
  stack_push(aux->accionJ, &ptsH);
  
  Accion *accion = (Accion *)malloc(sizeof(Accion));
  accion->accion = 'p';
  accion->puntos = ptsH;
  accion->dato = NULL;
  stack_push(aux->accionJ, accion);

  //testeo
} // LISTO

void mostrarItemEspecifico(Map *jugadores) {
  char item[101];
  
  printf("\nIngrese el nombre del item: ");
  scanf("%[^\n]%*c", item);
  


  
}

void deshacerUltima(Map *jugadores){
  
  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *aux = searchMap(jugadores, nombre);
  
  if (aux == NULL){
    puts("El jugador no se encuentra registrado.\n");
    return;
  }

  Accion *accion = (Accion *)stack_top(aux->accionJ); // tiraba un error por estar declarando last dentro de los casos de switch
  
  switch(accion->accion){
    case 'a': //ultima accion fue agregar(a) item
      eraseMap(aux->inventario, accion->dato); // eliminar item
      aux->cantItems--;
      stack_pop(aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;
    
    case 'e': //ultima accion fue eliminar(e) item
      insertMap(aux->inventario, accion->dato, accion->dato); // agregar item
      aux->cantItems++;
      stack_pop(aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;
    
    case 'p': //ultima accion fue agregar puntos(p) de habilidad
      aux->ph -= accion->puntos; // quitar puntos
      stack_pop(aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;
    
    case 'x': //no existe ultima accion realizada(x)
      printf("\nEste jugador no tiene acciones realizadas %s\n",nombre);
      return;
    break;
  }
} // LISTO 

void importarArchivo(Map *jugadores) {
  /*char archivo[100];
  printf("Ingrese el nombre del archivo CSV:\n");
  scanf("%s", archivo);
  FILE *csv = fopen(archivo, "r");

  if (csv == NULL){
    printf("Error al leer archivo\n");
    return;
  }

  char buffer[1024];

  */
} // en DESARROLLO

void exportarArchivo(Map *jugadores) {
  // ARREGLAR CASO DE MUCHOS ITEMS
  // test.csv es de prueba, falta agregar lectura de nombre
  FILE *fp = fopen("test.csv", "w");
  if (fp == NULL) {
    printf("Error al abrir/crear archivo %s\n", "test.csv");
    return;
  }
  // encabezados/headers
  fprintf(fp, "nombre,ph,item1,item2,item3\n");

  void *key = firstMap(jugadores);
  while (key != NULL) {
    jugador *info = (jugador *)searchMap(jugadores, key);

    fprintf(fp, "%s,%i,", info->nombre, info->ph);

    Map *invJugador = info->inventario;
    void *item = firstMap(invJugador);
    while (item != NULL) {
      fprintf(fp, "%s,", (char *)item);
      item = nextMap(invJugador);
    }

    fprintf(fp, "\n");

    key = nextMap(jugadores);
  }
  printf("Se ha exportado correctamente\n\n");
  fclose(fp);
} // en DESARROLLO

int main() {
  int opcion;
  Map *jugadores = createMap(is_equal_string);

  printf("~~~~~~BIENVENIDO AL MENU DE JUGADOR~~~~~~\n\n");
  while (1) {
    menuTexto(&opcion);

    switch (opcion) {
    case 1:
      printf("\n--------------------------------------------\n");
      crearPerfil(jugadores);
      printf("\n--------------------------------------------\n");

      break;
    case 2:
      printf("\n--------------------------------------------\n");
      mostrarPerfil(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 3:
      // printf("¿Que item deasea agregar?"); // lo comento pq estoy probando lo
      // de la pila para el deshacer :)
      printf("\n--------------------------------------------\n");
      agregarItem(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 4:
      printf("\n--------------------------------------------\n");
      eliminarItem(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 5:
      printf("\n--------------------------------------------\n");
      agregarPH(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 6:
      printf("\n--------------------------------------------\n");
      mostrarItemEspecifico(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 7:
      printf("\n--------------------------------------------\n");
      deshacerUltima(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 8:
      printf("\n--------------------------------------------\n");
      importarArchivo(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 9:
      printf("\n--------------------------------------------\n");
      exportarArchivo(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 0:

      exit(EXIT_SUCCESS);

    default:
      printf("Elija una opcion del menu para continuar:\n\n");
    }
  }

  return 0;
}
