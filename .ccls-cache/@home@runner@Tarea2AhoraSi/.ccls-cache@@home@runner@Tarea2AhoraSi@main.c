#include "Map.h"
#include "stack.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char accion; // 'a' para agregar item, 'e' para eliminar item, 'p' para
               // agregar ph
  int puntos;  // cantidad de puntos de habilidad agregados o eliminados
  void *dato;  // item agregado o eliminado, o NULL si se agregaron puntos de
               // habilidad
} Accion;

typedef struct {

  char *nombre;
  int ph;
  int cantItems;
  Map *inventario;
  Stack *accionJ;

} jugador;

int is_equal_string(void *key1, void *key2) {
  if (strcmp((char *)key1, (char *)key2) == 0)
    return 1;
  return 0;
}

int lower_than_string(void *key1, void *key2) {
  if (strcmp((char *)key1, (char *)key2) < 0)
    return 1;
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
} // Importado de Tarea

void crearPerfil(Map *jugadores) {

  jugador *aux = (jugador *)malloc(sizeof(jugador));
  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();

  aux->nombre = (char *)malloc(sizeof(char) * (strlen(nombre) + 1));
  if (aux->nombre == NULL)
    exit(EXIT_FAILURE);

  strcpy(aux->nombre, nombre);
  aux->cantItems = 0;
  aux->ph = 0;
  aux->inventario = createMap(is_equal_string);
  aux->accionJ = stack_create();

  insertMap(jugadores, aux->nombre, aux);
}

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
}

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

}

void eliminarItem(Map *jugadores) {

  char item[101], nombre[101];

  printf("\nIngrese el nombre del jugador: ");
  scanf("%s", nombre);
  getchar();

  jugador *aux = searchMap(jugadores, nombre);
  if (aux == NULL) {
    puts("El jugador no se encuentra registrado.\n");
    return;
  }

  printf("\nIngrese el nombre del item: ");
  scanf(" %[^\n]", item);
  getchar();
  void *itemAux = searchMap(aux->inventario, item);
  if (itemAux == NULL) {
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
}

void agregarPH(Map *jugadores) {

  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *aux = searchMap(jugadores, nombre);

  if (aux == NULL) {
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

}

void mostrarItemEspecifico(Map *jugadores) {
  char item[101];

  printf("\nIngrese el nombre del item: ");
  scanf(" %[^\n]", item);
  getchar();

  jugador *auxJ = firstMap(jugadores);
  int existe = 0;
  printf("\nJugadores que tienen este item en su inventario: \n");
  while (auxJ != NULL) {
    if (searchMap(auxJ->inventario, item) != NULL) {
      existe = 1;
      printf("- %s\n", auxJ->nombre);
    }
    auxJ = nextMap(jugadores);
  }
  if (existe == 0)
    printf("\n ** No se encontraron jugadores que contengan este item en su "
           "inventario **");
}

void deshacerUltima(Map *jugadores) {

  char nombre[101];
  printf("\nIngrese el nombre del jugador: ");
  scanf("%s[^\n]", nombre);
  getchar();
  jugador *aux = searchMap(jugadores, nombre);

  if (aux == NULL) {
    puts("El jugador no se encuentra registrado.\n");
    return;
  }

  Accion *accion =
      (Accion *)stack_top(aux->accionJ); 

  switch (accion->accion) {
  case 'a': // ultima accion fue agregar(a) item
    eraseMap(aux->inventario, accion->dato); // eliminar item
    aux->cantItems--;
    stack_pop(
        aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;

  case 'e': // ultima accion fue eliminar(e) item
    insertMap(aux->inventario, accion->dato, accion->dato); // agregar item
    aux->cantItems++;
    stack_pop(
        aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;

  case 'p': // ultima accion fue agregar puntos(p) de habilidad
    aux->ph -= accion->puntos; // quitar puntos
    stack_pop(
        aux->accionJ); // se elimina la ultima accion que se realizo de la pila
    break;

  case 'x': // no existe ultima accion realizada(x)
    printf("\nEste jugador no tiene acciones realizadas %s\n", nombre);
    return;
    break;
  }
}

void importarArchivo(Map *jugadores) {
  char filename[100];
  printf("Ingrese el nombre del archivo: ");
  scanf("%s", filename);

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error al abrir el archivo\n");
    return;
  }

  char line[1024];
  char *token;
  while (fgets(line, 1024, fp)) {
    jugador *j = (jugador *)malloc(sizeof(jugador));
    char *linePtr = line;

    // nombre
    token = strsep(&linePtr, ",");
    j->nombre = (char *)malloc(sizeof(char) * (strlen(token) + 1));
    token[strcspn(token, "\r\n")] = 0;
    strcpy(j->nombre, token);

    // puntos habilidad
    token = strsep(&linePtr, ",");
    j->ph = atoi(token);

    // n itmes
    token = strsep(&linePtr, ",");
    int numItems = atoi(token);
    j->cantItems = numItems;

    // agregar items
    j->inventario = createMap(is_equal_string);
    for (int i = 0; i < numItems; i++) {
      token = strsep(&linePtr, ",");
      token[strcspn(token, "\r\n")] = 0;
      char *item = (char *)malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(item, token);
      insertMap(j->inventario, item, item);
    }

    insertMap(jugadores, j->nombre, j);
  }
  fclose(fp);
}

void exportarArchivo(Map *jugadores) {
  char filename[256];
  printf("Ingrese el nombre del archivo: ");
  scanf("%255s", filename);

  FILE *archivo = fopen(filename, "w");
  if (archivo == NULL) {
    printf("Error al abrir el archivo para escritura.\n");
    return;
  }
  fprintf(archivo, "Nombre,Puntos de habilidad,#items,Item 1,Item 2,Item "
                   "3,Item 4,Item 5,Item 6,Item 7,Item 8\n");

  void *jugador_actual = firstMap(jugadores);
  while (jugador_actual != NULL) {
    jugador *info = (jugador *)jugador_actual;
    fprintf(archivo, "%s,%d,%d", info->nombre, info->ph, info->cantItems);
    Map *invJugador = info->inventario;
    void *item_actual = firstMap(invJugador);
    int i;
    for (i = 0; i < info->cantItems; i++) {
      char *nombre_item = (char *)item_actual;
      fprintf(archivo, ",%s", nombre_item);
      item_actual = nextMap(invJugador);
    }
    fprintf(archivo, "\n");
    jugador_actual = nextMap(jugadores);
  }
  fclose(archivo);
} 

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
