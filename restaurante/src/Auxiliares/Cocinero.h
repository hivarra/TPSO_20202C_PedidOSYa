/*
 * Cocinero.h
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_COCINERO_H_
#define AUXILIARES_COCINERO_H_

#include "shared.h"
#include "Configuracion.h"
#include "Semaforos.h"
#include "Hilos.h"
#include "Utils.h"
#include "Plato.h"

typedef struct{
	uint32_t id;
	char* afinidad;
	t_pcb* pcb;

}t_cocinero;

void hilo_cocinero(t_cocinero* cocinero);

#endif /* AUXILIARES_COCINERO_H_ */
