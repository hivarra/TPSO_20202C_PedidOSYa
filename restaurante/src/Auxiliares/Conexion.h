/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#define IP_RESTAURANTE "127.0.0.1"

#include <protocolo.h>
#include "Configuracion.h"
#include "Mensajes.h"

int socket_servidor;
int socket_envio;//POR ESTE SOCKET VAN A LLEVAR LAS CONSULTAS DE APP Y SE ENVIARAN SUS RESPUESTAS
int socket_escucha;//POR ESTE SOCKET SE ENVIARAN ACTUALIZACIONES DE PEDIDOS A APP
pthread_t hilo_escucha_app;

void conectar_a_app(void);
void conectar_a_sindicato(void);
void escuchar_clientes(void);


#endif /* AUXILIARES_CONEXION_H_ */
