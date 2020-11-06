
#ifndef AUXILIARES_FILESYSTEM_H_
#define AUXILIARES_FILESYSTEM_H_

typedef struct{
	int initial_block;
	int size;
}t_metadata;

#include <sys/mman.h>
#include <fcntl.h>
#include <commons/bitarray.h>
#include "../sindicato.h"

pthread_mutex_t mutex_bitmap;
int tamanio_bloques;
int cantidad_bloques;
char* bmap;
t_bitarray *bitmap;
char* ruta_files;
char* ruta_bloques;
char* ruta_restaurantes;
char* ruta_recetas;

void montarFileSystem(void);
int existeDirectorio(char*);
void crearDirectorio(char*);
int existeFile(char*);//Si exite por ej un Restaurante/Info.AFIP
void persistirDatos(char* buffer, int* arrayBloques);
void crearMetadataArchivo(char* path, t_metadata* mData);
t_metadata* leerMetadataArchivo(char* path);
int actualizarSizeMetadataArchivo(char* path, int newSize);

#endif /* AUXILIARES_FILESYSTEM_H_ */
