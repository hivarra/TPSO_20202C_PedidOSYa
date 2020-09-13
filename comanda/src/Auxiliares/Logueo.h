/*
 * Logueo.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_LOGUEO_H_
#define AUXILIARES_LOGUEO_H_

#include <shared.h>
#include "Configuracion.h"

t_log* logger;

void loguear_error_carga_propiedad(char* propiedad);
void cargar_logger_comanda();


#endif /* AUXILIARES_LOGUEO_H_ */
