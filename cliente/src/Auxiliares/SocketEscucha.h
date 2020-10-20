/*
 * SocketEscucha.h
 *
 *  Created on: 13 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_SOCKETESCUCHA_H_
#define AUXILIARES_SOCKETESCUCHA_H_

#include "Conexion.h"

pthread_t hilo_escucha;

void crear_hilo_conexion_escucha();

#endif /* AUXILIARES_SOCKETESCUCHA_H_ */
