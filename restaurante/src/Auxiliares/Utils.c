/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

void signalHandler(int sig){

	if(socket_envio != -1)
		close(socket_envio);
	if(socket_escucha != -1)
		close(socket_escucha);
	close(socket_servidor);

	pthread_mutex_destroy(&mutex_id_pcb);
	pthread_mutex_destroy(&mutex_id_pedidos);
	pthread_mutex_destroy(&mutex_cola_bloqueados_prehorno);
	pthread_mutex_destroy(&mutex_cola_exit);
	pthread_mutex_destroy(&mutex_cocineros);
	pthread_mutex_destroy(&mutex_afinidades_maestro);

	pthread_cancel(hilo_planificador_hornos);

	for(int i=0;i<list_size(AFINIDADES_MAESTRO);i++){
		list_destroy_and_destroy_elements(lista_colas_ready[i],free);
		pthread_mutex_destroy(&mutex_colas_ready[i]);
		sem_destroy(&sem_cola_ready[i]);
		sem_destroy(&sem_fin_paso[i]);
	}
	free(mutex_colas_ready);
	free(lista_colas_ready);
	free(sem_cola_ready);
	free(sem_fin_paso);

	for(int i=0;i<list_size(metadata_restaurante.afinidades_cocineros);i++){
		sem_destroy(&sem_realizar_paso[i]);
	}
	free(sem_realizar_paso);

	for(int i=0;i<metadata_restaurante.cantidad_hornos;i++){
		sem_destroy(&sem_hornear_plato[i]);
		pthread_mutex_destroy(&mutex_hornos[i]);
	}
	free(sem_hornear_plato);
	free(mutex_hornos);

	list_destroy_and_destroy_elements(metadata_restaurante.afinidades_cocineros,free);
	list_destroy_and_destroy_elements(metadata_restaurante.platos,free);
	list_destroy_and_destroy_elements(cola_exit,free);
	list_destroy_and_destroy_elements(cola_bloqueados_prehorno,free);
	list_destroy_and_destroy_elements(pedidos_pcbs,free);
	list_destroy_and_destroy_elements(AFINIDADES_MAESTRO,free);
	list_destroy_and_destroy_elements(lista_hornos,free);


	void destruir_cliente(t_cliente* cliente){
		close(cliente->socket_escucha);
		list_destroy_and_destroy_elements(cliente->pedidos,free);
		pthread_mutex_destroy(&cliente->mutex_pedidos);
		free(cliente);
	}
	list_destroy_and_destroy_elements(clientes_conectados, (void*)destruir_cliente);

	void destruir_cocinero(t_cocinero* cocinero){
		pthread_mutex_destroy(&cocinero->mutex_cocinero);
		free(cocinero);
	}
	list_destroy_and_destroy_elements(lista_cocineros, (void*)destruir_cocinero);

	destruir_logger(logger);
	destruir_config(config);

	puts("\n===================\nFin RESTAURANTE\n===================");

	sleep(2);

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
