/*
 * Utils.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Utils.h"

void signalHandler(int sig){

	close(socket_servidor);

	liberar_memoria();
	liberar_memoria_swap();

	bitarray_destroy(bitmap_mp);
	free(reserva_bitmap_mp);
	pthread_mutex_destroy(&mutex_bitmap_mp);

	bitarray_destroy(bitmap_ms);
	free(reserva_bitmap_ms);
	pthread_mutex_destroy(&mutex_bitmap_ms);

	destruir_logger(logger);
	destruir_config(config);

	puts("Fin COMANDA");

	exit(EXIT_SUCCESS);
}

int max(int num1, int num2){
   if (num1 > num2)
      return num1;
   else
      return num2;
}

void init_bitmap_mp(){
	cant_frames_mp = mem_principal_global.tamanio_memoria/32;
	int cant_bytes_bitmap = max(cant_frames_mp/8, 1);//Por si la division me da menor a 1 byte
	reserva_bitmap_mp = malloc(cant_bytes_bitmap);

	bitmap_mp = bitarray_create_with_mode(reserva_bitmap_mp, cant_bytes_bitmap, LSB_FIRST);

	for(int i = 0; i < cant_frames_mp; i++){
		 bitarray_clean_bit(bitmap_mp, i);
	}
	pthread_mutex_init(&mutex_bitmap_mp, NULL);
}

int get_free_frame_mp(){
	int pos = -1;
	pthread_mutex_lock(&mutex_bitmap_mp);
	for(int i = 0; i < cant_frames_mp; i++){
		if (bitarray_test_bit(bitmap_mp, i) == 0){
			bitarray_set_bit(bitmap_mp, i);
			pos = i;
			break;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap_mp);
	return pos;
}

void free_frame_mp(int pos){
	pthread_mutex_lock(&mutex_bitmap_mp);
	bitarray_clean_bit(bitmap_mp, pos);
	pthread_mutex_unlock(&mutex_bitmap_mp);
}

void init_bitmap_ms(){
	cant_frames_ms = mem_principal_global.tamanio_swap/32;
	int cant_bytes_bitmap = max(cant_frames_ms/8, 1);//Por si la division me da menor a 1 byte
	reserva_bitmap_ms = malloc(cant_bytes_bitmap);

	bitmap_ms = bitarray_create_with_mode(reserva_bitmap_ms, cant_bytes_bitmap, LSB_FIRST);

	for(int i = 0; i < cant_frames_ms; i++){
		 bitarray_clean_bit(bitmap_ms, i);
	}
	pthread_mutex_init(&mutex_bitmap_ms, NULL);
}

int get_free_frame_ms(){
	int pos = -1;
	pthread_mutex_lock(&mutex_bitmap_ms);
	for(int i = 0; i < cant_frames_ms; i++){
		if (bitarray_test_bit(bitmap_ms, i) == 0){
			bitarray_set_bit(bitmap_ms, i);
			pos = i;
			break;
		}
	}
	pthread_mutex_unlock(&mutex_bitmap_ms);
	return pos;
}

void free_frame_ms(int pos){
	pthread_mutex_lock(&mutex_bitmap_ms);
	bitarray_clean_bit(bitmap_ms, pos);
	pthread_mutex_unlock(&mutex_bitmap_ms);
}

void actualizar_bits_de_uso(t_entrada_pagina* entrada_pagina){
	if (mem_principal_global.algoritmo_reemplazo == LRU)
		entrada_pagina->ultimo_uso = timestamp();
	else
		entrada_pagina->uso = 1;
}
