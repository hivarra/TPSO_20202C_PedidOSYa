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
#include "MMU.h"

void* reserva_bitmap_mp;
t_bitarray* bitmap_mp;
pthread_mutex_t mutex_bitmap_mp;


void signalHandler(int);//Libera recursos al terminar con ctrl+c. Util para probar que recursos no se estan liberando.
void init_bitmap_mp(void);
int get_frame_free_mp(void);
void free_frame_mp(int);

#endif /* AUXILIARES_UTILS_H_ */
