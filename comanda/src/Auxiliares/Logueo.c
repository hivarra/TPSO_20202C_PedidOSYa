/*
 * Logueo.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */
#include "Logueo.h"

void loguear_error_carga_propiedad(char* propiedad) {
	char* str = string_new();
	char* descripcion = string_new();
	string_append(&descripcion,"Error carga propiedad: %s");
	string_append_with_format(&str, descripcion, propiedad);
	log_error(logger, str);
}

t_log* configurar_logger_comanda(char* nombreLog, char* nombreProceso) {
	t_log* logger = log_create(nombreLog, nombreProceso, false, LOG_LEVEL_TRACE);
	log_info(logger, "*************** NUEVO LOG ***************");
	return logger;
}

void cargar_logger_comanda(){
	logger=configurar_logger_comanda(comanda_conf.archivo_log, "comanda");
}
