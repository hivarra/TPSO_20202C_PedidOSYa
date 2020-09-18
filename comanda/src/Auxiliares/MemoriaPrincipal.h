/*
 * memoria_principal.h
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MEMORIAPRINCIPAL_H_
#define AUXILIARES_MEMORIAPRINCIPAL_H_

#include "Configuracion.h"

typedef struct{
	int tamanio_memoria;
	int tamanio_swap;
	char* algoritmo_reemplazo;
}t_params_global_mem_principal;

void* memoria_fisica;
t_params_global_mem_principal* mem_principal_global;

void inicializar_memoria_principal();
void setear_params_global_mem_principal();
void liberar_memoria_principal();

#endif /* AUXILIARES_MEMORIAPRINCIPAL_H_ */
