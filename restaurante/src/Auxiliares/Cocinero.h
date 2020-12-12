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
	char afinidad[L_PLATO];
	t_pcb* pcb;
	pthread_mutex_t mutex_cocinero;
}t_cocinero;

t_list* lista_cocineros;

void eliminar_paso_realizado(t_pcb* pcb);
t_paso_receta* obtener_siguiente_paso(t_pcb* pcb);
bool cocinero_esta_ejecutando(t_pcb* pcb);
void hilo_cocinero(t_cocinero* cocinero);
void eliminar_paso(t_paso_receta* paso,t_pcb* plato);

#endif /* AUXILIARES_COCINERO_H_ */
