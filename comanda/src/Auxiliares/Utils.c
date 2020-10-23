/*
 * Utils.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Utils.h"

int cant_frames_mp;

void signalHandler(int sig){

	liberar_memoria();

	bitarray_destroy(bitmap_mp);
	free(reserva_bitmap_mp);
	pthread_mutex_destroy(&mutex_bitmap_mp);

	destruir_logger(logger);
	destruir_config(config);

	puts("Fin COMANDA");

	exit(EXIT_SUCCESS);
}

void init_bitmap_mp(){
	cant_frames_mp = mem_principal_global->tamanio_memoria/32;
	reserva_bitmap_mp = malloc(cant_frames_mp/8);

	bitmap_mp = bitarray_create_with_mode(reserva_bitmap_mp, cant_frames_mp/8, LSB_FIRST);

	for(int i = 0; i < cant_frames_mp; i++){
		 bitarray_clean_bit(bitmap_mp, i);
	}
	pthread_mutex_init(&mutex_bitmap_mp, NULL);
}

int get_frame_free_mp(){
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
