/*
 * cliente.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Consola.h"
#include <signal.h>

void configurarProceso();
void signalHandler(int);

#endif /* CLIENTE_H_ */
