/*
 * semaforos.h
 *
 *  Created on: 8 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_SEMAFOROS_H_
#define AUXILIARES_SEMAFOROS_H_

#include <pthread.h>

sem_t sem_planificar_platos;
sem_t finCPUbound;
sem_t* sem_realizar_paso;
sem_t* sem_hilos_cocineros;


#endif /* AUXILIARES_SEMAFOROS_H_ */
