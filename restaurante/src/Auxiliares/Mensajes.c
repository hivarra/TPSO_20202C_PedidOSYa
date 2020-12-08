/*
 * Mensajes.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */
#include "Mensajes.h"

t_cliente* buscar_cliente(char* id){

	bool _busqueda_id(t_cliente* buscado){
		return (strcmp(id, buscado->nombre) == 0);
	}

	t_cliente* cliente_buscado = list_find(clientes_conectados, (void*)_busqueda_id);

	return cliente_buscado;
}

void procesar_handshake_inicial(t_handshake_inicial* msg_handshake_inicial, int socket_emisor){

	t_cliente* new_client = calloc(1,sizeof(t_cliente));
	strcpy(new_client->nombre, msg_handshake_inicial->id);
	new_client->socket_escucha = socket_emisor;
	new_client->pedidos = list_create();
	pthread_mutex_init(&new_client->mutex_pedidos,NULL);

	pthread_mutex_lock(&mutex_cliente_conectados);
	list_add(clientes_conectados, new_client);
	pthread_mutex_unlock(&mutex_cliente_conectados);
}

t_rta_consultar_platos* procesar_consultar_platos(){
	t_rta_consultar_platos* respuesta = malloc(sizeof(t_rta_consultar_platos));
	respuesta->cantPlatos = 0;
	respuesta->platos = list_create();

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Consultar PLatos] No se pudo conectar a Sindicato");
	else{
		char* msg_consultar_platos = calloc(1,L_ID);
		strcpy(msg_consultar_platos, restaurante_conf.nombre_restaurante);
		log_info(logger, "[Consultar Platos] Se consulta a Sindicato los platos.");
		enviar_consultar_platos(msg_consultar_platos, socket_sindicato, logger);
		free(msg_consultar_platos);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_CONSULTAR_PLATOS){
			list_destroy(respuesta->platos);
			free(respuesta);
			respuesta = recibir_rta_consultar_platos(socket_sindicato, logger);
		}
		close(socket_sindicato);
	}
	return respuesta;
}

uint32_t procesar_crear_pedido(){
	uint32_t nuevo_id = 0;

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Crear Pedido] No se pudo conectar a Sindicato");
	else{
		pthread_mutex_lock(&mutex_id_pedidos);
		id_pedidos++;
		t_guardar_pedido* msg_guardar_pedido = calloc(1,sizeof(t_guardar_pedido));
		strcpy(msg_guardar_pedido->restaurante, restaurante_conf.nombre_restaurante);
		msg_guardar_pedido->id_pedido = id_pedidos;
		log_info(logger, "[Crear Pedido] Se envia a Sindicato GUARDAR_PEDIDO con ID %d.", id_pedidos);
		enviar_guardar_pedido(msg_guardar_pedido, socket_sindicato, logger);
		free(msg_guardar_pedido);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_GUARDAR_PEDIDO){
			uint32_t resultado = recibir_entero(socket_sindicato, logger);
			log_info(logger, "[RTA_GUARDAR_PEDIDO]Resultado: %s",resultado? "OK":"FAIL");
			if (!resultado)
				id_pedidos--;
			else
				nuevo_id = id_pedidos;
		}
		pthread_mutex_unlock(&mutex_id_pedidos);
		close(socket_sindicato);
	}
	return nuevo_id;
}

uint32_t procesar_anadir_plato(t_anadir_plato* msg_anadir_plato){
	uint32_t resultado = 0;

	pthread_mutex_lock(&mutex_id_pedidos);
	uint32_t id_maximo = id_pedidos;
	pthread_mutex_unlock(&mutex_id_pedidos);

	if(msg_anadir_plato->id_pedido <= id_maximo){
		int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
		if (socket_sindicato == -1)
			log_warning(logger, "[Anadir Plato] No se pudo conectar a Sindicato");
		else{
			t_guardar_plato* msg_guardar_plato = calloc(1,sizeof(t_guardar_plato));
			strcpy(msg_guardar_plato->restaurante, restaurante_conf.nombre_restaurante);
			msg_guardar_plato->id_pedido = msg_anadir_plato->id_pedido;
			strcpy(msg_guardar_plato->plato, msg_anadir_plato->plato);
			msg_guardar_plato->cantPlato = 1;
			log_info(logger, "[Anadir Plato] Se envia a Sindicato GUARDAR_PLATO ID: %d, Plato: %s, Cant.: 1", msg_anadir_plato->id_pedido, msg_anadir_plato->plato);
			enviar_guardar_plato(msg_guardar_plato, socket_sindicato, logger);
			free(msg_guardar_plato);
			t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
			if (tipo_rta == RTA_GUARDAR_PLATO)
				resultado = recibir_entero(socket_sindicato, logger);
			close(socket_sindicato);
		}
	}
	else
		log_warning(logger, "[Anadir Plato] Pedido %d inexistente.", msg_anadir_plato->id_pedido);
	return resultado;
}
void imprimir_lista_pasos(t_list* lista_pasos){
	void imprimir_paso(t_paso_receta* paso_receta){
		log_info(logger,"PASO:%s",paso_receta->accion);
		log_info(logger,"TIEMPO:%d",paso_receta->tiempo);
	}
	list_iterate(lista_pasos,(void*)imprimir_paso);
}

void imprimir_lista_comida(t_list* lista_comidas){
	void imprimir_comida(t_comida* comida){
		log_info(logger,"NOMBRE_COMIDA:%s",comida->nombre);
		log_info(logger,"CANTIDAD_TOTAL:%d",comida->cantTotal);
		log_info(logger,"CANTIDAD_LISTA:%d",comida->cantLista);
	}
	list_iterate(lista_comidas,(void*)imprimir_comida);
}
void enviar_obtener_pasos_receta(t_args_aux* args_aux){
	/*SE ITERA LA LISTA DE COMIDAS DEL PEDIDO*/
	/*SE OBTIENE LISTA DE PASOS Y TIEMPOS DE CADA COMIDA*/
	void obtener_pasos_receta_de_comida(t_comida* plato){
		int socket_new = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
			if (socket_new == -1)
				log_warning(logger, "[Obtener Receta] No se pudo conectar a Sindicato");
			else{
				enviar_obtener_receta(plato->nombre,socket_new,logger);
				t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_new, logger);
				if(tipo_mensaje == RTA_OBTENER_RECETA){
					t_rta_obtener_receta* rta_obtener_receta = recibir_rta_obtener_receta(socket_new,logger);
					log_info(logger,"[RTA_OBTENER_RECETA] NOMBRE_PLATO:%s",rta_obtener_receta->nombre);
					log_info(logger,"[RTA_OBTENER_RECETA] CANTIDAD_PASOS:%d",rta_obtener_receta->cantPasos);
					log_info(logger,"[RTA_OBTENER_RECETA] LISTA PASOS:");
					imprimir_lista_pasos(rta_obtener_receta->pasos);

					int buscar_comida(t_comida* comida){
						return string_equals_ignore_case(comida->nombre,rta_obtener_receta->nombre);
					}

					t_comida* comida_buscada = list_find(args_aux->rta_obtener_pedido->comidas,(void*)buscar_comida);

					crear_y_agregar_pcb_a_cola_ready(args_aux->id_pedido,rta_obtener_receta,comida_buscada->cantTotal);
					free(rta_obtener_receta);
				}
				close(socket_new);
			}
	}
	list_iterate(args_aux->rta_obtener_pedido->comidas,(void*)obtener_pasos_receta_de_comida);

	inicializar_ciclo_planificacion(args_aux->rta_obtener_pedido->comidas);

	free(args_aux);
}
void obtener_pedido(uint32_t* id_pedido){
	int socket_new = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
		if (socket_new == -1)
			log_warning(logger, "[OBTENER_PEDIDO] No se pudo conectar a Sindicato");
		else{
			t_obtener_pedido* msg_obtener_pedido = malloc(sizeof(t_obtener_pedido));
			msg_obtener_pedido->id_pedido = *id_pedido;
			strcpy(msg_obtener_pedido->restaurante,restaurante_conf.nombre_restaurante);
			enviar_obtener_pedido(msg_obtener_pedido,socket_new,logger);
			free(msg_obtener_pedido);

			t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_new, logger);
			if(tipo_mensaje == RTA_OBTENER_PEDIDO){
				t_rta_obtener_pedido* rta_obtener_pedido = recibir_rta_obtener_pedido(socket_new,logger);
				log_info(logger,"[RTA_OBTENER_PEDIDO] ESTADO_PEDIDO:%d",rta_obtener_pedido->estado);
				log_info(logger,"[RTA_OBTENER_PEDIDO] CANTIDAD_COMIDAS:%d",rta_obtener_pedido->cantComidas);
				log_info(logger,"[RTA_OBTENER_PEDIDO] LISTA_COMIDAS:");
				imprimir_lista_comida(rta_obtener_pedido->comidas);

				t_args_aux* args_aux = malloc(sizeof(t_args_aux));
				args_aux->rta_obtener_pedido = rta_obtener_pedido;
				args_aux->id_pedido = *id_pedido;

				pthread_t hilo_enviar_obtener_receta;
				pthread_create(&hilo_enviar_obtener_receta,NULL,(void*)enviar_obtener_pasos_receta,args_aux);
				pthread_detach(hilo_enviar_obtener_receta);
			}
			close(socket_new);
		}
}

uint32_t procesar_confirmar_pedido(t_confirmar_pedido* msg_confirmar_pedido){
	t_resultado resultado = FAIL;

	int socket_new = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_new == -1)
		log_warning(logger, "[PROCESAR_CONFIRMAR_PEDIDO] No se pudo conectar a Sindicato");
	else{
		//SE ENVIA CONFIRMAR_PEDIDO A SINDICATO
		strcpy(msg_confirmar_pedido->restaurante,restaurante_conf.nombre_restaurante);
		enviar_confirmar_pedido(msg_confirmar_pedido,socket_new,logger);
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_new, logger);
		if(tipo_mensaje == RTA_CONFIRMAR_PEDIDO){
			uint32_t resultado = recibir_entero(socket_new,logger);
			log_info(logger,"[RTA_CONFIRMAR_PEDIDO] RESULTADO:%s",resultado?"OK":"FAIL");

			if(resultado){
				pthread_t hilo_enviar_obtener_pedido;
				pthread_create(&hilo_enviar_obtener_pedido,NULL,(void*)obtener_pedido,&msg_confirmar_pedido->id_pedido);
				pthread_detach(hilo_enviar_obtener_pedido);
			}
		}
		close(socket_new);
	}
	return resultado;
}

t_rta_consultar_pedido* procesar_consultar_pedido(uint32_t id_pedido){
	t_rta_consultar_pedido* respuesta = calloc(1,sizeof(t_rta_consultar_pedido));
	strcpy(respuesta->restaurante, restaurante_conf.nombre_restaurante);
	respuesta->cantComidas = 0;
	respuesta->estado = 0;
	respuesta->comidas = list_create();

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Consultar Pedido] No se pudo conectar a Sindicato");
	else{
		t_obtener_pedido* msg_obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
		strcpy(msg_obtener_pedido->restaurante, restaurante_conf.nombre_restaurante);
		msg_obtener_pedido->id_pedido = id_pedido;
		log_info(logger, "[Consultar Pedido] Se consulta a Sindicato la info del pedido %d.", id_pedido);
		enviar_obtener_pedido(msg_obtener_pedido, socket_sindicato, logger);
		free(msg_obtener_pedido);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			t_rta_obtener_pedido* respuesta_obt_ped = recibir_rta_obtener_pedido(socket_sindicato, logger);
			respuesta->cantComidas = respuesta_obt_ped->cantComidas;
			list_destroy(respuesta->comidas);
			respuesta->comidas = respuesta_obt_ped->comidas;
			respuesta->estado = respuesta_obt_ped->estado;
			free(respuesta_obt_ped);
		}
		close(socket_sindicato);
	}
	return respuesta;
}
bool es_pedido_de_app(uint32_t id_pedido){
	bool es_pedido(uint32_t* id_pedido_aux){
			return *id_pedido_aux == id_pedido;
	}
	return list_any_satisfy(lista_pedidos_app,(void*)es_pedido);
}
bool cliente_tiene_pedido(t_cliente* cliente,uint32_t id_pedido){
	bool ret=false;

	bool es_pedido(uint32_t* id_pedido_aux){

		return *id_pedido_aux == id_pedido;
	}
	pthread_mutex_lock(&cliente->mutex_pedidos);
	ret=list_any_satisfy(cliente->pedidos,(void*)es_pedido);
	pthread_mutex_unlock(&cliente->mutex_pedidos);
	return ret;
}
t_cliente* obtener_cliente_con_id_pedido(uint32_t id_pedido){
	t_cliente* cliente_ret=NULL;

	for(int i=0;i<list_size(clientes_conectados);i++){
		t_cliente* cliente_aux = list_get(clientes_conectados,i);

		if(cliente_tiene_pedido(cliente_aux,id_pedido)){
			cliente_ret = cliente_aux;
			break;
		}
	}
	return cliente_ret;
}
void enviar_actualizacion_plato_listo(t_plato_listo* plato_listo){
	uint32_t respuesta_plato_listo = FAIL;
	if(lista_pedidos_app != NULL){
		if(es_pedido_de_app(plato_listo->id_pedido)){
			enviar_plato_listo(plato_listo,socket_escucha,logger);
			log_info(logger, "[ENVIAR_PLATO_LISTO_A_MODULO_SOLICITANTE] Se envia actualizacion de PLATO_LISTO a App. Info enviada: RESTAURANTE:%s,PLATO:%s,ID_PEDIDO:%d."
					,plato_listo->restaurante,plato_listo->plato,plato_listo->id_pedido);
			respuesta_plato_listo = recibir_entero(socket_escucha,logger);
			log_info(logger, "[ENVIAR_PLATO_LISTO_A_MODULO_SOLICITANTE] Se recibe respuesta:%s",respuesta_plato_listo?"OK":"FAIL");
		}
	}
	else{
		pthread_mutex_lock(&mutex_cliente_conectados);
		t_cliente* cliente = obtener_cliente_con_id_pedido(plato_listo->id_pedido);
		pthread_mutex_unlock(&mutex_cliente_conectados);
		if(cliente != NULL){
			enviar_plato_listo(plato_listo,cliente->socket_escucha,logger);
			log_info(logger,"[ENVIAR_PLATO_LISTO_A_MODULO_SOLICITANTE] Se envia actualizacion de PLATO_LISTO a Cliente:%s. Info enviada: RESTAURANTE:%s,PLATO:%s,ID_PEDIDO:%d."
								,cliente->nombre,plato_listo->restaurante,plato_listo->plato,plato_listo->id_pedido);
			respuesta_plato_listo = recibir_entero(cliente->socket_escucha,logger);//RECIBO PARA RESPETAR PROTOCOLO, NO SE USA
//			log_info(logger, "[ENVIAR_PLATO_LISTO_A_MODULO_SOLICITANTE] Se recibe respuesta:%s",respuesta_plato_listo?"OK":"FAIL");
		}
		else
			log_info(logger,"[ENVIAR_PLATO_LISTO_A_MODULO_SOLICITANTE] Error al enviar actualizacion de PLATO_LISTO. No se encontró socket_escucha de app/cliente asociado a ID_PEDIDO:%d",plato_listo->id_pedido);
	}
	free(plato_listo);
}
void enviar_plato_listo_a_modulo_solicitante(uint32_t id_pedido,char plato[L_PLATO]){
	t_plato_listo* msg_plato_listo = calloc(1,sizeof(t_plato_listo));
	strcpy(msg_plato_listo->restaurante, restaurante_conf.nombre_restaurante);
	strcpy(msg_plato_listo->plato, plato);
	msg_plato_listo->id_pedido = id_pedido;

	pthread_t hilo_enviar_plato_listo;
	pthread_create(&hilo_enviar_plato_listo,NULL,(void*)enviar_actualizacion_plato_listo,msg_plato_listo);
	pthread_detach(hilo_enviar_plato_listo);
}
void enviar_plato_listo_a_sindicato(uint32_t id_pedido,char plato[L_PLATO]){
	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[ENVIAR_PLATO_LISTO_A_SINDICATO] No se pudo conectar a Sindicato");
	else{
		t_plato_listo* msg_plato_listo = calloc(1,sizeof(t_plato_listo));
		strcpy(msg_plato_listo->restaurante, restaurante_conf.nombre_restaurante);
		strcpy(msg_plato_listo->plato, plato);
		msg_plato_listo->id_pedido = id_pedido;
		log_info(logger, "[ENVIAR_PLATO_LISTO_A_SINDICATO] Se envia a Sindicato la info de PLATO_LISTO restaurante:%s,plato:%s,id_pedido:%d.",msg_plato_listo->restaurante,msg_plato_listo->plato,msg_plato_listo->id_pedido);
		enviar_plato_listo(msg_plato_listo,socket_sindicato,logger);
		free(msg_plato_listo);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_PLATO_LISTO){
			uint32_t respuesta_plato_listo = recibir_entero(socket_sindicato, logger);
			log_info(logger, "[RTA_PLATO_LISTO] Se recibe respuesta:%s",respuesta_plato_listo?"OK":"FAIL");
		}
		close(socket_sindicato);
	}
}
void enviar_terminar_pedido_a_sindicato(uint32_t id_pedido){
	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[ENVIAR_TERMINAR_PEDIDO_A_SINDICATO] No se pudo conectar a Sindicato");
	else{
		t_terminar_pedido* msg_terminar_pedido = calloc(1,sizeof(t_terminar_pedido));
		strcpy(msg_terminar_pedido->restaurante, restaurante_conf.nombre_restaurante);
		msg_terminar_pedido->id_pedido = id_pedido;

		log_info(logger, "[ENVIAR_TERMINAR_PEDIDO_A_SINDICATO] Se envia a Sindicato TERMINAR_PEDIDO con RESTAURANTE:%s,ID_PEDIDO:%d.",msg_terminar_pedido->restaurante,msg_terminar_pedido->id_pedido);
		enviar_terminar_pedido(msg_terminar_pedido,socket_sindicato,logger);
		free(msg_terminar_pedido);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_TERMINAR_PEDIDO){
			uint32_t respuesta_terminar_pedido = recibir_entero(socket_sindicato, logger);
			log_info(logger, "[RTA_TERMINAR_PEDIDO] Se recibe respuesta:%s",respuesta_terminar_pedido?"OK":"FAIL");
		}
		close(socket_sindicato);
	}
}
void enviar_actualizacion_finalizar_plato(t_finalizar_pedido* finalizar_plato){
	uint32_t respuesta_finalizar_plato = FAIL;

	pthread_mutex_lock(&mutex_cliente_conectados);
	t_cliente* cliente = obtener_cliente_con_id_pedido(finalizar_plato->id_pedido);
	pthread_mutex_unlock(&mutex_cliente_conectados);
	if(cliente != NULL){
		enviar_finalizar_pedido(finalizar_plato,cliente->socket_escucha,logger);
		log_info(logger,"[ENVIAR_FINALIZAR_PLATO_A_CLIENTE] Se envia actualizacion de FINALIZAR_PLATO a Cliente:%s. Info enviada: RESTAURANTE:%s,ID_PEDIDO:%d."
							,cliente->nombre,finalizar_plato->restaurante,finalizar_plato->id_pedido);
		respuesta_finalizar_plato = recibir_entero(cliente->socket_escucha,logger);//RECIBO PARA RESPETAR PROTOCOLO, NO SE USA
	}
	else
		log_info(logger,"[ENVIAR_FINALIZAR_PLATO_A_CLIENTE] No existe cliente al cual enviar FINALIZAR_PLATO.Info: RESTAURANTE:%s,ID_PEDIDO:%d",finalizar_plato->restaurante,finalizar_plato->id_pedido);

	free(finalizar_plato);
}
void enviar_finalizar_pedido_a_cliente(uint32_t id_pedido){
	t_finalizar_pedido* msg_finalizar_plato = calloc(1,sizeof(t_finalizar_pedido));
	strcpy(msg_finalizar_plato->restaurante, restaurante_conf.nombre_restaurante);
	msg_finalizar_plato->id_pedido = id_pedido;

	pthread_t hilo_enviar_finalizar_plato;
	pthread_create(&hilo_enviar_finalizar_plato,NULL,(void*)enviar_actualizacion_finalizar_plato,msg_finalizar_plato);
	pthread_detach(hilo_enviar_finalizar_plato);
}
