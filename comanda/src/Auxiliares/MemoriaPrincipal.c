/*
 * MemoriaPrincipal.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MemoriaPrincipal.h"

void inicializar_memoria_principal(){
	memoria_fisica = malloc(mem_principal_global->tamanio_memoria);
	memset(memoria_fisica, '\0', mem_principal_global->tamanio_memoria);
}

void setear_params_global_mem_principal(){
	mem_principal_global = malloc(sizeof(t_params_global_mem_principal));
	mem_principal_global->tamanio_memoria = comanda_conf.tamanio_memoria;
	mem_principal_global->tamanio_swap = comanda_conf.tamanio_swap;
	mem_principal_global->algoritmo_reemplazo = comanda_conf.algoritmo_reemplazo;
}

void liberar_memoria_principal(){
	free(memoria_fisica);
	free(mem_principal_global);
}
