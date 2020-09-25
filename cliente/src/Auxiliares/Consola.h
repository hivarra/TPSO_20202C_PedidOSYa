/*
 * Consola.h
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include <shared.h>
#include <protocolo.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Logueo.h"
#include "Configuracion.h"

int socket_envio;
int socket_recepcion;

void leer_consola();
void _leer_consola();
int procesar_comando(char *line);
void procesar_solicitud(char** parametros);
int conectar_a_server_de_config();
void inicializar_conexion_recepcion();
void crear_hilo_recepcion_mensajes();

// Auxiliares de consola
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

#endif /* AUXILIARES_CONSOLA_H_ */
