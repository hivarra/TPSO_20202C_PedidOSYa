/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

void liberar_bit_bloque(int pos){

	pthread_mutex_lock(&mutex_bitmap);
	bitarray_clean_bit(bitmap, pos-1);
	pthread_mutex_unlock(&mutex_bitmap);

	if (msync(bmap, cantidad_bloques/8, MS_SYNC) == -1)
		log_warning(logger, "No se pudo actualizar el bitmap.");
}

char* reducir_path_completo(char* path_completo){
	char** separado = string_split(path_completo, "/");

	int contador = 0;
	while(separado[contador]){
		contador++;
	}
	char* reducido = string_from_format("/%s/%s", separado[contador-2], separado[contador-1]);
	liberar_lista(separado);
	return reducido;
}

int asignarBloqueLibre(char* path_completo) {
	int pos = -2;

	pthread_mutex_lock(&mutex_bitmap);
	for(int i = 0; i < cantidad_bloques; i++){

		if (bitarray_test_bit(bitmap, i) == 0){
			bitarray_set_bit(bitmap, i);
			pos = i;
			break;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap);

	if (msync(bmap, cantidad_bloques/8, MS_SYNC) == -1)
		log_warning(logger, "No se pudo actualizar el bitmap.");

	if(pos == -2){
		log_error(logger, "[FIN DEL PROCESO] NO EXISTEN BLOQUES LIBRES.");
		puts("[ERROR] NO EXISTEN BLOQUES LIBRES.");
		signalHandler(SIGINT);
	}
	else{
		char* path_reducido = reducir_path_completo(path_completo);
		log_info(logger, "[Bloque Asignado] Se asigna el bloque %d al archivo %s.", pos+1, path_reducido);
		free(path_reducido);
	}

	return pos+1;
}

void vaciar_bloque(int num_bloque){

	char* nombre_bloque = string_from_format("%s%d.AFIP", ruta_bloques, num_bloque);

	FILE* file_bloque = fopen(nombre_bloque, "w");
	truncate(nombre_bloque, tamanio_bloques);
	free(nombre_bloque);
	fseek(file_bloque, -sizeof(uint32_t), SEEK_END);//Pongo el puntero al archivo para escribir los ultimos 4 bytes
	uint32_t num_cero = 0;
	fwrite(&num_cero, sizeof(uint32_t), 1, file_bloque);
	fclose(file_bloque);
}

void liberar_bloque(int num_bloque, char* path_completo){

	vaciar_bloque(num_bloque);
	liberar_bit_bloque(num_bloque);
	char* path_reducido = reducir_path_completo(path_completo);
	log_info(logger, "[Bloque Desasignado] Se desasigna el bloque %d del archivo %s.", num_bloque, path_reducido);
	free(path_reducido);
}

int calcularBloquesNecesarios(int cant_bytes){

	int bloques_nec = cant_bytes / (tamanio_bloques-sizeof(uint32_t));

	if(cant_bytes % (tamanio_bloques-sizeof(uint32_t)) > 0)
		bloques_nec++;

	return bloques_nec;
}

int* listar_bloques_necesarios_file_nuevo(int size_nuevo, char* path_completo){

	int bloques_nec = calcularBloquesNecesarios(size_nuevo);//Se agrega el caracter '\0'

	int* array = (int*) malloc(bloques_nec*sizeof(int));//Pido memoria para los bloques necesarios.

	for(int i = 0; i < bloques_nec; i++){
		array[i] = asignarBloqueLibre(path_completo);
	}

	return array;
}

int* listar_bloques_necesarios_file_existente(int size_nuevo, int size_viejo, int* array_bloques_asignados, char* path_completo){

	int bloques_nec = calcularBloquesNecesarios(size_nuevo);

	int bloques_actuales = calcularBloquesNecesarios(size_viejo);

	/*SI FALTAN BLOQUES, PIDO BLOQUES NUEVOS Y LOS AGREGO AL ARRAY*/
	if (bloques_actuales < bloques_nec){
		array_bloques_asignados = realloc(array_bloques_asignados, bloques_nec*sizeof(int));
		for (int i = bloques_actuales; i < bloques_nec; i++){
			array_bloques_asignados[i] = asignarBloqueLibre(path_completo);
		}
	}
	/*SI SOBRAN BLOQUES, LOS LIBERO, LOS VACIO, Y LOS SACO DEL ARRAY*/
	else if (bloques_actuales > bloques_nec){
		for(int i = bloques_nec; i < bloques_actuales; i++){
			liberar_bloque(array_bloques_asignados[i], path_completo);
		}
		vaciar_bloque(array_bloques_asignados[bloques_nec-1]);//Se vacia el ultimo bloque asignado para que la escritura quede limpia
		array_bloques_asignados = realloc(array_bloques_asignados, bloques_nec*sizeof(int));
	}

	return array_bloques_asignados;
}

t_file_leido* leer_bloques_file(t_metadata* mData){

	t_file_leido* file_leido = malloc(sizeof(t_file_leido));

	/*Reserva inicial del array*/
	int bloques_nec = calcularBloquesNecesarios(mData->size);
	file_leido->array_bloques = (int*) malloc(bloques_nec*sizeof(int));
	int pos_actual_array = 0;

	/*Inicializacion del string por leer*/
	file_leido->string_leido = string_new();

	/*Variables auxiliares*/
	int tam_string = tamanio_bloques-sizeof(uint32_t);
	char* buffer = calloc(1,tam_string+1);
	int num_bloque_i = mData->initial_block;
	uint32_t num_bloque_sig;

	while(num_bloque_i != 0){
		/*Agrego al array el bloque leido*/
		file_leido->array_bloques[pos_actual_array] = num_bloque_i;
		pos_actual_array++;
		/*Leo desde el archivo el priximo bloque*/
		char* nombre_bloque_i = string_from_format("%s%d.AFIP", ruta_bloques, num_bloque_i);
		FILE* file_bloque_i = fopen(nombre_bloque_i, "r");
		free(nombre_bloque_i);
		/*Copio el string del bloque en un string total*/
		fread(buffer, tam_string, 1, file_bloque_i);
		string_append(&file_leido->string_leido, buffer);
		/*Leo el siguiente num de bloque*/
		fread(&num_bloque_sig, sizeof(uint32_t), 1, file_bloque_i);
		fclose(file_bloque_i);
		num_bloque_i = num_bloque_sig;//Para diferenciar el uint32_t del int
	}
	free(buffer);

	return file_leido;
}

pthread_mutex_t* mutex_pedido(char* restaurante, uint32_t pedido){
	pthread_mutex_t* semaforo;

	char* nombre_semaforo = string_from_format("%s_%d", restaurante, pedido);

	if(dictionary_has_key(semaforos_pedidos, nombre_semaforo))
		semaforo = dictionary_get(semaforos_pedidos, nombre_semaforo);
	else{
		semaforo = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_lock(&mutexSemaforosPedidos);
		pthread_mutex_init(semaforo, NULL);
		dictionary_put(semaforos_pedidos, nombre_semaforo, semaforo);
		pthread_mutex_unlock(&mutexSemaforosPedidos);
	}

	free(nombre_semaforo);
	pthread_mutex_lock(semaforo);

	return semaforo;
}
