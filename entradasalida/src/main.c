#include "main.h" 

/*
//Creo que puede ir para la interfaz generica
// Funciones de envío y recepción de mensajes
void enviar_mensaje(int socket, char* mensaje) {
    send(socket, mensaje, strlen(mensaje), 0);
}

char* recibir_mensaje(int socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytes_recibidos = recv(socket, buffer, MAX_BUFFER_SIZE, 0);
    if (bytes_recibidos <= 0) {
        return NULL;
    }
    buffer[bytes_recibidos] = '\0';
    return strdup(buffer);
}

// Funciones de manejo de operaciones específicas
void manejar_operacion_memoria(int socket) {
    // Recibir mensaje de memoria
    char* mensaje = recibir_mensaje(socket);
    if (mensaje != NULL) {
        log_info(loggerES, "Operación de memoria recibida: %s", mensaje);
        free(mensaje);
    }
}

void manejar_operacion_kernel(int socket) {
    // Recibir mensaje de kernel
    char* mensaje = recibir_mensaje(socket);
    if (mensaje != NULL) {
        log_info(loggerES, "Operación de kernel recibida: %s", mensaje);
        free(mensaje);
    }
}

// Función para atender a la memoria
void* atender_memoria(void* arg) {
    while (1) {
        manejar_operacion_memoria(fd_es_memoria);
    }
    return NULL;
}

// Función para atender al kernel
void* atender_kernel(void* arg) {
    while (1) {
        manejar_operacion_kernel(fd_es_kernel);
    }
    return NULL;
}
*/

int main(){

    // Logger
    loggerES = iniciar_logger("entradasalida.log", "Logger_ES", 1, LOG_LEVEL_INFO);


    // Configuración
    iniciar_config();
    log_info(loggerES, "TIPO_INTERFAZ: %s", valores_config.TIPO_INTERFAZ); //es para probar el config
    
    
    //Transformamos los int en string 
	int a = valores_config.PUERTO_KERNEL;
    char puerto_kernel[10];
    sprintf(puerto_kernel, "%d", a);

    int b = valores_config.PUERTO_MEMORIA;
    char puerto_memoria[10];
    sprintf(puerto_memoria, "%d", b);



    // Sockets para conectar con modulos: Kernel y memoria
    // SE CONECTA A KERNEL
    fd_es_kernel = iniciar_socket_cliente(loggerES, valores_config.IP_KERNEL ,puerto_kernel, "Kernel");
    // SE CONECTA A MEMORIA
    fd_es_memoria = iniciar_socket_cliente(loggerES, valores_config.IP_MEMORIA ,puerto_memoria, "Memoria");

// Ateneder a memoria =============================================================================================================================
    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria,NULL,(void*)atender_io_memoria,NULL); 
    pthread_detach(hilo_memoria);
// Ateneder a kernel =============================================================================================================================
    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel,NULL,(void*)atender_io_kernel,NULL); 
    pthread_join(hilo_kernel, NULL);  
    
    // Liberamos recursos
    terminar_programa(new_config, loggerES, fd_es_kernel); //faltan mas argumentos aca, o capaz es mejor personalizar esta funcion para cada modulo

    return 0;
}

/*
-------------------------------------------------------------------------------------
----------------------- DECLARACION DE FUNCIONES ------------------------------------
-------------------------------------------------------------------------------------
*/



// Función para iniciar configuración
void iniciar_config(void) {
    new_config = config_create("entradasalida.config");
    if (new_config == NULL) {
        perror("Error al intentar cargar el config");
        exit(EXIT_FAILURE);
    }

    valores_config.TIPO_INTERFAZ = config_get_string_value(new_config, "TIPO_INTERFAZ");
    valores_config.TIEMPO_UNIDAD_TRABAJO = config_get_int_value(new_config, "TIEMPO_UNIDAD_TRABAJO");
    valores_config.IP_KERNEL = config_get_string_value(new_config, "IP_KERNEL");
    valores_config.PUERTO_KERNEL = config_get_int_value(new_config, "PUERTO_KERNEL");
    valores_config.IP_MEMORIA = config_get_string_value(new_config, "IP_MEMORIA");
    valores_config.PUERTO_MEMORIA = config_get_int_value(new_config, "PUERTO_MEMORIA");
    valores_config.PATH_BASE_DIALFS = config_get_string_value(new_config, "PATH_BASE_DIALFS");
    valores_config.BLOCK_SIZE = config_get_int_value(new_config, "BLOCK_SIZE");
    valores_config.BLOCK_COUNT = config_get_int_value(new_config, "BLOCK_COUNT");
    valores_config.RETRASO_COMPACTACION = config_get_int_value(new_config, "RETRASO_COMPACTACION");
}

//FUNCIONES PARA ATENDER OTROS MODULOS
void atender_io_memoria(void){
        bool control = 1;
        log_info(loggerES, "Esperando instrucciones de memoria");
        while (control) {
		int cod_op = recibir_operacion(fd_es_memoria);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerES, "el server memoria se desconecto. Terminando.");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerES,"Operacion desconocida de memoria.");
			break;
		}
	}
}
void atender_io_kernel(void){
        bool control = 1;
        log_info(loggerES, "Esperando instrucciones de Kernel");
        while (control) {
		int cod_op = recibir_operacion(fd_es_kernel);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerES, "el server Kernel se desconecto. Terminando.");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerES,"Operacion desconocida de memoria.");
			break;
		}
	}
}
