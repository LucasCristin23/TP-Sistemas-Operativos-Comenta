#ifndef MAINKERNEL_H_
#define MAINKERNEL_H_


#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/logger.h>
#include <utils/global.h>
#include <utils/t_datos.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <commons/collections/list.h>
#include <utils/global.h>
#include <commons/collections/queue.h>
#include <readline/history.h>
#include "dispatch.h"

//Declaro struct que contiene los valores del config
typedef struct {
    int PUERTO_ESCUCHA;
    char* IP_MEMORIA;
    int PUERTO_MEMORIA;
    char* IP_CPU;
    int PUERTO_CPU_DISPATCH;
    int PUERTO_CPU_INTERRUPT;
    char* ALGORITMO_PLANIFICACIÓN;
    int QUANTUM;
    t_list* RECURSOS;
    t_list* INSTANCIAS_RECURSOS;
    int GRADO_MULTIPROGRAMACION;
} kernel_config;

kernel_config valores_config;

int fd_socket_kernel_cpu_interrupt;
int fd_socket_kernel_cpu_dispatch;
int fd_socket_kernel_memoria; 
int fd_kernel_servidor;
int fd_bidireccional_kernel;


//Variables y Funciones
t_log* loggerKernel; 
t_config* config_kernel;
void init_config_kernel(void);
// int planificacion_fifo(t_list*, int, int);
void atender_kernel_cpu_dispatch(void);
void atender_kernel_cpu_interrupt(void);
void atender_kernel_memoria(void);
void atender_kernel_IO(void);



// Función de planificación FIFO
int planificacion_fifo(t_list*, int, t_pcb*);

// Función para simular el procesamiento de un proceso por la CPU
void processCPU(t_pcb* process);

// Planificador FIFO
void scheduleFIFO(t_list* cola);

// Planificador Round Robin (RR)
void scheduleRR(t_list* cola, int quantum);



// -------------------------------------- CONSOLA INTERACTIVA --------------------------------------
t_instruccion* armar_comando(char* cadena);
void destruirComando(t_instruccion*);
void tamanio_parametros(t_instruccion*);
void multiprogramacion(t_instruccion*);
void inicializarConsola();
void iniciar_proceso(t_instruccion*, int);

// -------------------------------------- PLANIFICADOR LARGO PLAZO --------------------------------------
t_queue* colaNew;
t_queue* colaReady;
t_queue* colaExit;

// Este lo puse yo amigo (desestimar)
t_dictionary* recurso_bloqueado;


//---------------------------------------- KERNEL GENERAL ------------------------------------
t_dictionary *matriz_recursos_asignados;
t_dictionary *matriz_recursos_pendientes;
t_list *recursos_totales;
char** recursos;
int cant_recursos;



void iniciarColas();
void agregar_a_colaNew();
void planificador_largo_plazo_procesos_nuevos();
void calcular_procesos_en_block(char*, void*);
void actualizar_estado_pcb(t_pcb*, char*);
void agregar_proceso_a_colaReady(int, char*);
void pasar_a_colaReady(t_pcb*);


#endif