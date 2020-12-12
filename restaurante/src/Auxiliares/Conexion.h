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
#include "Metadata_restaurante.h"
#include "Planificador.h"

typedef struct{
	t_list* comidas;
	uint32_t id_pedido;
}t_args_aux;

int socket_servidor;
int socket_envio;//POR ESTE SOCKET VAN A LLEVAR LAS CONSULTAS DE APP Y SE ENVIARAN SUS RESPUESTAS
int socket_escucha;//POR ESTE SOCKET SE ENVIARAN ACTUALIZACIONES DE PEDIDOS A APP
pthread_t hilo_escucha_app;

t_list* lista_pedidos_app;
t_list* clientes_conectados;

void conectar_a_app(void);
void conectar_a_sindicato(void);
void escuchar_clientes(void);


#endif /* AUXILIARES_CONEXION_H_ */
