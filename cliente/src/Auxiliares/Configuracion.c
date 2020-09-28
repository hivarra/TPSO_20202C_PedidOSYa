/*
 * Configuracion.c
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#include "Configuracion.h"

void cargarConfigCliente() {

	config = config_create(PATH);

	if (config_has_property(config, "IP"))
		cliente_config.ip = config_get_string_value(config, "IP");

	if (config_has_property(config, "PUERTO"))
		cliente_config.puerto = config_get_int_value(config, "PUERTO");

	if (config_has_property(config, "ARCHIVO_LOG"))
		cliente_config.archivo_log = config_get_string_value(config,"ARCHIVO_LOG");

	if (config_has_property(config, "POSICION_X"))
		cliente_config.posicion_x = config_get_int_value(config, "POSICION_X");

	if (config_has_property(config, "POSICION_Y"))
		cliente_config.posicion_y = config_get_int_value(config, "POSICION_Y");

	if (config_has_property(config, "ID_CLIENTE"))
			cliente_config.id_cliente = config_get_int_value(config, "ID_CLIENTE");
}
