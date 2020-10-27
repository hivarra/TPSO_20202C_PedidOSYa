/*
 * MemoriaPrincipal.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MemoriaPrincipal.h"

void inicializar_memoria_principal(){
	memoria_fisica = calloc(1,mem_principal_global.tamanio_memoria);//Inicializa con /0
	init_bitmap_mp();
}

void setear_params_global_mem_principal(){
	mem_principal_global.tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	mem_principal_global.tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");

	if (strcmp(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "LRU")== 0)
		mem_principal_global.algoritmo_reemplazo = LRU;
	else
		mem_principal_global.algoritmo_reemplazo = CLOCK_MEJ;
}

void liberar_memoria(){
	free(memoria_fisica);
	//TODO:liberar_tablas();
}

void liberar_memoria_swap(){
	remove(path_memoria_swap);
	free(path_memoria_swap);
	char* directorio_swap = getParentPath();
	string_append(&directorio_swap, "/Swap");
	rmdir(directorio_swap);
	free(directorio_swap);
}
