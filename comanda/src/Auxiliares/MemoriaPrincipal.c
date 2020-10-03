/*
 * MemoriaPrincipal.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MemoriaPrincipal.h"

void inicializar_memoria_principal(){
	memoria_fisica = malloc(mem_principal_global->tamanio_memoria);
	memset(memoria_fisica, '\0', mem_principal_global->tamanio_memoria);//TODO VER SI ES NEC.
}

void setear_params_global_mem_principal(){
	mem_principal_global = malloc(sizeof(t_params_global_mem_principal));
	mem_principal_global->tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	mem_principal_global->tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");

	if (strcmp(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "LRU")== 0)
		mem_principal_global->algoritmo_reemplazo = LRU;
	else
		mem_principal_global->algoritmo_reemplazo = CLOCK_MEJ;
}

void liberar_memoria_principal(){
	free(memoria_fisica);
	free(mem_principal_global);
}
