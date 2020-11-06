#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#define IP_SINDICATO "127.0.0.1"

#include <protocolo.h>
#include "Configuracion.h"
#include "Mensajes.h"

int socket_servidor;

void escuchar_conexiones_sindicato();


#endif /* AUXILIARES_CONEXION_H_ */
