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

	if (!existeDirectorio(sindicato_conf.punto_montaje)) {

		crearDirectorio(sindicato_conf.punto_montaje);
		crearMetadataGlobal();
		crearBitmap();
		crearDirectorioFiles();
		crearDirectorioRestaurantes();
		crearDirectorioRecetas();
		crearDirectorioBloques();
		generarBloques();

		log_info(logger, "File System: Creación finalizada");

	} else {

		ruta_files = string_new();
		string_append(&ruta_files, sindicato_conf.punto_montaje);
		string_append(&ruta_files, "/Files/");

		char* rutaBitmap = string_new();
		string_append(&rutaBitmap, sindicato_conf.punto_montaje);
		string_append(&rutaBitmap, "/Metadata");
		string_append(&rutaBitmap, "/Bitmap.bin");

		ruta_bloques = string_new();
		string_append(&ruta_bloques, sindicato_conf.punto_montaje);
		string_append(&ruta_bloques, "/Blocks/");

		tamanio_bloques = sindicato_conf.block_size;
		cantidad_bloques = sindicato_conf.blocks;

		log_info(logger, "File System: Ya existe");
	}

}

void crearDirectorio(char *path) {

	struct stat st = { 0 };

	if (stat(path, &st) == -1) {
		if (mkdir(path, 0777) == -1) {
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

	char* ruta_archivo = string_from_format("%s/Metadata.bin", ruta);
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
int existeDirectorio(char *path) {

	struct stat st = { 0 };

	if (stat(path, &st) != -1) {
		log_info(logger, "El directorio %s ya existe", path);
		return 1;
	} else {
		return 0;
	}

}

void crearMetadataGlobal() {

	char* rutaMetadata = string_new();
	string_append(&rutaMetadata, sindicato_conf.punto_montaje);
	string_append(&rutaMetadata, "/Metadata/");

	crearDirectorio(rutaMetadata);
	string_append(&rutaMetadata, "Metadata.bin");

	FILE *file = fopen(rutaMetadata, "w");
	char* buffer = string_new();
	string_append(&buffer, "BLOCK_SIZE=");
	string_append(&buffer, string_itoa(sindicato_conf.block_size));
	tamanio_bloques = sindicato_conf.block_size;
	string_append(&buffer, "\n");

	string_append(&buffer, "BLOCKS=");
	string_append(&buffer, string_itoa(sindicato_conf.blocks));
	cantidad_bloques = sindicato_conf.blocks;
	string_append(&buffer, "\n");

	string_append(&buffer, "MAGIC_NUMBER=");
	string_append(&buffer, sindicato_conf.magic_number);
	string_append(&buffer, "\n");

	fputs(buffer, file);
	log_info(logger, "Metadata global cargada");

	fclose(file);
	free(rutaMetadata);

}

void crearBitmap() {

	char* rutaBitmap = string_new();
	string_append(&rutaBitmap, sindicato_conf.punto_montaje);
	string_append(&rutaBitmap, "/Metadata");
	string_append(&rutaBitmap, "/Bitmap.bin");

	FILE *file = fopen(rutaBitmap, "w");
	fclose(file);
	int fd = open(rutaBitmap, O_RDWR);
	ftruncate(fd, (cantidad_bloques / 8) + 1);
	bmap = mmap(NULL, cantidad_bloques/8, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	bitmap = bitarray_create_with_mode(bmap, cantidad_bloques / 8, MSB_FIRST);
	msync(bmap, fd, MS_SYNC); // Para sincronizar el bitmap con el archivo físico.

	struct stat mystat;
	if (fstat(fd, &mystat) < 0) {
		log_error(logger, "Error al establecer fstat");
	}
	fstat(fd, &mystat);

	log_info(logger, "Bitmap generado");
	close(fd);
	free(rutaBitmap);

}

void crearDirectorioFiles() {

	ruta_files = string_new();
	string_append(&ruta_files, sindicato_conf.punto_montaje);
	string_append(&ruta_files, "/Files/");
	crearDirectorio(ruta_files);
	log_info(logger, "Ruta Files creada.");
}

void crearDirectorioRestaurantes(){

	char* ruta_restaurante = string_new();
	string_append(&ruta_restaurante, ruta_files);
	string_append(&ruta_restaurante, "/Restaurantes/");
	crearDirectorio(ruta_restaurante);
}

void crearDirectorioRecetas(){

	char* ruta_recetas = string_new();
	string_append(&ruta_recetas, ruta_files);
	string_append(&ruta_recetas, "/Recetas/");
	crearDirectorio(ruta_recetas);
}

void crearDirectorioBloques() {
	ruta_bloques = string_new();
	string_append(&ruta_bloques, sindicato_conf.punto_montaje);
	string_append(&ruta_bloques, "/Blocks/");
	crearDirectorio(ruta_bloques);
	log_info(logger, "Ruta Blocks creada.");
}

void generarBloques() {

	for(int i=0; i <= cantidad_bloques; i++) {

		char* nombre_bloque = string_new();
		string_append(&nombre_bloque, ruta_bloques);
		string_append(&nombre_bloque, string_itoa(i));
		string_append(&nombre_bloque, ".afip");

		FILE *fp = fopen(nombre_bloque, "w");
		fclose(fp);

		free(nombre_bloque);
	}
	log_info(logger, "Bloques generados.");
}
