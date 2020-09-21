/*
 * Filesystem.h
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_FILESYSTEM_H_
#define AUXILIARES_FILESYSTEM_H_

#include "../sindicato.h"
#include "shared.h"

typedef struct {
	char directory[200];
	int32_t size;
	char blocks[100];
	char open[10];
} t_metadata;

int tamanio_bloques;
int cantidad_bloques;
char* bmap;
t_bitarray *bitmap;
char* ruta_files;
char* ruta_bloques;

void montarFileSystem();
void crearDirectorio(char *path);
void crearMetadataDirectorio(char* ruta);
int existeDirectorio(char *path);
void crearMetadataGlobal();
void crearBitmap();

#endif /* AUXILIARES_FILESYSTEM_H_ */
