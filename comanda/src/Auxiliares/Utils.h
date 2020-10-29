/*
 * Utils.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_UTILS_H_
#define AUXILIARES_UTILS_H_

#include <shared.h>
#include <commons/bitarray.h>
#include "MemoriaPrincipal.h"
#include "Conexion.h"

int cant_frames_mp;
int cant_frames_ms;

void* reserva_bitmap_mp;
t_bitarray* bitmap_mp;
pthread_mutex_t mutex_bitmap_mp;

void* reserva_bitmap_ms;
t_bitarray* bitmap_ms;
pthread_mutex_t mutex_bitmap_ms;


void signalHandler(int);//Libera recursos al terminar con ctrl+c. Util para probar que recursos no se estan liberando.

void init_bitmap_mp(void);
int get_free_frame_mp(void);
void free_frame_mp(int);

void init_bitmap_ms(void);
int get_free_frame_ms(void);
void free_frame_ms(int);

void actualizar_bits_de_uso(t_entrada_pagina*);

#endif /* AUXILIARES_UTILS_H_ */
