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
			log_warning(logger, "[Conexion] Se desconecto del proceso server.");
			puts("Se desconecto del proceso server.");
			close(socket_escucha);
			pthread_exit(NULL);
		}

		switch(tipo_mensaje){

			case PLATO_LISTO:{
				t_plato_listo* recibido = recibir_plato_listo(socket_escucha, logger);
				log_info(logger, "[ACTUALIZACIONES] Se recibio PLATO_LISTO: Restaurante: %s, ID_Pedido: %d, Plato: %s.", recibido->restaurante, recibido->id_pedido, recibido->plato);
				free(recibido);
				enviar_entero(RTA_PLATO_LISTO, 1, socket_escucha, logger);
			}
			break;
			case FINALIZAR_PEDIDO:{
				t_finalizar_pedido* recibido = recibir_finalizar_pedido(socket_escucha, logger);
				log_info(logger, "[ACTUALIZACIONES] Se recibio FINALIZAR_PEDIDO: Restaurante: %s, ID_Pedido: %d.", recibido->restaurante, recibido->id_pedido);
				free(recibido);
				enviar_entero(RTA_FINALIZAR_PEDIDO, 1, socket_escucha, logger);
			}
			break;
			default:
				log_error(logger, "[ACTUALIZACIONES] No se reconoce el tipo de mensaje recibido.");
				break;
		}
	}
}

void crear_hilo_conexion_escucha(){
	pthread_create(&hilo_escucha, NULL, (void*)iniciar_conexion_escucha, NULL);
	pthread_detach(hilo_escucha);
}
