/*
 * Repartidor.h
 *
 *  Created on: 27 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_REPARTIDOR_H_
#define AUXILIARES_REPARTIDOR_H_

#include "Utils.h"

void* correr_repartidor(t_repartidor* repartidor);
void mover_una_posicion(t_repartidor* repartidor);
void aplicar_tiempo_espera_ready();

#endif /* AUXILIARES_REPARTIDOR_H_ */
