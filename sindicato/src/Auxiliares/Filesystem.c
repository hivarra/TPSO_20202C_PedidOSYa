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

	if(!existeDirectorio(sindicato_conf.punto_montaje_afip)) {

//		crearDirectorio(gamecard_conf.punto_montaje_tallgrass);
//		crearMetadataGlobal();
//		crearBitmap();
//		crearDirectorioFiles();
//		crearDirectorioBloques();
//		generarBloques();

		log_info(logger, "File System: Creación finalizada");

	} else {

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

}
