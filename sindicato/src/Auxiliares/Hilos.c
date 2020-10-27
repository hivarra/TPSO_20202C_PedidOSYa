#include "Hilos.h"

void crearHiloConsola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola");
	}

	log_info(logger, "Se generó el hilo para la consola");
}

