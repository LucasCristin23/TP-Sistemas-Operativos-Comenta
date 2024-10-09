#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <commons/log.h>
#include <commons/config.h>

#include <utils/hello.h>
#include <utils/t_datos.h>
#include <utils/global.h>
#include <utils/logger.h>
#include <utils/t_datos.h>


//Declaracion del struct del Config
typedef struct {
   char* TIPO_INTERFAZ;
   int TIEMPO_UNIDAD_TRABAJO;
   char* IP_KERNEL;
   int PUERTO_KERNEL;
   char* IP_MEMORIA;
   int PUERTO_MEMORIA;
   char* PATH_BASE_DIALFS;
   int BLOCK_SIZE;
   int BLOCK_COUNT;
   int RETRASO_COMPACTACION;
} ES_config;

ES_config valores_config;
t_log* loggerES;

int fd_es_kernel;
int fd_es_memoria;
//DECLARACION DE FUNCIONES 
t_config* new_config;
void iniciar_config(void);
void atender_io_kernel(void);
void atender_io_memoria(void);