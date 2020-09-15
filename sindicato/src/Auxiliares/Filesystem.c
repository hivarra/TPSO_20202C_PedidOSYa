/*
 * Filesystem.c
 *
 *  Created on: 13 sep. 2020
 *      Author: utnso
 */


#include "Filesystem.h"

#define BLOCK_SIZE 64
#define BLOCKS 5192
#define MAGIC_NUMBER "AFIP"

void montarFileSystem() {

//	if(!existeDirectorio(sindicato_conf.punto_montaje_afip)) {

//		crearDirectorio(gamecard_conf.punto_montaje_tallgrass);
//		crearMetadataGlobal();
//		crearBitmap();
//		crearDirectorioFiles();
//		crearDirectorioBloques();
//		generarBloques();

		log_info(logger, "File System: Creación finalizada");

//	} else {

//		ruta_files = string_new();
//		string_append(&ruta_files, gamecard_conf.punto_montaje_tallgrass);
//		string_append(&ruta_files, "/Files/");
//
//		char* rutaBitmap = string_new();
//		string_append(&rutaBitmap, gamecard_conf.punto_montaje_tallgrass);
//		string_append(&rutaBitmap, "/Metadata");
//		string_append(&rutaBitmap, "/Bitmap.bin");
//
//		ruta_bloques = string_new();
//		string_append(&ruta_bloques, gamecard_conf.punto_montaje_tallgrass);
//		string_append(&ruta_bloques, "/Blocks/");
//
//		tamanio_bloques = gamecard_conf.block_size;
//		cantidad_bloques = gamecard_conf.blocks;

		log_info(logger, "File System: Ya existe");
	}

//}

void crearDirectorio(char *path) {

	struct stat st = {0};

	if(stat(path, &st) == -1 ) {
		if(mkdir(path, 0777) == -1) {
			perror("mkdir");
			log_error(logger, "CREAR_DIR: ERROR. PERMISOS %s", path);
		} else {
			crearMetadataDirectorio(path);
		}
	} else {
//		log_info(logger, "CREAR_DIR: WARNING. YA EXISTE %s", path);
	}

}

void crearMetadataDirectorio(char* ruta) {

	char* ruta_archivo = string_from_format("%s/Metadata.bin", ruta );
//	log_info(logger, "%s", ruta_archivo);
	FILE* fp = fopen(ruta_archivo, "w");
	char* contenido = string_new();
	string_append(&contenido, "DIRECTORY=Y");

//	log_info(logger, "Contenido Metadata: %s", contenido);

	fputs(contenido, fp);
	free(contenido);
	free(ruta_archivo);
	fclose(fp);

}
