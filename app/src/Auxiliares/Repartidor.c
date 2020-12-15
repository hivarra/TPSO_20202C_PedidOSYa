/*
 * Repartidor.c
 *
 *  Created on: 27 nov. 2020
 *      Author: utnso
 */

#include "Repartidor.h"

void* correr_repartidor(t_repartidor* repartidor) {

	// Semáforo para ver si está disponible para moverse
	// Mover entrenador a posición nueva
	while(1) {

		sem_wait(&repartidor->sem_moverse);

		int descansar = 0;
		t_instruccion instruccion_anterior = repartidor->instruccion;
//		log_info(logger, "Repartidor N°%d | Estado %s", repartidor->id, get_nombre_instruccion(repartidor->instruccion));
//		log_info(logger, "Repartidor N°%d | Se va a mover %d", repartidor->id , repartidor->quantum);

		while(repartidor->quantum > 0 && !descansar) {
			mover_una_posicion(repartidor);

			if(repartidor->frecuenciaDescanso == 0) {
				descansar = 1;
				log_info(logger, "Repartidor N°%d tiene que descansar", repartidor->id);
				break;
			}

		}

//		if(repartidor->quantum != 0) {
		if(repartidor->frecuenciaDescanso == 0) {
			// Está descansando
			repartidor->instruccion = DESCANSAR;
		} else if (repartidor->instruccion == BUSCAR_PEDIDO){
			// Esperar pedido
			repartidor->instruccion = ESPERAR_PEDIDO;
		}

		log_info(logger, "Repartidor N°%d | Pasó a estado %s", repartidor->id, get_nombre_instruccion(repartidor->instruccion));

		if(repartidor->instruccion == ESPERAR_PEDIDO || repartidor->instruccion == DESCANSAR) {
			bloquearPCB(repartidor, instruccion_anterior);
		} else {
			finalizarPCB(repartidor);
		}

	}

	return NULL;
}

void mover_una_posicion(t_repartidor* repartidor) {

	// Verifico si me tengo que mover en X o en Y
	if(repartidor->posX != repartidor->objetivo_posX) {

		// Valido si me muevo para la derecha o izquierda
		if(repartidor->posX < repartidor->objetivo_posX) {
			repartidor->posX += 1;
		} else {
			repartidor->posX -= 1;
		}

	} else {
		// Valido si me muevo para abajo o para arriba
		if(repartidor->posY < repartidor->objetivo_posY) {
			repartidor->posY += 1;
		} else {
			repartidor->posY -= 1;
		}
	}

	aplicar_retardo_operacion();

	char* desc = string_from_format("Repartidor N°%d", repartidor->id);
	log_info(logger, "%s | Se movió a la posición: %d,%d", desc, repartidor->posX, repartidor->posY);

	repartidor->quantum -= 1;
	repartidor->frecuenciaDescanso -= 1;

}


