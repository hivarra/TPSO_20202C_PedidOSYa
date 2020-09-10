#include "Configuracion.h"

void cargarConfigSindicato(char* path) {

	config = config_create(path);

	if (config_has_property(config, "PUNTO_MONTAJE"))
		sindicato_conf.punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE");
		string_trim_right(&sindicato_conf.punto_montaje);

	if (config_has_property(config, "PUERTO_ESCUCHA"))
		sindicato_conf.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");

	if (config_has_property(config, "BLOCK_SIZE"))
		sindicato_conf.block_size = config_get_int_value(config, "BLOCK_SIZE");

	if (config_has_property(config, "BLOCKS"))
		sindicato_conf.blocks = config_get_int_value(config, "BLOCKS");

	if (config_has_property(config, "MAGIC_NUMBER"))
		sindicato_conf.magic_number = config_get_string_value(config, "MAGIC_NUMBER");

}
void mostrar_propiedades() {

	log_info(logger,"Propiedades cargadas:");
	log_info(logger,"Punto montaje: %s", sindicato_conf.punto_montaje);
	log_info(logger,"Puerto escucha: %d", sindicato_conf.puerto_escucha);
	log_info(logger,"Blocks: %d", sindicato_conf.blocks);
	log_info(logger,"Block size: %d", sindicato_conf.block_size);
	log_info(logger,"Magic number: %s", sindicato_conf.magic_number);
}
