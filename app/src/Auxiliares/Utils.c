/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

char* instruccion_str[] = { "BUSCAR_PEDIDO", "ESPERAR_PEDIDO", "ENTREGAR_PEDIDO", "DESCANSAR", NULL };

t_info_cliente* buscarClienteConectado(char* nombre_cliente){
	bool cliente_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(info_cliente->id, nombre_cliente);
	}

	pthread_mutex_lock(&mutexClientes);
	t_info_cliente* cliente = list_find(clientesConectados,(void*)cliente_igual);
	pthread_mutex_unlock(&mutexClientes);

	return cliente;
}

t_info_cliente* buscarClientePorPedido(int id_pedido){

	bool contiene_pedido(uint32_t* pedido){
		return *pedido == id_pedido;
	}

	bool cliente_igual(t_info_cliente* info_cliente){
		return list_any_satisfy(info_cliente->pedidos, (void*)contiene_pedido);
	}

	pthread_mutex_lock(&mutexClientes);
	t_info_cliente* cliente = list_find(clientesConectados,(void*)cliente_igual);
	pthread_mutex_unlock(&mutexClientes);

	return cliente;
}

t_info_restaurante* buscarRestauranteConectado(char* nombre_restaurante) {
	bool restaurante_igual(t_info_restaurante* info_restaurante) {
		return string_equals_ignore_case(info_restaurante->id,
				nombre_restaurante);
	}
	t_info_restaurante* restaurante;
	pthread_mutex_lock(&mutexRestaurantes);
	if (list_size(restaurantesConectados) > 0)
		restaurante = list_find(restaurantesConectados, (void*)restaurante_igual);
	else
		restaurante = infoRestoDefault;
	pthread_mutex_unlock(&mutexRestaurantes);

	return restaurante;
}

t_rta_consultar_restaurantes* obtenerRestaurantes(){
	t_rta_consultar_restaurantes* respuesta_restaurantes = malloc(sizeof(t_rta_consultar_restaurantes));
	respuesta_restaurantes->restaurantes = list_create();

	void* obtenerIdChar(t_info_restaurante* info_restaurante){
		char* nombre_restraurante = malloc(L_ID);
		strcpy(nombre_restraurante, info_restaurante->id);
		return nombre_restraurante;
	}

	pthread_mutex_lock(&mutexRestaurantes);
	if(list_size(restaurantesConectados) > 0) {
		/*MAPPING  t_info_cliente -> char[L_ID] */
		respuesta_restaurantes->restaurantes = list_map(restaurantesConectados,(void*)obtenerIdChar);
		respuesta_restaurantes->cantRestaurantes = list_size(restaurantesConectados);
	} else {
		// Cargo el restaurante default
		char* resto_default = calloc(1, L_ID);
		strcpy(resto_default, infoRestoDefault->id);
		list_add(respuesta_restaurantes->restaurantes, resto_default);
		respuesta_restaurantes->cantRestaurantes = 1;
	}
	pthread_mutex_unlock(&mutexRestaurantes);

	return respuesta_restaurantes;
}

void imprimirRepartidor(t_repartidor* repartidor) {

	log_info(logger, "Repartidor N°%d | Posición: %d-%d | Objetivo: %d-%d | Descanso: %d | Duración: %d", repartidor->id, repartidor->posX, repartidor->posY, repartidor->objetivo_posX, repartidor->objetivo_posY, repartidor->frecuenciaDescanso, repartidor->tiempoDescanso);

}

void imprimirPCB(t_pcb* pcb) {

	log_info(logger, "PCB | Pedido: %d | Repartidor: %d | Instrucción: %d | Posición: %d-%d", pcb->id_pedido, pcb->id_repartidor, pcb->instruccion, pcb->restaurante_posX, pcb->restaurante_posY);
}

void crearPCB(t_info_cliente* cliente, int id_pedido) {

	// Busco el restaurante para la ubicación
	t_info_restaurante* info_rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);

	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->id_pedido = id_pedido;
	pcb->id_repartidor = -1;
	pcb->instruccion = -1;
	strcpy(pcb->restaurante, cliente->restaurante_seleccionado);
	pcb->restaurante_posX = info_rest->pos_x;
	pcb->restaurante_posY = info_rest->pos_x;
	pcb->cliente_posX = cliente->pos_x;
	pcb->cliente_posY = cliente->pos_y;
	pcb->ultima_estimacion = app_conf.estimacion_inicial;
	pcb->tiempo_espera_ready = 0;
	pthread_mutex_lock(&mutex_nuevos);
	list_add(pedidos_planificables, pcb);
	pthread_mutex_unlock(&mutex_nuevos);
	sem_init(&pcb->sem_pedido_listo, 0, 0);
	sem_post(&sem_pedidos);
	log_info(logger, "Se creo PCB para el pedido: %d", pcb->id_pedido);
	imprimirPCB(pcb);

}

void disponibilizar_repartidor(t_repartidor* repartidor) {

	repartidor->instruccion = BUSCAR_PEDIDO;
	repartidor->disponible = 1;
	sem_post(&sem_repartidor_disponible);

	log_info(logger, "Repartidor N°%d disponible", repartidor->id);
}

t_repartidor* repartidor_mas_cercano(int posX, int posY) {

	t_repartidor* repartidor_cercano ;
	int menor_distancia = 100000;

	int disponibles(t_repartidor* repartidor) {
		return repartidor->disponible;
	}

	pthread_mutex_lock(&mutexRepartidores);
	t_list* repartidores_disponibles = list_filter(repartidores, (void*)disponibles);
	pthread_mutex_unlock(&mutexRepartidores);

	void mas_cercano(t_repartidor* repartidor) {

		int distancia_actual = distancia_a_posicion(repartidor, posX, posY);
		if(distancia_actual < menor_distancia) {
			menor_distancia = distancia_actual;
			repartidor_cercano = repartidor;
		}
	}

	list_iterate(repartidores_disponibles, (void*)mas_cercano);

	return repartidor_cercano;
}

int distancia_a_posicion(t_repartidor* repartidor, int posX, int posY) {

	int x = distancia_absoluta(repartidor->posX, posX);
	int y = distancia_absoluta(repartidor->posY, posY);
	return x + y;

}

int distancia_absoluta(int pos1, int pos2) {

	return fabs(pos1 - pos2);
}

void asignar_repartidor(t_repartidor* repartidor, t_pcb* pcb) {

	pcb->id_repartidor = repartidor->id;
	repartidor->disponible = 0;
	repartidor->objetivo_posX = pcb->restaurante_posX;
	repartidor->objetivo_posY = pcb->restaurante_posY;

//	log_info(logger, "Repartidor N°%d asignado a pedido N°%d", repartidor->id, pcb->id_pedido);
}

t_repartidor* obtener_repartidor(int id_repartidor) {

	int es_igual(t_repartidor* repartidor) {
		return repartidor->id == id_repartidor;
	}

	return list_find(repartidores, (void*)es_igual);
}

char* get_nombre_instruccion(t_instruccion enum_instruccion) {

	return instruccion_str[enum_instruccion];
}

void aplicar_retardo_operacion() {

	sleep(app_conf.retardo_ciclo_cpu);
}

void ejecutarPCB(t_pcb* pcb) {

	pthread_mutex_lock(&mutex_ejecutando);
	list_add(ejecutando, pcb);
	pthread_mutex_unlock(&mutex_ejecutando);
	t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);

	if(repartidor->instruccion == BUSCAR_PEDIDO) {
		log_info(logger, "Repartidor N°%d | Pasa a EXEC | Buscar pedido N°%d", pcb->id_repartidor, pcb->id_pedido);
	} else if (repartidor->instruccion == ENTREGAR_PEDIDO){
		log_info(logger, "Repartidor N°%d | Pasa a EXEC | Entregar pedido N°%d", pcb->id_repartidor, pcb->id_pedido);
	}
}

void bloquearPCB(t_repartidor* repartidor, t_instruccion instruccion_anterior) {

	t_pcb* pcb = sacarPCBDeEjecutando(repartidor);

	pthread_mutex_lock(&mutex_bloqueados);
	list_add(bloqueados, pcb);
	pthread_mutex_unlock(&mutex_bloqueados);

//	log_info(logger, "PCB | Pedido N°%d bloqueado por Repartidor N°%d en Estado %s", pcb->id_pedido, repartidor->id, get_nombre_instruccion(repartidor->instruccion));

	if(repartidor->instruccion == DESCANSAR) {

		// Descansa, y pasa a READY
//		log_info(logger, "Repartidor N°%d | Entra en descanso %d segundos", repartidor->id, repartidor->tiempoDescanso);
		log_info(logger, "Repartidor N°%d | Pasa a BLOCK | Por descanso", pcb->id_repartidor);
		sleep(repartidor->tiempoDescanso);
		repartidor->frecuenciaDescanso = atoi(app_conf.frecuencias_descanso[repartidor->id - 1]);
		repartidor->instruccion = instruccion_anterior;

		pthread_mutex_lock(&mutex_listos);
		list_add(listos, pcb);
		pthread_mutex_unlock(&mutex_listos);

		sem_post(&sem_ready);

	}

	if(repartidor->instruccion == ESPERAR_PEDIDO) {

		log_info(logger, "Repartidor N°%d | Pasa a BLOCK | Esperar pedido N°%d", pcb->id_repartidor, pcb->id_pedido);

		if(list_size(restaurantesConectados) > 0) {

			sem_wait(&pcb->sem_pedido_listo);

		}

		retirarPedido(pcb);

	}
}

void retirarPedido(t_pcb* pcb) {

	t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
	repartidor->instruccion = ENTREGAR_PEDIDO;
	repartidor->objetivo_posX = pcb->cliente_posX;
	repartidor->objetivo_posY = pcb->cliente_posY;

	int esElPCB(t_pcb* pcb) {
		return pcb->id_repartidor == repartidor->id;
	}

	pthread_mutex_lock(&mutex_bloqueados);
	list_remove_by_condition(bloqueados, (void*)esElPCB);
	pthread_mutex_unlock(&mutex_bloqueados);

	pthread_mutex_lock(&mutex_listos);
	list_add(listos, pcb);
	pthread_mutex_unlock(&mutex_listos);

	log_info(logger, "Repartidor N°%d | Pasa a READY | Retiró pedido N°%d", pcb->id_repartidor, pcb->id_pedido);

	sem_post(&sem_ready);

}

t_pcb* sacarPCBDeEjecutando(t_repartidor* repartidor) {

	int esElPCB(t_pcb* pcb) {

		return pcb->id_repartidor == repartidor->id;
	}

	pthread_mutex_lock(&mutex_ejecutando);
	t_pcb* pcb = list_remove_by_condition(ejecutando, (void*)esElPCB);
	pthread_mutex_unlock(&mutex_ejecutando);

	sem_post(&sem_limite_exec);

	return pcb;
}

void finalizarPCB(t_repartidor* repartidor) {

	t_pcb* pcb = sacarPCBDeEjecutando(repartidor);

	pthread_mutex_lock(&mutex_finalizados);
	list_add(finalizados, pcb);
	pthread_mutex_unlock(&mutex_finalizados);

	int socket_comanda = conectar_a_comanda_simple();
	t_finalizar_pedido* finalizar_pedido = calloc(1, sizeof(t_finalizar_pedido));
	finalizar_pedido->id_pedido = pcb->id_pedido;
	strcpy(finalizar_pedido->restaurante, pcb->restaurante);
	enviar_finalizar_pedido(finalizar_pedido, socket_comanda, logger);
	uint32_t resultado;
	t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
	if (tipo_rta == RTA_FINALIZAR_PEDIDO){
		resultado = recibir_entero(socket_comanda, logger);
		log_info(logger, "[RTA_FINALIZAR_PEDIDO]Resultado Comanda: %s",resultado? "OK":"FAIL");
	}
	close(socket_comanda);

	if(resultado) {
		t_info_cliente* cliente = buscarClientePorPedido(pcb->id_pedido);
		enviar_finalizar_pedido(finalizar_pedido, cliente->socketEscucha, logger);
		tipo_rta = recibir_tipo_mensaje(cliente->socketEscucha, logger);
		if (tipo_rta == RTA_FINALIZAR_PEDIDO){
			resultado = recibir_entero(cliente->socketEscucha, logger);
			log_info(logger, "[RTA_FINALIZAR_PEDIDO]Resultado Cliente: %s",resultado? "OK":"FAIL");
		}
	}
	free(finalizar_pedido);

	log_info(logger, "Repartidor N°%d | Pasa a EXIT | Pedido N°%d entregado", pcb->id_repartidor, pcb->id_pedido);

	disponibilizar_repartidor(repartidor);

	liberarPCB(pcb);
}

void liberarPCB(t_pcb* pcb_a_liberar) {

	int esElPCB(t_pcb* pcb) {

		return pcb->id_pedido == pcb_a_liberar->id_pedido;
	}

	pthread_mutex_lock(&mutex_finalizados);
	list_remove_by_condition(finalizados, (void*)esElPCB);
	pthread_mutex_unlock(&mutex_finalizados);

	sem_destroy(&pcb_a_liberar->sem_pedido_listo);
	free(pcb_a_liberar);
}

void notificar_pedido_listo(int id_pedido) {

	t_pcb* pcb = buscarPCB(id_pedido);
	sem_post(&pcb->sem_pedido_listo);
}

t_pcb* buscarPCB(int id_pedido) {

	int esElPCB(t_pcb* pcb) {

			return pcb->id_pedido == id_pedido;
		}

	t_pcb* pcb = list_find(bloqueados, (void*)esElPCB);

	if(pcb == NULL) {
		pcb = list_find(ejecutando, (void*)esElPCB);
	}

	return pcb;
}

int conectar_a_comanda_simple(){

	int socket_comanda = crear_conexion(app_conf.ip_comanda, app_conf.puerto_comanda);
	if (socket_comanda == -1){
		log_error(logger, "No se pudo conectar a Comanda");
		puts("No se pudo conectar a Comanda.");
		exit(-1);//Termina el programa
	}
	return socket_comanda;
}
