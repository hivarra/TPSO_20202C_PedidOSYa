/*
 * Hilos.h
 *
 *  Created on: 8 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_HILOS_H_
#define AUXILIARES_HILOS_H_

pthread_mutex_t mutex_id_pcb;
pthread_mutex_t mutex_afinidades_maestro;
pthread_mutex_t* mutex_colas_hornos;
pthread_mutex_t* mutex_colas_ready;

#endif /* AUXILIARES_HILOS_H_ */
