
#ifndef AUXILIARES_FILESYSTEM_H_
#define AUXILIARES_FILESYSTEM_H_

#include "../sindicato.h"
#include "shared.h"
#include <commons/bitarray.h>

typedef struct {
	uint32_t initial_block;
	uint32_t size;
} t_metadata;

typedef struct{
	char* nombreRestaurante;
	char* cantidadCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	char* cantidadHornos;
}t_crear_restaurante;

typedef struct{
	char* nombre;
	char* pasos;
	char* tiempoPasos;
}t_crear_receta;

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
void crearRestaurante(t_crear_restaurante* argsCrearRestaurante);
void crearReceta(t_crear_receta* argsCrearReceta);
void generarBloques();
int existeDirectorio(char *path);


#endif /* AUXILIARES_FILESYSTEM_H_ */
