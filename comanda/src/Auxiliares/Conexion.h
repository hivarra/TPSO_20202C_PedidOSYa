/*
 * Conexion.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

//#define IP_COMANDA "127.0.0.1"

#include <protocolo.h>
#include "Config_y_log.h"
#include "MMU.h"

int socket_servidor;

void escuchar_conexiones_comanda();
//void escuchar__conexiones_comanda();

#endif /* AUXILIARES_CONEXION_H_ */
