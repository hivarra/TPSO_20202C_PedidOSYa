/*
 * planificador.h
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_

#include "Conexion.h"
#include "Plato.h"
#include "Cocinero.h"
#include "Metadata_restaurante.h"
#include "Semaforos.h"
#include "Hilos.h"

#include <semaphore.h>

typedef enum{
	FIFO,
	RR
}t_algoritmo;

uint32_t id_pcb_global;
t_dictionary* dictionary_colas_ready;
t_list* lista_colas_hornos;
t_algoritmo algoritmo_planificacion;

void inicializar_planificador(void);
void inicializar_planificacion(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta);

#endif /* AUXILIARES_PLANIFICADOR_H_ */
