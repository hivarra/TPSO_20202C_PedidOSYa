/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#define IP_APP "127.0.0.1"

//#include <shared.h>
#include <protocolo.h>
#include "Configuracion.h"

typedef struct{
	char id[L_ID];
	char restaurante_seleccionado[L_ID];
	int socketEscucha;
	int pos_x;
	int pos_y;
	t_list* pedidos;//Pedidos que realiza un cliente
}t_info_cliente;

typedef struct{
	char id[L_ID];
	int socketEscucha;
	int socketEnvio;
	int pos_x;
	int pos_y;
}t_info_restaurante;

int socket_app;
uint32_t id_rest_default;
pthread_mutex_t mutexRestaurantes,
				mutexClientes,
				mutex_id_rest_default;

t_info_restaurante* infoRestoDefault;

t_list* restaurantesConectados;
t_list* clientesConectados;

void conectar_a_comanda(void);
int conectar_a_comanda_simple(void);
void crearServidor(void);
t_info_cliente* buscarClienteConectado(char* id);
t_info_restaurante* buscarRestauranteConectado(char* nombre_restaurante);
//void imprimir_restaurante(t_restaurante* restaurante);
//void agregarRestaurante(t_restaurante* restaurante);
char* estado_string(uint32_t estado_num);

#endif /* AUXILIARES_CONEXION_H_ */
