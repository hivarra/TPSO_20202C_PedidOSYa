#include "configuracion.h"

void cargarConfigSindicato(char* path) {

	config = config_create(path);

	if (config_has_property(config, "PUNTO_MONTAJE_AFIP"))
		sindicato_conf.punto_montaje_afip = config_get_string_value(config,"PUNTO_MONTAJE_AFIP");
		string_trim_right(&sindicato_conf.punto_montaje_afip);

	if (config_has_property(config, "IP_RESTAURANTE"))
		sindicato_conf.ip_restaurante = config_get_string_value(config, "IP_RESTAURANTE");
		string_trim_right(&sindicato_conf.ip_restaurante);

	if (config_has_property(config, "PUERTO_RESTAURANTE"))
		sindicato_conf.puerto_broker = config_get_int_value(config, "PUERTO_RESTAURANTE");

	if (config_has_property(config, "BLOCK_SIZE"))
		sindicato_conf.block_size = config_get_int_value(config, "BLOCK_SIZE");

	if (config_has_property(config, "BLOCKS"))
		sindicato_conf.blocks = config_get_int_value(config, "BLOCKS");

	if (config_has_property(config, "MAGIC_NUMBER"))
		sindicato_conf.magic_number = config_get_string_value(config, "MAGIC_NUMBER");

	if (config_has_property(config, "ID_PROCESO"))
		sindicato_conf.id_proceso = config_get_int_value(config, "ID_PROCESO");

}
