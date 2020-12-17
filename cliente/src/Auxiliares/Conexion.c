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

	if (socket_aux  == -1){
		log_error(logger, "[Conexion] No se pudo conectar al servidor. Se cierra el modulo Cliente.");
		puts("No se pudo conectar al servidor. Se cierra el modulo Cliente.");
		terminar_programa(-1);
	}

	return socket_aux;
}

int realizar_handshake_inicial(){

	int socket_handshake = conectar_a_server();

	t_handshake_inicial* handshake_inicial = calloc(1,sizeof(t_handshake_inicial));
	strcpy(handshake_inicial->id, cliente_config.id_cliente);
	handshake_inicial->posX = cliente_config.posicion_x;
	handshake_inicial->posY = cliente_config.posicion_y;
	handshake_inicial->tipoProceso = CLIENTE;

	enviar_handshake_inicial(handshake_inicial, socket_handshake, logger);
	free(handshake_inicial);
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
	if(tipo_mensaje == RTA_HANDSHAKE){
		uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
		tipo_proceso_server = respuesta_entero;
		log_info(logger, "[Conexion] Se ha conectado exitosamente al modulo %s.", get_nombre_proceso(tipo_proceso_server));
	}

	return socket_handshake;
}

void inicializar_conexion(){

	/*INICIALIZO SOCKET PARA LUEGO SABER SI HAY QUE CERRARLOS*/
	socket_escucha = -1;

	/*REALIZO HANDSHAKE*/
	int socket_handshake = realizar_handshake_inicial();
	/*SI ME CONECTO A APP/RESTAURANTE, ENVIO INFO SOCKET_ENVIO Y CREO HILO PARA SOCKET_ESCUCHA*/
	if(tipo_proceso_server == APP || tipo_proceso_server == RESTAURANTE){
		socket_escucha = socket_handshake;//UTILIZO EL SOCKET DEL HANDSHAKE PARA RECIBIR ACTUALIZACIONES
		crear_hilo_conexion_escucha();
	}
	else
		close(socket_handshake);
	/*SI ME CONECTO A COMANDA/SINDICATO, CIERRO EL SOCKET HANDSHAKE Y ME COMUNICO POR UN NUEVO SOCKET BIDIRECCIONAL POR CADA MENSAJE*/
}

void terminar_programa(int result){

	destruir_logger(logger);
	destruir_config(config);

	if (socket_escucha != -1){
		close(socket_escucha);
		pthread_cancel(hilo_escucha);
		puts("\nFinishing listening thread...");
		sleep(2);
	}

	puts("\n================\nFin CLIENTE\n================");
	exit(result);
}
