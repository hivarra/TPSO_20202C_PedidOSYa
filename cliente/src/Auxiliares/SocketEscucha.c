/*
 * SocketEscucha.c
 *
 *  Created on: 13 oct. 2020
 *      Author: utnso
 */
#include "SocketEscucha.h"

void iniciar_conexion_escucha(){

	while(1){

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_escucha, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del proceso server");
			puts("Se desconecto del proceso server");
			close(socket_escucha);
			pthread_exit(NULL);
		}

		log_info(logger, "[ACTUALIZACIONES] Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case PLATO_LISTO:{
				t_plato_listo* recibido = recibir_plato_listo(socket_escucha, logger);
				log_info(logger, "Restaurante: %s, ID_Pedido: %d, Plato: %s", recibido->restaurante, recibido->id_pedido, recibido->plato);
				free(recibido);
				enviar_entero(RTA_PLATO_LISTO, 1, socket_escucha, logger);
			}
			break;
			case FINALIZAR_PEDIDO:{
				t_finalizar_pedido* recibido = recibir_finalizar_pedido(socket_escucha, logger);
				log_info(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
				free(recibido);
				enviar_entero(RTA_FINALIZAR_PEDIDO, 1, socket_escucha, logger);
			}
			break;
			default:
				puts("No se reconoce el tipo de mensaje recibido");
				break;
		}
	}
}

void enviar_info_socket_escucha(int socket){

	t_socket_escucha* socket_escucha_info = calloc(1,sizeof(t_socket_escucha));
	strcpy(socket_escucha_info->id, cliente_config.id_cliente);
	socket_escucha_info->tipoProceso = CLIENTE;
	enviar_socket_escucha(socket_escucha_info, socket, logger);
	free(socket_escucha_info);
}

void crear_hilo_conexion_escucha(){

	socket_escucha = conectar_a_server();
	enviar_info_socket_escucha(socket_escucha);

	pthread_create(&hilo_escucha, NULL, (void*)iniciar_conexion_escucha, NULL);
	pthread_detach(hilo_escucha);
}
