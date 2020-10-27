
#ifndef AUXILIARES_FILESYSTEM_H_
#define AUXILIARES_FILESYSTEM_H_

#include "../sindicato.h"
#include "shared.h"
#include <commons/bitarray.h>

typedef struct {
	char directory[200];
	int32_t size;
	char blocks[100];
	char open[10];
} t_metadata;

typedef struct{
	char* nombreRestaurante;
	int cantidadCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	int cantidadHornos;
}t_crear_restaurante;

int tamanio_bloques;
int cantidad_bloques;
char* bmap;
t_bitarray *bitmap;
char* ruta_files;
char* ruta_bloques;
char* ruta_restaurantes;
char* ruta_recetas;


void montarFileSystem();
void crearBitmap();
void crearDirectorio(char *path);
void crearDirectorioFiles();
void crearDirectorioRestaurantes();
void crearDirectorioRecetas();
void crearDirectorioBloques();
void crearMetadataDirectorio(char* ruta);
void crearMetadataGlobal();
void crearRestaurante();
void generarBloques();
int existeDirectorio(char *path);


#endif /* AUXILIARES_FILESYSTEM_H_ */
