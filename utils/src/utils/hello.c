#include <utils/hello.h>

void decir_hola(char* quien) {
    printf("Hola desde %s!!\n", quien);
}

//FUNCIONES RELACIONADAS A SERVIDORES

int iniciar_servidor(char* puerto, t_log* logger, char* msj)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,	
							 servinfo->ai_socktype,	
							 servinfo->ai_protocol);
	
	
	// Asociamos el socket a un puerto
	bind(socket_servidor,servinfo->ai_addr,servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor,SOMAXCONN);

	freeaddrinfo(servinfo);

	// log_trace(logger, "Listo para escuchar a mi cliente en %s",msj)
	// esta comentado porque lo vamos a usar mas adelantes 
	log_info(logger, "Listo para escuchar a mi cliente en %s",msj);

	return socket_servidor;
}

// @brief necesita el socket del server y el logger del modulo para registrar lo que pasa 

int esperar_cliente(int socket_servidor, t_log* logger, char* msj)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor,NULL,NULL);			// ACCEPT es bloqueante
	log_info(logger, "Se conecto el cliente: %s", msj);

	return socket_cliente; // Este socket es con el que nos vamos a comunicar 
}



/*
*
*
* Empezamos con los sockets de cliente
* 
*
*/

int iniciar_socket_cliente( t_log* logger, char* ip, char* puerto, char* msj)
{
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, puerto, &hints, &server_info); 

	// Creamos socket cliente
	int fd_socket_cliente = socket(server_info->ai_family,
                         			server_info->ai_socktype,
                         			server_info->ai_protocol);

	if(connect(fd_socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		log_info(logger, "No se pudo establecer la conexion con %s", msj);
		// exit(EXIT_FAILURE);
	}else{
		log_info(logger, "Se pudo establecer la conexion con el modulo %s", msj);
	}

	freeaddrinfo(server_info);

	return fd_socket_cliente;
}

//Funcion para liberar el espacio utilizado
void terminar_programa(t_config* config, t_log* logger, int socket_cliente)
{
    config_destroy(config);
    log_destroy(logger);
	close(socket_cliente);
};








