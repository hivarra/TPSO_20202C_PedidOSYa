#include "Filesystem.h"

/*Declaracion de funciones privadas*/
void crearBitmap(void);
void leerBitmap(void);
void crearDirectorio(char*);
void crearMetadataGlobal(void);
void crearDirectorioFiles(void);
void crearDirectorioRestaurantes(void);
void crearDirectorioRecetas(void);
void crearDirectorioBloques(void);
void generarBloques(void);

void montarFileSystem() {

	char* path_metadata = string_from_format("%s/Metadata/Metadata.AFIP", config_get_string_value(config, "PUNTO_MONTAJE"));
	FILE* f_metadata = fopen(path_metadata, "r");

	if (f_metadata != NULL){
		fclose(f_metadata);
		t_config* metadata_global = config_create(path_metadata);
		tamanio_bloques = config_get_int_value(metadata_global, "BLOCK_SIZE");
		cantidad_bloques = config_get_int_value(metadata_global, "BLOCKS");
		leerBitmap();
		ruta_bloques = string_from_format("%s/Blocks/", config_get_string_value(config, "PUNTO_MONTAJE"));
		ruta_files = string_from_format("%s/Files/", config_get_string_value(config, "PUNTO_MONTAJE"));
		ruta_restaurantes = string_from_format("%s/Restaurantes/", ruta_files);
		ruta_recetas = string_from_format("%s/Recetas/", ruta_files);
		log_trace(logger,"\tBlocks: %d.", cantidad_bloques);
		log_trace(logger,"\tBlock size: %d.", tamanio_bloques);
		log_trace(logger,"\tMagic number: %s.", config_get_string_value(metadata_global, "MAGIC_NUMBER"));
		log_trace(logger,"File System AFIP: Se leyo el FS existente.");
		config_destroy(metadata_global);
	}
	else{
		crearDirectorio(config_get_string_value(config, "PUNTO_MONTAJE"));
		crearMetadataGlobal();
		crearBitmap();
		crearDirectorioBloques();
		generarBloques();
		crearDirectorioFiles();
		crearDirectorioRestaurantes();
		crearDirectorioRecetas();
		log_trace(logger,"\tBlocks: %d.", cantidad_bloques);
		log_trace(logger,"\tBlock size: %d.", tamanio_bloques);
		log_trace(logger,"\tMagic number: %s.", config_get_string_value(config, "MAGIC_NUMBER"));
		log_trace(logger,"File System AFIP: Creación finalizada.");
	}
	free(path_metadata);
	semaforos_pedidos = dictionary_create();
	pthread_mutex_init(&mutexSemaforosPedidos, NULL);
}

void crearBitmap() {

	char* rutaBitmap = string_from_format("%s/Metadata/Bitmap.bin", config_get_string_value(config, "PUNTO_MONTAJE"));

	int bitarray_fd = open(rutaBitmap, O_RDWR | O_CREAT, 0777);
	free(rutaBitmap);

	if (bitarray_fd == -1){
		puts("No se pudo crear el fichero del bitarray.");
		exit(-1);
	}

	ftruncate(bitarray_fd, cantidad_bloques/8);

	bmap = mmap(NULL, cantidad_bloques/8, PROT_READ | PROT_WRITE, MAP_SHARED, bitarray_fd, 0);
	close(bitarray_fd);

	if (bmap == MAP_FAILED) {
		puts("No se pudo mapear el fichero del bitarray.");
		exit(-1);
	}

	bitmap = bitarray_create_with_mode(bmap, cantidad_bloques/8, LSB_FIRST);

	for(int i = 0; i < cantidad_bloques; i++){
		 bitarray_clean_bit(bitmap, i);
	}

	if (msync(bmap, cantidad_bloques/8, MS_SYNC) == -1)
		log_warning(logger, "No se pudo actualizar el bitmap.");

	pthread_mutex_init(&mutex_bitmap, NULL);
}

void leerBitmap(){

	char* rutaBitmap = string_from_format("%s/Metadata/Bitmap.bin", config_get_string_value(config, "PUNTO_MONTAJE"));

	int bitarray_fd = open(rutaBitmap, O_RDWR, 0777);
	free(rutaBitmap);

	if (bitarray_fd == -1){
		puts("No se pudo leer el fichero del bitarray.");
		exit(-1);
	}

	bmap = mmap(NULL, cantidad_bloques/8, PROT_READ | PROT_WRITE, MAP_SHARED, bitarray_fd, 0);
	close(bitarray_fd);

	if (bmap == MAP_FAILED) {
		puts("No se pudo mapear el fichero del bitarray.");
		exit(-1);
	}

	bitmap = (t_bitarray*)&bmap;

	if (msync(bmap, cantidad_bloques/8, MS_SYNC) == -1)
		log_warning(logger, "No se pudo actualizar el bitmap.");

	pthread_mutex_init(&mutex_bitmap, NULL);
}

void crearDirectorioFiles() {

	ruta_files = string_from_format("%s/Files/", config_get_string_value(config, "PUNTO_MONTAJE"));
	crearDirectorio(ruta_files);
}

void crearDirectorioRestaurantes() {

	ruta_restaurantes = string_from_format("%s/Restaurantes/", ruta_files);
	crearDirectorio(ruta_restaurantes);
}

void crearDirectorioRecetas() {

	ruta_recetas = string_from_format("%s/Recetas/", ruta_files);
	crearDirectorio(ruta_recetas);
}

void crearDirectorioBloques() {
	ruta_bloques = string_from_format("%s/Blocks/", config_get_string_value(config, "PUNTO_MONTAJE"));
	crearDirectorio(ruta_bloques);
}

void generarBloques() {

	for (int i = 1; i <= cantidad_bloques; i++) {

		char* nombre_bloque_i = string_from_format("%s%d.AFIP", ruta_bloques, i);

		FILE* file_bloque_i = fopen(nombre_bloque_i, "w");
		truncate(nombre_bloque_i, tamanio_bloques);
		free(nombre_bloque_i);
		fseek(file_bloque_i, -sizeof(uint32_t), SEEK_END);//Pongo el puntero al archivo para escribir los ultimos 4 bytes
		uint32_t num_cero = 0;
		fwrite(&num_cero, sizeof(uint32_t), 1, file_bloque_i);
		fclose(file_bloque_i);
	}
}

void crearMetadataGlobal() {

	char* rutaMetadata = string_from_format("%s/Metadata", config_get_string_value(config, "PUNTO_MONTAJE"));
	crearDirectorio(rutaMetadata);

	string_append(&rutaMetadata, "/Metadata.AFIP");
	FILE *file = fopen(rutaMetadata, "w");
	free(rutaMetadata);

	tamanio_bloques = config_get_int_value(config, "BLOCK_SIZE");
	char* buffer = string_from_format("BLOCK_SIZE=%d\n", tamanio_bloques);

	cantidad_bloques = config_get_int_value(config, "BLOCKS");
	string_append_with_format(&buffer, "BLOCKS=%d\n", cantidad_bloques);

	string_append_with_format(&buffer, "MAGIC_NUMBER=%s", config_get_string_value(config, "MAGIC_NUMBER"));

	fputs(buffer, file);
	free(buffer);
	fclose(file);
}

int existeDirectorio(char *path) {

	struct stat st = { 0 };

	if (stat(path, &st) != -1)
		return 1;
	else
		return 0;
}

void crearDirectorio(char *path) {

	if (!existeDirectorio(path)){
		if (mkdir(path, 0777) == -1) {
			perror("mkdir");
			log_error(logger, "[CrearDirectorio] ERROR. PERMISOS %s", path);
		}
	}
//SI YA EXISTE, NO HACE NADA.
}

int existeFile(char* file){
	int result = 0;

	FILE* f_file = fopen(file, "r");

	if (f_file != NULL){
		fclose(f_file);
		result = 1;
	}
	return result;
}

void escribirBloque(int bloque, void* buff, int size, int sig_bloque) {

	char* archivo = string_from_format("%s%d.AFIP", ruta_bloques, bloque);
	FILE* file_bloque = fopen(archivo, "r+");
	fwrite(buff, size, 1, file_bloque);
	fseek(file_bloque, -sizeof(uint32_t), SEEK_END);
	uint32_t u_sig_bloque = sig_bloque;
	fwrite(&u_sig_bloque, sizeof(uint32_t), 1, file_bloque);
	fclose(file_bloque);
	free(archivo);
}

void persistirDatos(char* contenido, int* array_bloques) {

	int tamEscribible = tamanio_bloques-sizeof(uint32_t);
	int tamContenidoNoEscrito = strlen(contenido)+1;
	int cant_bloques = calcularBloquesNecesarios(tamContenidoNoEscrito);
	int sig_bloque = 0;
	int offset = 0;

	for(int i = 0; i < cant_bloques; i++){
		/*Si es ultimo bloque, el siguiente bloque sera 0*/
		if (i != cant_bloques-1)
			sig_bloque = array_bloques[i+1];
		else
			sig_bloque = 0;

		/*Verifico si debo escribir el bloque completo*/
		if (tamContenidoNoEscrito >= tamEscribible){
			escribirBloque(array_bloques[i], contenido+offset, tamEscribible, sig_bloque);
			offset += tamEscribible;
			tamContenidoNoEscrito -= tamEscribible;
		}
		/*Si no tengo que escribir el bloque completo*/
		else
			escribirBloque(array_bloques[i], contenido+offset, tamContenidoNoEscrito, sig_bloque);
	}
}

void crearMetadataArchivo(char* ruta, t_metadata* metadata){

	FILE* fp = fopen(ruta, "w");
	char* contenido = string_from_format("SIZE=%d\nINITIAL_BLOCK=%d", metadata->size, metadata->initial_block);
	fputs(contenido, fp);
	free(contenido);
	fclose(fp);
}

t_metadata* leerMetadataArchivo(char* ruta){
	t_metadata* metadata = NULL;

	t_config* configMetadata = config_create(ruta);
	if (configMetadata != NULL){
		metadata = malloc(sizeof(t_metadata));
		metadata->size = config_get_int_value(configMetadata, "SIZE");
		metadata->initial_block = config_get_int_value(configMetadata, "INITIAL_BLOCK");
		config_destroy(configMetadata);
	}
	return metadata;
}

int actualizarSizeMetadataArchivo(char* ruta, int size){
	int result = 0;

	t_config* configMetadata = config_create(ruta);
	if (configMetadata != NULL && config_has_property(configMetadata, "SIZE")){
		char* str_size = string_itoa(size);
		config_set_value(configMetadata, "SIZE", str_size);
		free(str_size);
		config_save(configMetadata);
		config_destroy(configMetadata);
		result = 1;
	}
	return result;
}
