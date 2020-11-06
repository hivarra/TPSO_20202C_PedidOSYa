#include "Configuracion.h"

void cargarConfigSindicato(char* path) {

	config = config_create(path);

	if (!config_has_property(config, "PUNTO_MONTAJE")){
		puts("Error al leer PUNTO_MONTAJE de archivo de configuracion.");
		exit(2);
	}
	if (!config_has_property(config, "PUERTO_ESCUCHA")){
		puts("Error al leer PUERTO_ESCUCHA de archivo de configuracion.");
		exit(2);
	}
	if (!config_has_property(config, "ARCHIVO_LOG")){
		puts("Error al leer ARCHIVO_LOG de archivo de configuracion.");
		exit(2);
	}
	if (!config_has_property(config, "BLOCK_SIZE")){
		puts("Error al leer BLOCK_SIZE de archivo de configuracion.");
		exit(2);
	}
	if (!config_has_property(config, "BLOCKS")){
		puts("Error al leer BLOCKS de archivo de configuracion.");
		exit(2);
	}
	if (!config_has_property(config, "MAGIC_NUMBER")){
		puts("Error al leer MAGIC_NUMBER de archivo de configuracion.");
		exit(2);
	}

	free(path);
}

void cargar_logger_sindicato() {
	int carpeta_creada;

	carpeta_creada = crear_carpeta_log(config_get_string_value(config, "ARCHIVO_LOG"));
	if (carpeta_creada){
		logger = log_create(config_get_string_value(config, "ARCHIVO_LOG"), "Sindicato", 0, LOG_LEVEL_TRACE);
		log_trace(logger, "*************** NUEVO LOG ***************");
	}
	else{
		puts("Error al crear la carpera del log");
		exit(2);
	}
}

void mostrar_propiedades() {

	log_trace(logger,"Propiedades cargadas:");
	log_trace(logger,"Archivo log: %s", config_get_string_value(config, "ARCHIVO_LOG"));
	log_trace(logger,"Punto montaje: %s", config_get_string_value(config, "PUNTO_MONTAJE"));
	log_trace(logger,"Puerto escucha: %s", config_get_string_value(config, "PUERTO_ESCUCHA"));
	//Las propiedades restantes se loguaran de acuerdo a si existe el FS.
}
