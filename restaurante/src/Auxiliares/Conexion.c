/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Conexion.h"

int *conectar_a_app() {

	int socket_cliente;

	if((socket_cliente = definirSocket(logger))<= 0)
	{
		log_error(logger, "CONEXION | No se pudo definir socket.");
		return -1;
	}

	if(conectarseAServidor(socket_cliente, restaurante_conf.ip_app, restaurante_conf.puerto_app, logger) <= 0) {
		log_error(logger, "CONEXION | No se pudo conectar a servidor %s", get_nombre_proceso(APP));
		return -1;
	}

	t_restaurante* restaurante = malloc(sizeof(t_restaurante));
	strcpy(restaurante->nombre, restaurante_conf.nombre_restaurante);
	restaurante->posX = 0; // TODO: cambiar por info de Sindicato
	restaurante->posY = 0; // TODO: cambiar por info de Sindicato

	enviarMensaje(RESTAURANTE, 0, HANDSHAKE, sizeof(t_restaurante), restaurante, socket_cliente, APP, logger);
	t_mensaje* msg = recibirMensaje(socket_cliente, logger);
	destruirMensaje(msg);
//	loggear(logger,LOG_LEVEL_INFO, "FIN Handshake(%d)", tipoProcesoReceptor);
	return socket_cliente;

}
