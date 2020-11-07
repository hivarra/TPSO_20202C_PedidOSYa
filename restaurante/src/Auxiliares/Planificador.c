/*
 * planificador.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#include "Planificador.h"

void inicializar_algoritmo(){
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			algoritmo_planificacion = FIFO;
		else
			algoritmo_planificacion = RR;
}
void inicializar_colas_ready(){
	dictionary_colas_ready = dictionary_create();

	for(int i=0;i<=list_size(metadata_restaurante->afinidades_cocineros);i++){
		char* afinidad = list_get(metadata_restaurante->afinidades_cocineros,i);
		if(string_equals_ignore_case(afinidad,"N")){
			t_list* cola_ready = list_create();
			dictionary_put(dictionary_colas_ready,afinidad,cola_ready);
		}
	}
	t_list* cola_ready_normal = list_create();
	dictionary_put(dictionary_colas_ready,"NORMAL",cola_ready_normal);
}
void inicializar_colas_hornos(){
	for(int i=0;i<=metadata_restaurante->cantidad_hornos;i++){
		t_list* cola_horno = list_create();
		list_add(lista_colas_hornos,cola_horno);
	}
}
void inicializar_planificador(){
	inicializar_algoritmo();
	inicializar_colas_ready();
	inicializar_colas_hornos();
}
