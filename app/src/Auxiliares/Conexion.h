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
//#include <shared.h>
#include <protocolo.h>
#include "Configuracion.h"

typedef struct{
	char id[L_ID];
	char restaurante_seleccionado[L_ID];
	int socketEscucha;
	int pos_x;
	int pos_y;
	//t_list* pedidos;//(Posiblemente necesario)Pedidos que realiza un cliente
}t_info_cliente;

typedef struct{
	char id[L_ID];
	int socketEscucha;
	int socketEnvio;
	int pos_x;
	int pos_y;
}t_info_restaurante;

int socket_app;
//t_list* restaurantes;
pthread_mutex_t mutexRestaurantes;
pthread_mutex_t mutexClientes;

t_info_restaurante* infoRestoDefault;

t_list* restaurantesConectados;
t_list* clientesConectados;

void conectar_a_comanda();
void crearServidor();
void esperar_cliente(int socket_servidor);
void inicializarListaClientesRest();
void incializarRestoDefault();
t_info_cliente* buscarClienteConectado(char* id);
t_info_restaurante* buscarRestauranteConectado(char* nombre_restaurante);
uint32_t generar_id_pedido();
//void imprimir_restaurante(t_restaurante* restaurante);
//void agregarRestaurante(t_restaurante* restaurante);

#endif /* AUXILIARES_CONEXION_H_ */
