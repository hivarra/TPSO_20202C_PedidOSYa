/*
 * Conexion.h
 *
 *  Created on: 13 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#include <protocolo.h>
#include "Configuracion.h"
#include "SocketEscucha.h"

t_tipoProceso tipo_proceso_server;
int socket_escucha;

int conectar_a_server();
void inicializar_conexion();
void terminar_programa(int);

#endif /* AUXILIARES_CONEXION_H_ */
