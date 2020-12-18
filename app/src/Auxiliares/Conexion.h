/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

//#define IP_APP "127.0.0.1"

#include <protocolo.h>
#include "Configuracion.h"
#include "Utils.h"

int socket_app;
uint32_t id_rest_default;

void inicializarListaClientesRest(void);
void conectar_a_comanda(void);
void crearServidor(void);
char* estado_string(uint32_t estado_num);

#endif /* AUXILIARES_CONEXION_H_ */
