/*
 * Mensajes.h
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MENSAJES_H_
#define AUXILIARES_MENSAJES_H_

#include "../sindicato.h"

t_rta_consultar_platos* procesar_consultar_platos(char*);
uint32_t procesar_guardar_pedido(t_guardar_pedido*);
uint32_t procesar_guardar_plato(t_guardar_plato*);
uint32_t procesar_confirmar_pedido(t_confirmar_pedido*);
t_rta_obtener_pedido* procesar_obtener_pedido(t_obtener_pedido*);
t_rta_obtener_restaurante* procesar_obtener_restaurante(char*);
uint32_t procesar_plato_listo(t_plato_listo*);
uint32_t procesar_terminar_pedido(t_terminar_pedido*);
t_rta_obtener_receta* procesar_obtener_receta(char*);

#endif /* AUXILIARES_MENSAJES_H_ */
