/*
 * comanda.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef COMANDA_H_
#define COMANDA_H_

#include <shared.h>
#include <signal.h>
#include "Auxiliares/Logueo.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Utils.h"
#include "Auxiliares/MMU.h"

void signalHandler(int);//Libera recursos al terminar con ctrl+c. Util para probar que recursos no se estan liberando.

#endif /* COMANDA_H_ */
