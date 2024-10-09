#ifndef MAINCPU_H_
#define MAINCPU_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <math.h>


#include <utils/hello.h>
#include <utils/logger.h>
#include <utils/global.h>
#include <utils/t_datos.h> 

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include <manejoInstrucciones.h>


//Struct del config
typedef struct {
    char* IP_MEMORIA;
    int PUERTO_MEMORIA;
    int PUERTO_ESCUCHA_DISPATCH;
    int PUERTO_ESCUCHA_INTERRUPT;
    int CANTIDAD_ENTRADAS_TLB;
    char* ALGORITMO_TLB;
} cpu_config;

cpu_config valores_config;

int fd_cpu_modo_dispatch;
int fd_cpu_modo_interrupt;
int fd_socket_cpu; //se conecta a memoria
int fd_bidireccional_kernel_dispatch;
int socket_kernel_interrupt; // Le cambie el nombre

int pid_ejecutando;
char* pid_a_desalojar = NULL;
bool interrupcion_pendiente = false;
bool continuar_ciclo_instruccion = true;

// VARIABLES MANEJO DE INSTRUCCIONES
extern int configTamanioPagina;



//DECLARACION FUNCIOIONES
t_config* config;
t_log* loggerCPU;
void init_config_cpu(void);
void atender_cpu_kernel_interrupt(void);
void atender_cpu_kernel_dispatch(void);
void atender_cpu_memoria(void);
void procesar_proceso(t_pcb*);
int recibir_operacion(int);

void recibirInterrupcion(int);
void* manejarInterrupcion(void* args);
void devolverKernel(t_contexto_ejec* , op_code, int);
t_instruccion* recibirInstruccionMemoria(int, int);

// void recibir_mensaje(int socket_cliente);

// FIRMA FUNCIONES MANEJO DE INSTRUCCIONES 
u_int32_t obtenerValorReg(char* regLeer, t_contexto_ejec** contextoActual);
void setearReg(t_contexto_ejec** contexto, char* registro, u_int32_t);
void instruccionSet(t_contexto_ejec** contexto, t_instruccion* instruccion);
void instruccionSum(t_contexto_ejec** contexto, t_instruccion* instruccion);
void instruccionSub(t_contexto_ejec** contexto, t_instruccion* instruccion);
void instruccionJNZ(t_contexto_ejec** contexto, t_instruccion* instruccion);
u_int32_t leerValorMemoria(int,int);
void instruccionMOVIN(t_contexto_ejec** contexto, t_instruccion* instruccion);
// int obtenerMarco(int, int);
int traducir_direccion_logica(int, int, t_contexto_ejec*);
bool decodificar_direccion_logica(t_contexto_ejec**, int);


void asignarValor(int, u_int32_t, int);
void instruccionMOVOUT(t_contexto_ejec**, t_instruccion*);


#endif



