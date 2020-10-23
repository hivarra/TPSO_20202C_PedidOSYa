/*
 * Configuracion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <protocolo.h>
#include <shared.h>

// Definición de tipos
typedef struct{
	char* ip_sindicato;
	char* puerto_sindicato;
	int quantum;
	int cantidad_hornos;
	char* nombre_restaurante;
	uint32_t posX;
	uint32_t posY;
}t_restaurante_conf;

typedef struct{
	char nombre[L_ID];
	int* socket_envio;//para recibir solicitudes y responderlas
	int* socket_escucha;//para enviarle al cliente actualizaciones
	//t_list* pedidos;
}t_cliente;

// Variables globales
t_config* config;
t_restaurante_conf restaurante_conf;
t_log* logger;

uint32_t id_pedido_actual;//Manipularlo con un semaforo
t_list* cocineros_restaurante;
t_list* platos_restaurante;
t_list* clientes_conectados;//t_cliente

void cargar_configuracion_restaurante(char *path_config);
void cargar_logger_restaurante();
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */
