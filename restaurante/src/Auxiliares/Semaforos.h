/*
 * semaforos.h
 *
 *  Created on: 8 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_SEMAFOROS_H_
#define AUXILIARES_SEMAFOROS_H_

#include <semaphore.h>

sem_t sem_hornear_plato;
sem_t sem_hay_espacio_en_horno;
sem_t finCPUbound;
sem_t* sem_realizar_paso;
sem_t* sem_hilos_cocineros;
sem_t* sem_horno_finalizado;
sem_t* sem_cola_ready;

#endif /* AUXILIARES_SEMAFOROS_H_ */
