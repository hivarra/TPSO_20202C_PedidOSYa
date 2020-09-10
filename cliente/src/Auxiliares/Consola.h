/*
 * Consola.h
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include <shared.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Logueo.h"
#include "Configuracion.h"


int crearConsola();
int procesar_comando(char *line);
void procesar_mensaje(char** parametros);
int inicializar_conexion(char* proceso);
void procesar_solicitud(int socket, char** parametros);

// Auxiliares de consola
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

#endif /* AUXILIARES_CONSOLA_H_ */
