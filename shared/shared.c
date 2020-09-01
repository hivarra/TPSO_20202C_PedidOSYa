/*
 * shared.c
 *
 *  Created on: 31 ago. 2020
 *      Author: utnso
 */

#include "shared.h"

/* ---------- Logger ---------- */
t_log* configurar_logger(char* nombreLog, char* nombreProceso) {
	t_log* logger = log_create(nombreLog, nombreProceso, false, LOG_LEVEL_INFO);
	log_info(logger, "*************** NUEVO LOG ***************");
	return logger;
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
void destruir_logger(t_log* logger) {

	if(logger!=NULL)
		log_destroy(logger);
}

/* ---------- Configuración ---------- */
t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger) {
	t_config* config = config_create(nombreArchivo);
	if (!config) {
		log_error(logger, "No se pudo abrir el archivo de configuración.");
		return NULL;
	}
	log_info(logger,"Archivo de configuración OK.");
	return config;
}

void destruir_config(t_config* config) {

	if(config!=NULL)
		config_destroy(config);
}

/* ---------- Conexión ---------- */
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
//		log_error(logger, "CONEXION | No se pudo definir socket.");
		return -1;
	}

	if(conectarseAServidor(socket_cliente, ip, puerto, logger)<=0)
	{
//		log_error(logger, "CONEXION | No se pudo conectar a servidor %s", get_nombre_proceso(tipoProcesoReceptor));
		return -1;
	}

//	loggear(logger,LOG_LEVEL_INFO, "INICIO Handshake(%d)...", tipoProcesoReceptor);
	enviarMensaje(tipoProcesoEmisor, id_proceso, HANDSHAKE, 0, NULL, socket_cliente, tipoProcesoReceptor, logger);
	t_mensaje* msg = recibirMensaje(socket_cliente, logger);
	destruirMensaje(msg);
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

int enviarMensaje(int tipoProcesoEmisor, int id_proceso, int tipoMensaje, int len, void* content,
		int socketReceptor, int tipoProcesoReceptor, t_log* logger) {

	void* buffer = serializar(tipoProcesoEmisor, id_proceso, tipoMensaje, len, content);

	if (len > 0 && content != NULL)
		if (buffer == NULL) {
			log_error(logger, "No se pudo serializar mensaje (proceso emisor %s).", get_nombre_proceso(tipoProcesoEmisor));
			return -1;
		}

	if (send(socketReceptor, buffer, sizeof(t_header) + len, 0) <= 0) {
		free(buffer);
		log_info(logger, "No se pudo enviar mensaje (proceso emisor %s - proceso receptor %s).",
				get_nombre_proceso(tipoProcesoEmisor), get_nombre_proceso(tipoProcesoReceptor));
		return 0;
	}
	/* Cambiar esto porque no está bien */
//	log_info(logger, "Se envió mensaje (proceso emisor %s - proceso receptor %s).",
//			get_nombre_proceso(tipoProcesoEmisor), get_nombre_proceso(tipoProcesoReceptor));

	free(buffer); // Sirver para t_mensaje
	return 1;
}

t_mensaje* recibirMensaje(int socketEmisor, t_log* logger) {

	void* buffer = malloc(sizeof(t_header));

	if (recv(socketEmisor, buffer, sizeof(t_header), 0) <= 0) {
		free(buffer);
		log_error(logger, "Se desconecto o ocurrio un error con el socket (sock_id: %d).", socketEmisor);
		return NULL;
	}

	t_mensaje* msg = deserializar(buffer);

	msg->content = NULL;

	if (msg->header.longitud > 0) {
		//buffer = calloc(sizeof(char), msg->header.longitud + 1);
		void* c_buffer = malloc(msg->header.longitud + 1);
		if (recv(socketEmisor, c_buffer, msg->header.longitud, MSG_WAITALL) <= 0) {
			free(c_buffer);
			log_error(logger, "Se desconecto o ocurrio un error con el socket (sock_id: %d).", socketEmisor);
			return NULL;
		}
		msg->content = c_buffer;
	}
	return msg;
}

void* serializar(int tipoProceso, int id_proceso, int tipoMensaje, int len, void* content) {

	t_mensaje mensaje;
	mensaje.header.tipoProceso = tipoProceso;
	mensaje.header.idProceso = id_proceso;
	mensaje.header.tipoMensaje = tipoMensaje;
	mensaje.header.longitud = len;
	mensaje.content = content;

	void* buffer = malloc(sizeof(t_header) + len);
	memcpy(buffer, &(mensaje.header), sizeof(t_header));
	memcpy(buffer + sizeof(t_header), mensaje.content, len);

	return buffer;
}

t_mensaje* deserializar(void* buffer) {
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
	memcpy(&(mensaje->header), buffer, sizeof(t_header));
	free(buffer);
	return mensaje;
}

void destruirMensaje(t_mensaje* msg) {

	if(msg == NULL)
		return;
	if (msg != NULL && msg->header.longitud > 0 && msg->content != NULL)
		free(msg->content);
	if(msg != NULL)
		free(msg);
}

/* ---------- Errores ---------- */
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
t_tipoMensaje tipo_mensaje_string_to_enum(char *sval) {
	t_tipoMensaje result = HANDSHAKE;
	int i = 0;
	for (i = 0; mensajes_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, mensajes_str[i]))
			return result;
	return -1;
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

	char* path = string_from_format("%s", getParentPath());
	string_append(&path, "/config/");
	string_append(&path, nombre_archivo);
	return path;
}

char* getLogPath(char* nombre_archivo) {

	char* path = string_from_format("%s", getParentPath());
	string_append(&path, "/log/");
	string_append(&path, nombre_archivo);
	return path;
}