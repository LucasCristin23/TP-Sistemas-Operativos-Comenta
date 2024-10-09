#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_
#include<stdio.h>
#include<stdlib.h>

#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>


/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/
void decir_hola(char* quien);


//DECLARACION DE FUNCIONES RELACIONADAS CON SERVER
int iniciar_servidor(char* puerto, t_log* logger, char* msj);
int esperar_cliente(int socket_servidor, t_log* logger, char* msj);
int iniciar_socket_cliente(t_log* logger, char* ip, char* puerto, char* msj);
void terminar_programa(t_config*, t_log*, int);
//








#endif
