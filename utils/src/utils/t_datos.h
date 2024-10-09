#ifndef T_DATOS_H_
#define T_DATOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h> 
#include <string.h>

// Reg CPU
typedef struct{
    u_int32_t PC; //Program Counter, indica la próxima instrucción a ejecutar
    u_int8_t AX;
    u_int8_t BX;
    u_int8_t CX;
    u_int8_t DX;
    u_int32_t EAX;
    u_int32_t EBX;
    u_int32_t ECX;
    u_int32_t EDX;
    u_int32_t SI; //Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string
    u_int32_t DI; //Contiene la dirección lógica de memoria de destino a donde se va a copiar un string
} t_reg_cpu;

// Instruccion
typedef struct {
	int tamanio_opcode;
	char* opcode;
	int tamanio_parametro1;
	int tamanio_parametro2;
	int tamanio_parametro3;
	char* parametros[3];

}t_instruccion;

// Contexto de ejecucion
typedef struct
{
	int pid;
	t_instruccion* instruccion;
    // El Program counter lo usamos por los registros de cpu 
	t_reg_cpu* registros_CPU;

} t_contexto_ejec;


// Estructura del PCB
typedef struct {
    int PID;  // Identificador del proceso
    int program_counter; // La dirección de la siguiente instruccion del programa que se ejecutará
    t_reg_cpu* registros_cpu;
    int quantum;  // Tiempo de ráfaga del proceso
    int tiempo_restante;  // Tiempo restante del proceso (para VRR)
    char* estado_proceso; //"NEW", "READY", "EXECUTE", "BLOCKED", "EXIT" 
    t_instruccion* comando;
} t_pcb;


// Recursos
typedef struct  {
	char* nombre_recurso;
	int instancias_en_posesion;
} t_recurso;


// t_datos de memoria
typedef struct{
    char* pid;
    int numeroPagina;
} t_referenciaXpid;

typedef struct{
    int marco;
    bool presencia;
    bool modificado;  // Este no estoy seguro de que lo usemos, lo puse por las dudas 
    u_int32_t posicionSwap;
} t_tabla_de_paginas;

typedef struct{
    int pid;
    int numeroMarco;
    bool libre;
    int posicionInicioMarco;
} t_situacion_marco;






// Firmas funciones

// Función para crear un nuevo PCB
t_pcb* crearPCB();

#endif