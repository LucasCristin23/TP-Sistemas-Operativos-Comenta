#ifndef MAINMEMORIA_H_
#define MAINMEMORIA_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h> 
#include <math.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

#include <utils/hello.h>
#include <utils/t_datos.h>
#include <utils/global.h>
#include <utils/logger.h>
#include <utils/t_datos.h>


typedef struct{
    int PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_PAGINA;
    char* PATH_INSTRUCCIONES;
    int RETARDO_RESPUESTA;
} memoria_config;
memoria_config valores_config;


// Crear proceso
typedef struct{
    char* nombre;
    t_pcb* pcb;
    char* path_proceso;
    op_code* cod_op;
} proceso_t;

t_log* loggerMemoria;
t_config* config_memoria;
t_dictionary* instrucciones_por_pid;
int memoria_fd; //escucha a cpu, kernel e IO
int socket_cpu;
int socket_kernel;
int socket_io;

//FIRMAS FUNCIONES
void init_config_memoria(void);
void crear_proceso(int);
void atender_cliente(int);
void manejar_conexiones(int);
void leer_archivo(int);
void enviar_instrucciones_a_cpu(int);


void atender_memoria_cpu(void);
void devolver_marco(int);
void leer_memoria(int);
void escribir_memoria(int);


void atender_memoria_IO(void);



void atender_memoria_kernel(void);



// ---------- PAGINACION ----------
void* espacioUsuario; // Le cambie el nombre a memoriaPrincipal
int tamanioPagina;
int cantidadFrames;

// Firmas funciones
void iniciarPaginacion(void);


#endif
