/*
 * Configuracion.c
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#include "Configuracion.h"

void cargarConfigCliente() {

	config = config_create(PATH);

	if (config_has_property(config, "IP_COMANDA"))
		cliente_config.ip_comanda = config_get_string_value(config, "IP_COMANDA");

	if (config_has_property(config, "PUERTO_COMANDA"))
		cliente_config.puerto_comanda = config_get_int_value(config, "PUERTO_COMANDA");

	if (config_has_property(config, "IP_RESTAURANTE"))
		cliente_config.ip_restaurante = config_get_string_value(config, "IP_RESTAURANTE");

	if (config_has_property(config, "PUERTO_RESTAURANTE"))
		cliente_config.puerto_restaurante = config_get_int_value(config, "PUERTO_RESTAURANTE");

	if (config_has_property(config, "IP_SINDICATO"))
		cliente_config.ip_sindicato = config_get_string_value(config, "IP_SINDICATO");

	if (config_has_property(config, "PUERTO_SINDICATO"))
		cliente_config.puerto_sindicato = config_get_int_value(config, "PUERTO_SINDICATO");

	if (config_has_property(config, "IP_APP"))
		cliente_config.ip_app = config_get_string_value(config, "IP_APP");

	if (config_has_property(config, "PUERTO_APP"))
		cliente_config.puerto_app = config_get_int_value(config, "PUERTO_APP");

	if (config_has_property(config, "ARCHIVO_LOG"))
		cliente_config.archivo_log = config_get_string_value(config,"ARCHIVO_LOG");

	if (config_has_property(config, "POSICION_X"))
		cliente_config.posicion_x = config_get_int_value(config, "POSICION_X");

	if (config_has_property(config, "POSICION_Y"))
		cliente_config.posicion_y = config_get_int_value(config, "POSICION_Y");

}
