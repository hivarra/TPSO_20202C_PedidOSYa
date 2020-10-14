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
			puts("Se desconecto del proceso server\n");
			close(socket_escucha);
			pthread_exit(NULL);
		}

		switch(tipo_mensaje){

			case RTA_CONSULTAR_RESTAURANTES:;
				t_rta_consultar_restaurantes* rta_consultar_restaurantes = recibir_rta_consultar_restaurantes(socket_escucha,logger);
				log_info(logger,"CANTIDAD_RESTAURANTES:%d",rta_consultar_restaurantes->cantRestaurantes);
				imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"RESTAURANTES");

				break;
			default:
				puts("No se reconoce el tipo de mensaje recibido\n");
				break;
		}
	}
}

void enviar_info_socket_escucha(int socket){

	t_socket_escucha* socket_escucha_info = malloc(sizeof(t_socket_escucha));
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
