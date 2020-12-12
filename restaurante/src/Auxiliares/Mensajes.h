/*
 * mensajes.h
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MENSAJES_H_
#define AUXILIARES_MENSAJES_H_

#include <protocolo.h>
#include "Conexion.h"

t_cliente* buscar_cliente(char*);//Busca por ID un cliente en la lista de clientes conectados
void procesar_handshake_inicial(t_handshake_inicial*, int socket_emisor);

t_rta_consultar_platos* procesar_consultar_platos(void);
uint32_t procesar_crear_pedido(void);
uint32_t procesar_anadir_plato(t_anadir_plato*);
uint32_t procesar_confirmar_pedido(uint32_t);
t_rta_consultar_pedido* procesar_consultar_pedido(uint32_t);
void enviar_plato_listo_a_sindicato(uint32_t id_pedido,char plato[L_PLATO]);
void enviar_terminar_pedido_a_sindicato(uint32_t id_pedido);
void enviar_plato_listo_a_modulo_solicitante(uint32_t id_pedido,char plato[L_PLATO]);
void enviar_finalizar_pedido_a_cliente(uint32_t id_pedido);
t_cliente* obtener_cliente_con_id_pedido(uint32_t id_pedido);

#endif /* AUXILIARES_MENSAJES_H_ */
