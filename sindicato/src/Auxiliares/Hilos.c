#include "Hilos.h"

void CrearHiloConsola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola");
	}

	pthread_detach(thread_consola);

	log_info(logger, "Se generó el hilo para la consola");
}

