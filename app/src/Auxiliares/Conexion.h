/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#define IP_APP "127.0.0.1"

#include <shared.h>
#include "Configuracion.h"

int socket_app;

void *crearServidor();
void* atenderConexion();

#endif /* AUXILIARES_CONEXION_H_ */
