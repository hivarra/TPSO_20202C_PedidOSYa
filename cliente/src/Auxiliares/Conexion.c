/*
 * Conexion.c
 *
 *  Created on: 13 oct. 2020
 *      Author: utnso
 */
#include "Conexion.h"

int conectar_a_server(){

	int socket_aux;

	socket_aux = crear_conexion(cliente_config.ip, cliente_config.puerto);

	//socket_aux = conectar_a_servidor(cliente_config.ip, cliente_config.puerto, logger);

	return socket_aux;
}

void enviar_info_socket_envio(){

	t_socket_envio* param_socket_envio = calloc(1,sizeof(t_socket_envio));
	strcpy(param_socket_envio->id, cliente_config.id_cliente);
	param_socket_envio->posX = cliente_config.posicion_x;
	param_socket_envio->posY = cliente_config.posicion_y;
	param_socket_envio->tipoProceso = CLIENTE;
	enviar_socket_envio(param_socket_envio, socket_envio, logger);
	free(param_socket_envio);
}

void realizar_handshake(){

	int socket_handshake = conectar_a_server();

	if (socket_handshake == -1){
		puts("No se pudo conectar al servidor. Se cierra el modulo Cliente\n");
		terminar_programa(-1);
	}

	enviar_mensaje_vacio(HANDSHAKE, socket_handshake, logger);
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
	if(tipo_mensaje == RTA_HANDSHAKE){
		uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
		tipo_proceso_server = respuesta_entero;
		log_info(logger, "Fin Handshake con proceso %s", get_nombre_proceso(tipo_proceso_server));
	}
	close(socket_handshake);
}

void inicializar_conexion(){

	/*INICIALIZO SOCKET PARA LUEGO SABER SI HAY QUE CERRARLOS*/
	socket_envio = -1;
	socket_escucha = -1;

	/*REALIZO HANDSHAKE*/
	realizar_handshake();
	/*SI ME CONECTO A APP/RESTAURANTE, ENVIO INFO SOCKET_ENVIO Y CREO HILO PARA SOCKET_ESCUCHA*/
	if(tipo_proceso_server == APP || tipo_proceso_server == RESTAURANTE){
		socket_envio = conectar_a_server();
		enviar_info_socket_envio();
		crear_hilo_conexion_escucha();
	}
	/*SI ME CONECTO A COMANDA/SINDICATO, ME COMUNICO POR UN NUEVO SOCKET BIDIRECCIONAL POR CADA MENSAJE*/
}

void terminar_programa(int result){

	destruir_logger(logger);
	destruir_config(config);

	if (socket_envio != -1)
		close(socket_envio);

	if (socket_escucha != -1){
		close(socket_escucha);
		pthread_cancel(hilo_escucha);
		puts("Ending listening thread...\n");
		sleep(2);
	}

	puts("Fin CLIENTE\n");
	exit(result);
}
