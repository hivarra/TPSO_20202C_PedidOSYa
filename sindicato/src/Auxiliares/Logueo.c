/*
 * Logueo.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_LOGUEO_C_
#define AUXILIARES_LOGUEO_C_

#include "Logueo.h"

void loguear_error_carga_propiedad(char* propiedad) {
	char* str = string_new();
	char* descripcion = string_new();
	string_append(&descripcion,"Error carga propiedad: %s");
	string_append_with_format(&str, descripcion, propiedad);
	log_error(logger, str);
}

#endif /* AUXILIARES_LOGUEO_C_ */
