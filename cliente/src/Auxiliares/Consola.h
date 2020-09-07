/*
 * Consola.h
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include <shared.h>
#include "Logueo.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


int crearConsola();
int procesar_comando(char *line);


// Auxiliares de consola
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

#endif /* AUXILIARES_CONSOLA_H_ */
