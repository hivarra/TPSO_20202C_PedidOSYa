/*
 * shared.c
 *
 *  Created on: 31 ago. 2020
 *      Author: utnso
 */

#include "shared.h"
#define CONEXIONES_MAXIMAS 10

pthread_mutex_t lock_logger;

char* procesos_str[] = {
		"APP",
		"CLIENTE",
		"COMANDA",
		"RESTAURANTE",
		"SINDICATO"};

char* mensajes_str[] = {
		"HANDSHAKE",
		"CONSULTAR_RESTAURANTES",
		"SELECCIONAR_RESTAURANTE",
		"OBTENER_RESTAURANTE",
		"CONSULTAR_PLATOS",
		"CREAR_PEDIDO",
		"GUARDAR_PEDIDO",
		"ANADIR_PLATO",
		"GUARDAR_PLATO",
		"CONFIRMAR_PEDIDO",
		"PLATO_LISTO",
		"CONSULTAR_PEDIDO",
		"OBTENER_PEDIDO",
		"FINALIZAR_PEDIDO",
		"TERMINAR_PEDIDO",
		"OBTENER_RECETA",
		"SOCKET_ESCUCHA",
		"SOCKET_ENVIO"};

char* respuestas_str[] = {
		"RTA_CONSULTAR_RESTAURANTES",
		"RTA_SELECCIONAR_RESTAURANTE",
		"RTA_OBTENER_RESTAURANTE",
		"RTA_OBTENER_RESTAURANTE",
		"RTA_CONSULTAR_PLATOS",
		"RTA_CREAR_PEDIDO",
		"RTA_GUARDAR_PEDIDO",
		"RTA_ANADIR_PLATO",
		"RTA_GUARDAR_PLATO",
		"RTA_CONFIRMAR_PEDIDO",
		"RTA_PLATO_LISTO",
		"RTA_CONSULTAR_PEDIDO",
		"RTA_OBTENER_PEDIDO",
		"RTA_FINALIZAR_PEDIDO",
		"RTA_TERMINAR_PEDIDO",
		"RTA_OBTENER_RECETA",
		"SOCKET_ESCUCHA",
		"SOCKET_ENVIO"};


/* ---------- Logger ---------- */
t_log* configurar_logger(char* nombreLog, char* nombreProceso) {
	t_log* logger = log_create(nombreLog, nombreProceso, false, LOG_LEVEL_INFO);
	log_info(logger, "*************** NUEVO LOG ***************");
	return logger;
}

void destruir_logger(t_log* logger) {
	if(logger!=NULL)
		log_destroy(logger);
}

/* ---------- Configuración ---------- */
void destruir_config(t_config* config) {
	if(config!=NULL)
		config_destroy(config);
}

/* ---------- Conexión ---------- */
int crear_conexion(char *ip, char *puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		socket_cliente = -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

int definirSocket(t_log* logger) {

	int nuevoSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (nuevoSocket == -1) {
		log_error(logger, "No se pudo abrir el socket");
		return -1;
	}
//	log_info(logger, "Socket creado correctamente.");
	return nuevoSocket;
}

int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger) {
	struct sockaddr_in direccion;
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = htonl(INADDR_ANY);
	direccion.sin_port = htons(puerto);
	memset(&(direccion.sin_zero), '\0', 8);

	/* Bindeo */
	if (bind(socket, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
		log_error(logger, "No se pudo bindear el socket");
		return -1;
	}
//	log_info(logger, "Socket bindeado correctamente.");

	/* Escucha */
	if (listen(socket, CONEXIONES_MAXIMAS) < 0) {
		log_error(logger, "No se puede escuchar conexiones");
		return 0;
	}
	log_info(logger, "SERVIDOR | Escuchando conexiones");
	return 1;
}

int aceptarConexiones(int socket, t_log* logger) {
	int client_socket;
	struct sockaddr_in server_address;
	unsigned int len = sizeof(struct sockaddr);

	if ((client_socket = accept(socket, (struct sockaddr*) &server_address,
			&len)) == -1) {
		log_error(logger, "No se puede aceptar conexiones");
		return -1;
	}
//	log_info(logger, "Conexion aceptada (sock_id: %d)", client_socket);
	return client_socket;
}

int conectar_a_servidor(char* ip, int puerto, int id_proceso, int tipoProcesoEmisor, int tipoProcesoReceptor, t_log* logger) {

	int socket_cliente;

	if((socket_cliente = definirSocket(logger))<= 0)
	{
		log_error(logger, "CONEXION | No se pudo definir socket.");
		return -1;
	}

	if(conectarseAServidor(socket_cliente, ip, puerto, logger)<=0)
	{
		log_error(logger, "CONEXION | No se pudo conectar a servidor %s", get_nombre_proceso(tipoProcesoReceptor));
		return -1;
	}

//	loggear(logger,LOG_LEVEL_INFO, "INICIO Handshake(%d)...", tipoProcesoReceptor);
//	enviarMensaje(tipoProcesoEmisor, id_proceso, HANDSHAKE, 0, NULL, socket_cliente, tipoProcesoReceptor, logger);
//	t_mensaje* msg = recibirMensaje(socket_cliente, logger);
//	destruirMensaje(msg);
//	loggear(logger,LOG_LEVEL_INFO, "FIN Handshake(%d)", tipoProcesoReceptor);
	return socket_cliente;
}

int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger) {

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_port = htons(puerto);
	direccionServidor.sin_addr.s_addr = inet_addr(ip);

	if (connect(socket, (void *) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
//		log_error(logger, "No se pudo conectar al Servidor.");
		return 0;
	}
//	log_info(logger, "Conectado al Servidor correctamente.");
	return 1;
}
/* ---------- Exit ---------- */
void _exit_with_error(char* error_msg, void * buffer, t_log* logger) {

	if (buffer != NULL)
		free(buffer);
	if (error_msg != NULL)
		loggear(logger, LOG_LEVEL_ERROR, error_msg);

	exit_gracefully(EXIT_FAILURE, logger);
}

void exit_gracefully(int return_nr, t_log *logger) {

  loggear(logger, LOG_LEVEL_INFO, "********* FIN DEL PROCESO *********");
  destruir_logger(logger);

  exit(return_nr);
}

/* ---------- Tipos mensajes y procesos ---------- */
char* get_nombre_proceso(int enum_proceso) {

	return procesos_str[enum_proceso];
}

char* get_nombre_mensaje(int enum_mensaje) {

	return mensajes_str[enum_mensaje];
}

char* get_nombre_respuesta(int enum_respuesta) {

	return respuestas_str[enum_respuesta];
}

t_tipoProceso tipo_proceso_string_to_enum(char *sval) {
	t_tipoProceso result = APP;
	for (int i = 0; procesos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, procesos_str[i]))
			return result;
	return -1;
}

t_tipoMensaje tipo_mensaje_string_to_enum(char *sval) {
	t_tipoMensaje result = HANDSHAKE;
	for (int i = 0; mensajes_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, mensajes_str[i]))
			return result;
	return -1;
}

/* ---------- Paths ---------- */
char* getCurrentPath() {
	long size;
	char *buf;
	char *ptr;

	size = pathconf(".", _PC_PATH_MAX);

	if ((buf = (char *)malloc((size_t)size)) != NULL)
	    ptr = getcwd(buf, (size_t)size);

	return ptr;
}

char* getParentPath() {
	return dirname(getCurrentPath());
}

char* getConfigPath(char* nombre_archivo) {
	char* parentPath = getParentPath();
	char* path = string_from_format("%s/config/%s", parentPath, nombre_archivo);
	free(parentPath);
	return path;
}

/* ---------- Extras ---------- */
uint64_t timestamp() {
	struct timeval valor;
	gettimeofday(&valor, NULL);
	unsigned long long result = (((unsigned long long )valor.tv_sec) * 1000 + ((unsigned long) valor.tv_usec));
	uint64_t tiempo = result;
	return tiempo;
}

void liberar_lista(char** lista){
	int contador = 0;
	while(lista[contador] != NULL){
		free(lista[contador]);
		contador++;
	}
	free(lista);
}


/* ---------- Posiblemente no usados ---------- */
char* getLogPath(char* nombre_archivo) {
	char* path = string_new();
	string_append(&path, "/home");
	string_append(&path, nombre_archivo);
	return path;
}

void loggear(t_log* logger, t_log_level level, const char* message_format, ...) {

	va_list arguments;
	va_start(arguments, message_format);

	char *message;

	message = string_from_vformat(message_format, arguments);

	switch (level) {
	case LOG_LEVEL_TRACE:
		;
		pthread_mutex_lock(&lock_logger);
		log_trace(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_DEBUG:
		;
		pthread_mutex_lock(&lock_logger);
		log_debug(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_INFO:
		;
		pthread_mutex_lock(&lock_logger);
		log_info(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_WARNING:
		;
		pthread_mutex_lock(&lock_logger);
		log_warning(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_ERROR:
		;
		pthread_mutex_lock(&lock_logger);
		log_error(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	default:
		break;
	}

	free(message);
	va_end(arguments);
}

t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger) {
	t_config* config = config_create(nombreArchivo);
	if (!config) {
		log_error(logger, "No se pudo abrir el archivo de configuración.");
		return NULL;
	}
	log_info(logger,"Archivo de configuración OK.");
	return config;
}

void eliminar_blancos(char *str)
{
    // To keep track of non-space character count
    int count = 0;

    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is
                                   // incremented
    str[count] = '\0';
}

void imprimir_lista_strings(t_list* lista, char* nombre_lista) {

	void imprimir_lista(char* str) {
		printf("  %s\n", str);
	}

	if (!list_is_empty(lista)) {
		printf("%s \n", nombre_lista);
		list_iterate(lista, (void*) imprimir_lista);
	} else {
		printf("%s VACIOS \n", nombre_lista);
	}
}
