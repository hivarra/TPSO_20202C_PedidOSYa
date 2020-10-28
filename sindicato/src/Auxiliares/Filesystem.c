#include "Filesystem.h"

#define BLOCK_SIZE 64
#define BLOCKS 5192
#define MAGIC_NUMBER "AFIP"

void leerBitmap(){
	char* rutaBitmap = string_new();
	string_append(&rutaBitmap, sindicato_conf.punto_montaje);
	string_append(&rutaBitmap, "/Metadata");
	string_append(&rutaBitmap, "/Bitmap.bin");

	int fd = open(rutaBitmap, O_RDWR);
	bmap = mmap(NULL, cantidad_bloques / 8, PROT_WRITE | PROT_READ, MAP_SHARED,
			fd, 0);
	bitmap = (void*)&bmap;
	msync(bmap,cantidad_bloques / 8, MS_SYNC); // Para sincronizar el bitmap con el archivo físico.

	struct stat mystat;
	if (fstat(fd, &mystat) < 0) {
		log_error(logger, "Error al establecer fstat");
	}
	fstat(fd, &mystat);

	log_info(logger, "Bitmap leido");
	close(fd);
	free(rutaBitmap);
}
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
		/*TODO:BUG:SI YA EXISTE, DEBE ASIGNAR EL BITMAP YA CREADO*/
		ruta_files = string_new();
		string_append(&ruta_files, sindicato_conf.punto_montaje);
		string_append(&ruta_files, "/Files/");

		leerBitmap();

//		char* rutaBitmap = string_new();
//		string_append(&rutaBitmap, sindicato_conf.punto_montaje);
//		string_append(&rutaBitmap, "/Metadata");
//		string_append(&rutaBitmap, "/Bitmap.bin");

		ruta_bloques = string_new();
		string_append(&ruta_bloques, sindicato_conf.punto_montaje);
		string_append(&ruta_bloques, "/Blocks/");

		tamanio_bloques = sindicato_conf.block_size;
		cantidad_bloques = sindicato_conf.blocks;

		log_info(logger, "File System: Ya existe");
	}

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
	bmap = mmap(NULL, cantidad_bloques / 8, PROT_WRITE | PROT_READ, MAP_SHARED,
			fd, 0);
	bitmap = bitarray_create_with_mode(bmap, cantidad_bloques / 8, MSB_FIRST);
	msync(bmap, cantidad_bloques / 8, MS_SYNC); // Para sincronizar el bitmap con el archivo físico.

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

void crearDirectorioRestaurantes() {

	ruta_restaurantes = string_new();
	string_append(&ruta_restaurantes, ruta_files);
	string_append(&ruta_restaurantes, "/Restaurantes/");
	crearDirectorio(ruta_restaurantes);
}

void crearDirectorioRecetas() {

	ruta_recetas = string_new();
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

void crearDirectorio(char *path) {

	struct stat st = { 0 };

	if (stat(path, &st) == -1) {
		if (mkdir(path, 0777) == -1) {
			perror("mkdir");
			log_error(logger, "CREAR_DIR: ERROR. PERMISOS %s", path);
		}
	} else {
//		log_info(logger, "CREAR_DIR: WARNING. YA EXISTE %s", path);
	}

}
/*TODO: DEPRECATED FUNCTION?? SI NO ES NECESARIO ELIMINARLO*/
void crearMetadataDirectorio(char* ruta) {

	char* ruta_archivo = string_from_format("%s/Metadata.AFIP", ruta);
	FILE* fp = fopen(ruta_archivo, "w");
	char* contenido = string_new();
	string_append(&contenido, "DIRECTORY=Y");

	fputs(contenido, fp);
	free(contenido);
	free(ruta_archivo);
	fclose(fp);

}

void crearMetadataGlobal() {

	char* rutaMetadata = string_new();
	string_append(&rutaMetadata, sindicato_conf.punto_montaje);
	string_append(&rutaMetadata, "/Metadata/");

	crearDirectorio(rutaMetadata);
	string_append(&rutaMetadata, "Metadata.AFIP");

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
int existeRestaurante(char* nombreRestaurante){
	char *ruta_pokemon = string_from_format("%s%s", ruta_files, nombreRestaurante);
	log_info(logger, "Ruta: %s", ruta_pokemon);
	FILE *fp = fopen(ruta_pokemon, "r");
	free(ruta_pokemon);

	if (fp) {
		fclose(fp);
		return 1;
	} else {
		return 0;
	}
}
char* generarContenidoRestauranteEnBloques(t_crear_restaurante* argsCrearRestaurante) {
	char* buffer = string_from_format("CANTIDAD_COCINEROS=%s\nPOSICION=%s\nAFINIDAD_COCINEROS=%s\nRECETAS=%s\nPRECIO_RECETAS=%s\nCANTIDAD_HORNOS=%s",
			argsCrearRestaurante->cantidadCocineros,
			argsCrearRestaurante->posicion,
			argsCrearRestaurante->afinidadCocineros,
			argsCrearRestaurante->platos,
			argsCrearRestaurante->preciosPlatos,
			argsCrearRestaurante->cantidadHornos);
	return buffer;
}
char* generarContenidoRecetaEnBloques(t_crear_receta* argsCrearReceta) {
	char* buffer = string_from_format("PASOS=%s\nTIEMPO_PASOS=%s",
			argsCrearReceta->pasos,
			argsCrearReceta->tiempoPasos);
	return buffer;
}
int calcularBloquesNecesarios(int bytes) {
	int aux = (bytes / tamanio_bloques);
	return (bytes % tamanio_bloques == 0) ? aux : (aux + 1);
}
int asignarBloqueLibre() {

	size_t sizeBitmap = bitarray_get_max_bit(bitmap);
	int bloque = 1;
	int count = 0;

	while(count < sizeBitmap && bloque != -1) {

		if(bitarray_test_bit(bitmap, count ) == 0) {
			bitarray_set_bit(bitmap, count);
			log_info(logger,"Bloque asignado: %d", count);
			bloque = count;
			return bloque;
		}
		count ++;
	}

	return bloque;
}
void escribirBloque(int bloque, char* buff) {

	log_info(logger, "Bloque a escribir: %d", bloque);
	log_info(logger, "Contenido a escribir: %s", buff);
	char* archivo = string_from_format("%s%d%s", ruta_bloques,bloque,".AFIP");
	FILE* fp = fopen(archivo, "w");
	fputs(buff, fp);
	fseek(fp,string_length(buff)+1,SEEK_SET);
	uint32_t uBloque = bloque;
	fprintf(fp, "%d", uBloque);
	fclose(fp);

}
void persistirDatos(char* contenido, char* str_bloques, int cant_bloques) {

	char* buffer;
	int inicio_str = 0;
	char** arr_bloques = string_get_string_as_array(str_bloques);

	if(cant_bloques == 0){
		cant_bloques = 1;
	}
	for(int i=1; i <= cant_bloques; i++) {

		buffer = string_substring(contenido, inicio_str, tamanio_bloques-4);
		int bloque = atoi(arr_bloques[i-1]);
		escribirBloque(bloque, buffer);
		inicio_str += tamanio_bloques-4;
		free(arr_bloques[i-1]);
	}
	free(arr_bloques);
}
void crearMetadataArchivo(char* ruta, t_metadata* metadata) {
//	log_info(logger, "%s", ruta_archivo);
	FILE* fp = fopen(ruta, "w");
	char* contenido = string_new();
	string_append(&contenido, "SIZE=");
	string_append(&contenido, string_itoa(metadata->size));
	string_append(&contenido, "\n");
	string_append(&contenido, "INITIAL_BLOCK=");
	string_append(&contenido, string_itoa(metadata->initial_block));

//	log_info(logger, "Contenido Metadata: %s", contenido);

	fputs(contenido, fp);
	free(contenido);
	fclose(fp);
}
void crearRestaurante(t_crear_restaurante* argsCrearRestaurante) {
	/*0.Se valida si ya existe el restaurante*/
	if(existeRestaurante(argsCrearRestaurante->nombreRestaurante))
		log_info(logger, "El restaurante:%s ya existe en el FileSystem Sindicato",argsCrearRestaurante->nombreRestaurante);
	else{
		// 1. Creo directorio con nombre restaurante
		char* ruta_restaurante = string_new();
		string_append(&ruta_restaurante, ruta_files);
		string_append(&ruta_restaurante, "Restaurantes/");
		string_append(&ruta_restaurante, argsCrearRestaurante->nombreRestaurante);
		log_info(logger, "RUTA: %s", ruta_restaurante);
		crearDirectorio(ruta_restaurante);
		// 2. Armar buffer a escribir
		char* linea = generarContenidoRestauranteEnBloques(argsCrearRestaurante);
		log_info(logger, "Linea a escribir: %s", linea);
		// 3. Calcular size buffer/archivo
		int tamanio = string_length(linea);
		log_info(logger, "Tamanio info: %d", tamanio);
		// 4. Calcular cantidad de bloques necesarios
		int bloques_necesarios = calcularBloquesNecesarios(tamanio);
		log_info(logger, "Bloques necesarios: %d", bloques_necesarios);
		// 5. Pido los bloques que necesito y genero el string de tipo [1,2,3]
		char* str_bloques = string_new();
		string_append(&str_bloques, "[");
		int bloqueInicial;
		for(int i=1; i <= bloques_necesarios; i++) {
			int bloque = asignarBloqueLibre();
			if(i==1)
				bloqueInicial = bloque;
			char* str_bloque = (i == bloques_necesarios) ? string_from_format("%d]", bloque) : string_from_format("%d,", bloque);
			string_append(&str_bloques, str_bloque);
		}
		log_info(logger, "Bloques: %s", str_bloques);

		// 6. Guardo la información en los bloques
		persistirDatos(linea, str_bloques, bloques_necesarios);

		// 7. Creo info.AFIP
		t_metadata* metadata = malloc(sizeof(t_metadata));
		metadata->initial_block = bloqueInicial;
		metadata->size = tamanio;

		string_append(&ruta_restaurante,"/info.AFIP");
		crearMetadataArchivo(ruta_restaurante, metadata);
//			aplicar_retardo_fs("Creación de Restaurante");LO DEJO COMENTADO PORQUE SEGURO SE LES OLVIDO A LOS AYUDANTES

		free(metadata);
		free(linea);
		free(ruta_restaurante);

		log_info(logger, "Restaurante:%s creado", argsCrearRestaurante->nombreRestaurante);
	}
}
void crearReceta(t_crear_receta* argsCrearReceta){
	/*0.Se valida si ya existe la receta*/
	if(existeRestaurante(argsCrearReceta->nombre))
		log_info(logger, "La receta:%s ya existe en el FileSystem Sindicato",argsCrearReceta->nombre);
	else{
		// 1. Creo directorio con nombre de la receta
		char* ruta_receta = string_new();
		string_append(&ruta_receta, ruta_files);
		string_append(&ruta_receta, "Recetas/");
		string_append(&ruta_receta, argsCrearReceta->nombre);
		log_info(logger, "RUTA: %s", ruta_receta);
		crearDirectorio(ruta_receta);
		// 2. Armar buffer a escribir
		char* linea = generarContenidoRecetaEnBloques(argsCrearReceta);
		log_info(logger, "Linea a escribir: %s", linea);
		// 3. Calcular size buffer/archivo
		int tamanio = string_length(linea);
		log_info(logger, "Tamanio info: %d", tamanio);
		// 4. Calcular cantidad de bloques necesarios
		int bloques_necesarios = calcularBloquesNecesarios(tamanio);
		log_info(logger, "Bloques necesarios: %d", bloques_necesarios);
		// 5. Pido los bloques que necesito y genero el string de tipo [1,2,3]
		char* str_bloques = string_new();
		string_append(&str_bloques, "[");
		int bloqueInicial;
		for(int i=1; i <= bloques_necesarios; i++) {
			int bloque = asignarBloqueLibre();
			if(i==1)
				bloqueInicial = bloque;
			char* str_bloque = (i == bloques_necesarios) ? string_from_format("%d]", bloque) : string_from_format("%d,", bloque);
			string_append(&str_bloques, str_bloque);
		}
		log_info(logger, "Bloques: %s", str_bloques);

		// 6. Guardo la información en los bloques
		persistirDatos(linea, str_bloques, bloques_necesarios);

		// 7. Creo nombreReceta.AFIP
		t_metadata* metadata = malloc(sizeof(t_metadata));
		metadata->initial_block = bloqueInicial;
		metadata->size = tamanio;
		string_append(&ruta_receta,"/");
		string_append(&ruta_receta,argsCrearReceta->nombre);
		string_append(&ruta_receta,".AFIP");
		crearMetadataArchivo(ruta_receta, metadata);
//			aplicar_retardo_fs("Creación de Restaurante");LO DEJO COMENTADO PORQUE SEGURO SE LES OLVIDO A LOS AYUDANTES

		free(metadata);
		free(linea);
		free(ruta_receta);

		log_info(logger, "Receta:%s creada", argsCrearReceta->nombre);
	}
}

void generarBloques() {

	for (int i = 0; i <= cantidad_bloques; i++) {

		char* nombre_bloque = string_new();
		string_append(&nombre_bloque, ruta_bloques);
		string_append(&nombre_bloque, string_itoa(i));
		string_append(&nombre_bloque, ".AFIP");

		FILE *fp = fopen(nombre_bloque, "w");
		fclose(fp);

		free(nombre_bloque);
	}
	log_info(logger, "Bloques generados.");
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
