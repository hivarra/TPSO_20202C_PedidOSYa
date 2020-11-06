/*
 * Utils.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_UTILS_H_
#define AUXILIARES_UTILS_H_

typedef struct{
	char* string_leido;
	int* array_bloques;
}t_file_leido;

#include "../sindicato.h"
#include "Filesystem.h"

int calcularBloquesNecesarios(int);
int* listar_bloques_necesarios_file_nuevo(int new_size);
int* listar_bloques_necesarios_file_existente(int new_size, int old_size, int* array_blocks_asigned);
t_file_leido* leer_bloques_file(t_metadata*);//Devuelve el string total del archivo y un array con los bloques asignados

#endif /* AUXILIARES_UTILS_H_ */
