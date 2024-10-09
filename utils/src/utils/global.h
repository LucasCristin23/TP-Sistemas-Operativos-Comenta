#ifndef GLOBAL_H_
#define GLOBAL_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

#include<commons/collections/queue.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<utils/t_datos.h>


typedef enum
{
    //generales
	MENSAJE,
	PAQUETE,
    HANDSHAKE,
    RPTA_HANDSHAKE,
    //kernel-memoria
    INICIAR_PROCESO,
    CREAR_PROCESO,
	TERMINAR_PROCESO,
    EJECUTAR_SCRIPT,
    //kernel-cpu
    OPERACION,
    APROPIAR_RECURSOS,
    DESALOJAR_RECURSOS,

    //Memoria
    LEER_MEMORIA,
    LEER_MEMORIA_RESPUESTA,
    ESCRIBIR_MEMORIA,
    ESCRIBIR_MEMORIA_RESPUESTA,
    ACCESO_PAGINA,
    PAGE_FAULT,
    INSTRUCCION,
    INTERRUPCION

    //etc
} op_code;





typedef struct{ //============================= BUFFER INTERMEDIO ========================================
    u_int32_t size;
    u_int32_t offset;
    void*  stream;

} t_buffer; 


typedef struct{  //===================== BUFFER ========================================
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete; 

typedef struct{
    op_code codigo_operacion;
} t_paquete_opcode;




/*-------------------------------------------------------------------------------------
-----------------               FIRMAS FUNCIONES                    -------------------
-------------------------------------------------------------------------------------*/



t_paquete* crear_paquete(op_code);
t_paquete_opcode* crear_paquete_operacion(op_code);
void agregar_a_paquete_sin_agregar_tamanio(t_paquete* paquete, void* valor, int tamanio);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void* serializar_paquete_operacion(t_paquete_opcode*, int);
void* serializar_paquete(t_paquete*, int);
void enviar_paquete(t_paquete*, int);
void enviar_paquete_operacion(t_paquete_opcode*, int);
void enviar_operacion(op_code, int);
void enviar_mensaje(char* mensaje, int socket_cliente, op_code codigo);
void crear_buffer(t_paquete*);
void eliminar_paquete(t_paquete* paquete);

void tiempo_de_espera(int);
char* listar_pids_cola(t_queue*);


// ---------------- SERVER ----------------

int recibir_operacion(int);
char* recibir_mensaje(int);
void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_path_pid(int, char**, int*);
void recibir_handshake(int);


t_instruccion* deserializarInstruccion(void *buffer, int* desplazamiento);
t_instruccion* recibirInstruccion(int socketCliente);
void destruir_instruccion(t_instruccion* instruccion);

void recibirProgramCounter(int, int* pid, int* PC);

t_contexto_ejec* deserializar_contexto_de_ejecucion(void *buffer, int, int *desplazamiento);
t_contexto_ejec* recibirContextoEjecucion(int socketCliente);
void destruir_contexto_de_ejecucion(t_contexto_ejec* contexto);


#endif 