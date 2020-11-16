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
	char* nombre_restaurante;
	int retardo_ciclo_cpu;
}t_restaurante_conf;

typedef struct{
	char nombre[L_ID];
	int socket_escucha;//para enviarle al cliente actualizaciones
	//t_list* pedidos;
}t_cliente;

// Variables globales
t_config* config;
t_restaurante_conf restaurante_conf;
t_log* logger;

void cargar_configuracion_restaurante(char *path_config);
void cargar_logger_restaurante();
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */
