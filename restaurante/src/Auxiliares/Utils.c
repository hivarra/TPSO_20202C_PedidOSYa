/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

void signalHandler(int sig){

	destruir_logger(logger);
	destruir_config(config);

	if(socket_servidor != -1)
		close(socket_servidor);

	list_destroy_and_destroy_elements(clientes_conectados, free);

	puts("Fin RESTAURANTE");

	exit(EXIT_SUCCESS);
}
t_afinidad* obtener_afinidad(char nombre_plato[L_PLATO]){
	bool igual_nombre_plato(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,nombre_plato);
	}
	t_afinidad* afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)igual_nombre_plato);

	//Si no tiene afinidad, se debe agregar a cola ready sin afinidad ("N")
	bool es_sin_afinidad(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,"N");
	}
	if(afinidad_buscada == NULL)
		afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)es_sin_afinidad);

	return afinidad_buscada;
}
int min(int num1, int num2){
   if (num1 < num2)
      return num1;
   else
      return num2;
}
char* estado_pcb_enum_a_string(t_estado_pcb estado_enum){
	if(estado_enum == READY)
		return "READY";
	else if (estado_enum == NEW)
		return "NEW";
	else if (estado_enum == BLOCKED_POR_HORNO)
		return "BLOCKED_POR_HORNO";
	else if (estado_enum == BLOCKED_POR_REPOSAR)
		return "BLOCKED_POR_REPOSAR";
	else if (estado_enum == EXEC)
		return "EXEC";
	else
		return "EXIT";
}
t_list* duplicar_lista_pasos(t_list* lista_pasos){
	t_list* lista_duplicada = list_create();
	void copiar_elemento(t_paso_receta* paso){
		t_paso_receta* paso_nuevo = calloc(1,sizeof(t_paso_receta));
		strcpy(paso_nuevo->accion,paso->accion);
		paso_nuevo->tiempo = paso->tiempo;

		list_add(lista_duplicada,paso_nuevo);
	}
	list_iterate(lista_pasos,(void*)copiar_elemento);

	return lista_duplicada;
}
