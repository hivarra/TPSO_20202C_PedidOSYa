/*
 * planificador.h
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_

#include "Conexion.h"

typedef enum{
	FIFO,
	RR
}t_algoritmo;

t_algoritmo algoritmo_planificacion;

void inicializar_planificador(void);

#endif /* AUXILIARES_PLANIFICADOR_H_ */
