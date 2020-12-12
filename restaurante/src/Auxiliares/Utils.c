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
t_afinidad* obtener_id_afinidad(char nombre_plato[L_PLATO]){
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
