#include "main.h"



int main(){

    //Logger

    loggerKernel = iniciar_logger("kernel.log", "Logger_kernel", 1, LOG_LEVEL_INFO);

    //Config    
    init_config_kernel();

    int a = valores_config.PUERTO_ESCUCHA;
    char puerto_escucha[10];
    sprintf(puerto_escucha, "%d", a);

    int b = valores_config.PUERTO_MEMORIA;
    char puerto_memoria[10];
    sprintf(puerto_memoria, "%d", b);

    int c = valores_config.PUERTO_CPU_DISPATCH;
    char puerto_cpu_dispatch[10];
    sprintf(puerto_cpu_dispatch, "%d", c);

    int d = valores_config.PUERTO_CPU_INTERRUPT;
    char puerto_cpu_interrupt[10];
    sprintf(puerto_cpu_interrupt, "%d", d);



    /*
    *
    *   CREAMOS SOCKET PARA Q KERNEL CONECTE CPU
    *
    */

    // este conecta a cpu
    fd_socket_kernel_cpu_interrupt = iniciar_socket_cliente(loggerKernel, valores_config.IP_CPU, puerto_cpu_interrupt, "CPU interrupt");
    fd_socket_kernel_cpu_dispatch = iniciar_socket_cliente(loggerKernel, valores_config.IP_CPU, puerto_cpu_dispatch, "CPU dispatch");
    // Este conecta a memoria
    fd_socket_kernel_memoria = iniciar_socket_cliente(loggerKernel, valores_config.IP_MEMORIA, puerto_memoria, "Memoria");
    
    // creamos servidor de kernel
    fd_kernel_servidor = iniciar_servidor(puerto_escucha, loggerKernel , "Kernel");
    fd_bidireccional_kernel = esperar_cliente(fd_kernel_servidor, loggerKernel, "Entrada/Salida");

// Iniciamos colas (New, Ready, Blocked, Exit)
    iniciarColas();


// Ateneder a cpu - interrupt =============================================================================================================================
    pthread_t hilo_cpu_interrupt;
    pthread_create(&hilo_cpu_interrupt,NULL,(void*)atender_kernel_cpu_interrupt,NULL); 
    pthread_detach(hilo_cpu_interrupt); 

// Ateneder a cpu - dispatch ==============================================================================================================================
    pthread_t hilo_cpu_dispatch;
    pthread_create(&hilo_cpu_dispatch,NULL,(void*)atender_kernel_cpu_dispatch,NULL); 
    pthread_detach(hilo_cpu_dispatch); 
// Ateneder a memoria =====================================================================================================================================
    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria,NULL,(void*)atender_kernel_memoria,NULL); 
    pthread_detach(hilo_memoria);
// Ateneder a I/O =========================================================================================================================================
    pthread_t hilo_io;
    pthread_create(&hilo_io,NULL,(void*)atender_kernel_IO,NULL); 
    pthread_detach(hilo_io); 

// Iniciamos consola   
    inicializarConsola();



    //Liberamos espacio
    terminar_programa(config_kernel, loggerKernel,fd_socket_kernel_cpu_interrupt);

    return 0;
}

/*
-------------------------------------------------------------------------------------
----------------------- DECLARACION DE FUNCIONES ------------------------------------
-------------------------------------------------------------------------------------
*/


//Funcion "iniciar config"
void init_config_kernel(void)
{
    config_kernel = config_create("./kernel.config"); //cambié el path, ahora no hay problemas para referenciarlo
    if (config_kernel == NULL){
        perror("ERROR AL CARGAR EL CONFIG!");
        exit(EXIT_FAILURE);
    }
    valores_config.PUERTO_ESCUCHA = config_get_int_value(config_kernel, "PUERTO_ESCUCHA");
    valores_config.IP_MEMORIA = config_get_string_value(config_kernel, "IP_MEMORIA");
    valores_config.PUERTO_MEMORIA = config_get_int_value(config_kernel, "PUERTO_MEMORIA");
    valores_config.IP_CPU = config_get_string_value(config_kernel, "IP_CPU");
    valores_config.PUERTO_CPU_DISPATCH = config_get_int_value(config_kernel, "PUERTO_CPU_DISPATCH");
    valores_config.PUERTO_CPU_INTERRUPT = config_get_int_value(config_kernel, "PUERTO_CPU_INTERRUPT");
    valores_config.ALGORITMO_PLANIFICACIÓN = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
    valores_config.QUANTUM = config_get_int_value(config_kernel, "QUANTUM");
    valores_config.RECURSOS = config_get_array_value(config_kernel, "RECURSOS"); //Hay que ver hacemos con esto
    valores_config.INSTANCIAS_RECURSOS = config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS"); //Hay que ver hacemos con esto
    valores_config.GRADO_MULTIPROGRAMACION = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");
}



// int main() {
//     // Inicializar la cola de procesos
//     t_list* cola = list_create();

//     // Crear algunos PCBs de ejemplo
//     PCB* process1 = crearPCB(1, 2);
//     PCB* process2 = crearPCB(2, 3);
//     PCB* process3 = crearPCB(3, 4);

//     // Agregar los procesos a la cola usando planificacion_fifo
//     planificacion_fifo(cola, 1, process1);
//     planificacion_fifo(cola, 1, process2);
//     planificacion_fifo(cola, 1, process3);

//     // Planificar y procesar los procesos utilizando FIFO
//     printf("Planificando procesos utilizando FIFO:\n");
//     scheduleFIFO(cola);

//     // Reinicializar la cola de procesos y agregar los procesos nuevamente
//     cola = list_create();
//     process1 = crearPCB(1, 2);
//     process2 = crearPCB(2, 3);
//     process3 = crearPCB(3, 4);
//     planificacion_fifo(cola, 1, process1);
//     planificacion_fifo(cola, 1, process2);
//     planificacion_fifo(cola, 1, process3);

//     // Planificar y procesar los procesos utilizando Round Robin (RR) con un quantum de 2 unidades de tiempo
//     printf("\nPlanificando procesos utilizando Round Robin (RR) con un quantum de 2 unidades de tiempo:\n");
//     scheduleRR(cola, 2);

//     // Liberar la memoria de la cola
//     list_destroy_and_destroy_elements(cola, free);

//     return 0;
// }


// FUNCIONES PARA ATENDER A LOS DEMAS MODULOS =============================================================================================
void atender_kernel_cpu_dispatch(void)
{
        bool control = 1;
        log_info(loggerKernel, "Esperando instrucciones de cpu-dispatch");
        while (control) {
		int cod_op = recibir_operacion(fd_socket_kernel_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerKernel, "el server CPU - Dispatch se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerKernel,"Operacion desconocida de CPU.");
			break;
		}
	}
}

void atender_kernel_cpu_interrupt(void)
{
        bool control = 1;
        log_info(loggerKernel, "Esperando instrucciones de cpu-interrupt");
        while (control) {
		int cod_op = recibir_operacion(fd_socket_kernel_cpu_interrupt);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerKernel, "el CPU - Interrupt se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerKernel,"Operacion desconocida de CPU.");
			break;
		}
	}
}

void atender_kernel_memoria(void)
{
        bool control = 1;
        log_info(loggerKernel, "Esperando instrucciones de memoria");
        while (control) {
		int cod_op = recibir_operacion(fd_socket_kernel_memoria);
		switch (cod_op) {
		case INICIAR_PROCESO:
			char* msg = recibir_mensaje(fd_socket_kernel_memoria);
            log_info(loggerKernel, "Se recibió el %s de memoria, procede el planificador de largo plazo", msg);
            planificador_largo_plazo_procesos_nuevos();
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerKernel, "El server memoria se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerKernel,"Operacion desconocida de memoria.");
			break;
		}
	}
}

void atender_kernel_IO(void)
{
        bool control = 1;
        log_info(loggerKernel, "Esperando instrucciones de I/O");
        while (control) {
		int cod_op = recibir_operacion(fd_bidireccional_kernel);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(loggerKernel, "El cliente I/O se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(loggerKernel,"Operacion desconocida de I/O.");
			break;
		}
	}
}


//  ===================================================================================================================================================


// Función de planificación FIFO
int planificacion_fifo(t_list* cola, int n, t_pcb* proceso)
{
    switch (n) {
        case 1: // agregar a la cola
            list_add(cola, proceso);
            return 0;
        case 2: // sacar de la cola
            if (list_size(cola) > 0) {
                t_pcb* removed_process = list_remove(cola, 0);
                return removed_process->PID;
            } else {
                return -1; // Indica que la cola está vacía
            }
        default:
            return -1; // Operación no válida
    }
}

// Función para simular el procesamiento de un proceso por la CPU
void processCPU(t_pcb* process)
{
    printf("Proceso con PID %d está siendo procesado por la CPU\n", process->PID);
    sleep(process->tiempo_restante); // Simulando el tiempo de ráfaga del proceso
    printf("Proceso con PID %d ha sido procesado por la CPU\n", process->PID);
    free(process); // Liberar la memoria del proceso después de ser procesado
}

// Planificador FIFO
void scheduleFIFO(t_list* cola)
{
    while (list_size(cola) > 0) {
        t_pcb* process = list_remove(cola, 0);
        processCPU(process);
    }
}

// Planificador Round Robin (RR)
void scheduleRR(t_list* cola, int quantum)
{
    while (list_size(cola) > 0) {
        t_pcb* process = list_remove(cola, 0);
        printf("Proceso con PID %d está siendo procesado por la CPU\n", process->PID);
        if (process->tiempo_restante > quantum) {
            sleep(quantum); // Simulando el tiempo de ráfaga con el quantum
            process->tiempo_restante -= quantum;
            printf("Quantum expirado para proceso con PID %d, tiempo restante: %d\n", process->PID, process->tiempo_restante);
            list_add(cola, process); // Mover el proceso al final de la lista si su ráfaga no ha terminado
        } else {
            sleep(process->tiempo_restante); // Procesar el tiempo restante
            printf("Proceso con PID %d ha sido procesado por completo\n", process->PID);
            free(process); // Liberar la memoria del proceso después de ser procesado completamente
        }
    }
}





//lo puse asi por que faltan cosas

/* // Planificador Virtual Round Robin (VRR)
void scheduleVRR(t_list* cola, int quantum) {
    while (list_size(cola) > 0) {
        PCB* process = list_remove(cola, 0);
        printf("Proceso con PID %d está siendo procesado por la CPU\n", process->PID);
        if (process->remaining_time > quantum) {
            sleep(quantum);
            process->remaining_time -= quantum;
            printf("Quantum expirado para proceso con PID %d, tiempo restante: %d\n", process->PID, process->remaining_time);
            list_add(cola, process); // Vuelve a encolar el proceso al final de la lista
        } else {
            sleep(process->remaining_time);
            printf("Proceso con PID %d ha sido procesado por completo\n", process->PID);
            free(process);
        }
    }
}
*/
/*
void planificador_largo_plazo(t_list* colaNew, t_list* colaReady, int grado_multiprogramacion) {
    while (list_size(colaReady) < grado_multiprogramacion && list_size(colaNew) > 0) {
        PCB* proceso = list_remove(colaNew, 0);
        list_add(colaReady, proceso);
        printf("Proceso con PID %d movido a Ready\n", proceso->PID);
    }
}
*/


//------------------------------ CONSOLA ------------------------------
t_instruccion* armar_comando(char *cadena) 
{

	t_instruccion* comando = malloc(sizeof(t_instruccion));


	char* token = strtok(strdup(cadena), " "); //obtiene el primer elemento en token
	string_to_upper(token); //paso el char a mayusculas
	comando->opcode = token;


	token = strtok(NULL, " "); // avanza al segundo elemento
	int i = 0; //variable local utilizada para cargar el array de parametros

	comando->parametros[0] = NULL;
	comando->parametros[1] = NULL;
	comando->parametros[2] = NULL;

	while (token != NULL) { //ingresa si el parametro no es NULL

		comando->parametros[i] = strdup(token); //carga el parametro en el array de la struct
		token = strtok(NULL, " ");
		i++;
	}
	return comando;
}


void tamanio_parametros(t_instruccion* ptrInstrucion) {

	ptrInstrucion->tamanio_opcode = strlen(ptrInstrucion->opcode) + 1;

	if (ptrInstrucion->parametros[0] != NULL) {
		ptrInstrucion->tamanio_parametro1 = strlen(ptrInstrucion->parametros[0]) + 1;
	} else {
		ptrInstrucion->tamanio_parametro1 = 0;
	}
	if (ptrInstrucion->parametros[1] != NULL) {
		ptrInstrucion->tamanio_parametro2 = strlen(ptrInstrucion->parametros[1]) + 1;
	} else {
		ptrInstrucion->tamanio_parametro2 = 0;
	}
	if (ptrInstrucion->parametros[2] != NULL) {
		ptrInstrucion->tamanio_parametro3 = strlen(ptrInstrucion->parametros[2]) + 1;
	} else {
		ptrInstrucion->tamanio_parametro3 = 0;
	}
}


void multiprogramacion(t_instruccion* comando) 
{
	//Modificar el grado de multiprogramacion (no desalojar procesos)
	int nuevo_grado_multiprogramacion = atoi(comando->parametros[0]);
																										// VER SI ANDA DECLARADO ASI		
	log_info(loggerKernel, "Cambio de Grado de Multiprogramación: Grado Anterior: %d - Grado Actual: %d", valores_config.GRADO_MULTIPROGRAMACION , nuevo_grado_multiprogramacion);
	valores_config.GRADO_MULTIPROGRAMACION = nuevo_grado_multiprogramacion;

	destruirComando(comando);
}



void inicializarConsola() 
{
    log_info(loggerKernel, "INICIANDO CONSOLA INTERACTIVA");

	while (1) {
        printf("Ingrese un comando (o help para ver el listado): \n");
		char* leido = readline(">");

		//guarda historial de comandos
		if(leido){
			add_history(leido);
		}
		t_instruccion* comando = armar_comando(leido);
		tamanio_parametros(comando);

        if (strcmp(comando->opcode, "HELP") == 0){
            printf("COMANDOS: \n"
                " EJECUTAR_SCRIPT \n"
                " INICIAR_PROCESO \n"
                " FINALIZAR_PROCESO \n"
                " INICIAR_PLANIFICACION \n"
                " DETENER_PLANIFICACION \n"
                " MULTIPROGRAMACION \n"
                " PROCESO_ESTADO \n");
            destruirComando(comando);

        }else if (strcmp(comando->opcode, "EJECUTAR_SCRIPT") == 0){
			// ACA VA LA FUNCION DE EJECUTAR SCRIPT

		}else if (strcmp(comando->opcode, "INICIAR_PROCESO") == 0) { 				
			iniciar_proceso(comando, valores_config.QUANTUM);

		} else if (strcmp(comando->opcode, "FINALIZAR_PROCESO") == 0) {
			//finalizar_proceso(comando);

        } else if (strcmp(comando->opcode, "INICIAR_PLANIFICACION") == 0) {
			destruirComando(comando);
			//iniciar_planificacion();

		} else if (strcmp(comando->opcode, "DETENER_PLANIFICACION") == 0) {
			destruirComando(comando);
			//detener_planificacion();

		} else if (strcmp(comando->opcode, "MULTIPROGRAMACION") == 0) {
			multiprogramacion(comando);

		} else if (strcmp(comando->opcode, "PROCESO_ESTADO") == 0) {
			destruirComando(comando);
			//proceso_estado();

		}else if(strcmp(comando->opcode, "EXIT") == 0){					// NO ESTOY SEGURO DE QUE VAYA EL EXIT
			destruirComando(comando);
			break;
		} else {
			log_error(loggerKernel,"Comando desconocido! Revisa la documentación por favor.");
			destruirComando(comando);
		}

		free(leido);
	}
}




void destruirComando(t_instruccion* comando){

	free(comando->opcode);
	free(comando->parametros[0]);
	free(comando->parametros[1]);
	free(comando->parametros[2]);
	free(comando);
}


void iniciar_proceso(t_instruccion* comando, int quantum)
{
    t_pcb* pcb = crearPCB(); 

     if (pcb->comando == NULL) {
        pcb->comando = malloc(sizeof(t_instruccion));
        if (pcb->comando == NULL) {
            // Manejar error de asignación de memoria
            return;
        }
    }

    pcb->comando->parametros[0] = strdup(comando->parametros[0]);
	//pcb->comando->parametros[1] = strdup(comando->parametros[1]);
	pcb->comando->tamanio_parametro1 = comando->tamanio_parametro1;
	pcb->comando->tamanio_parametro2 = 0; //comando->tamanio_parametro2;
	pcb->comando->tamanio_parametro3 = 0;
    
    agregar_a_colaNew(&pcb);

    //Le mandamos el comando a memoria para que cree el proceso
	t_paquete* instrucciones = crear_paquete(CREAR_PROCESO);
	agregar_a_paquete(instrucciones, pcb->comando->parametros[0], pcb->comando->tamanio_parametro1);
	agregar_a_paquete_sin_agregar_tamanio(instrucciones, &(pcb->PID), sizeof(int));
	enviar_paquete(instrucciones, fd_socket_kernel_memoria);
    eliminar_paquete(instrucciones);
}


// -------------------------------------- PLANEADOR DE LARGO PLAZO  --------------------------------------

void iniciarColas(){
    colaNew = queue_create();
	colaReady = queue_create();
	colaExit = queue_create();
    log_info(loggerKernel, "Se crearon las colas de procesos con éxito");
}

void agregar_a_colaNew(t_pcb** pcb_proceso){
	queue_push(colaNew, *pcb_proceso);
	log_info(loggerKernel, "Se creó el proceso: %d en la cola de NEW", (*pcb_proceso)->PID);
}

void planificador_largo_plazo_procesos_nuevos(){
    int tamanio_colaReady = queue_size(colaReady);
    int tamanio_colaNew = queue_size(colaNew);

    int procesos_en_memoria = tamanio_colaReady;

    //simulamos agregar el proceso a ready y ver si el grado de multiprogramacion lo permite
    procesos_en_memoria++;
    if(tamanio_colaNew != 0 && procesos_en_memoria <= valores_config.GRADO_MULTIPROGRAMACION){
			agregar_proceso_a_colaReady(fd_socket_kernel_memoria, valores_config.ALGORITMO_PLANIFICACIÓN);
		}

}





// int calcular_procesos_en_memoria(int procesos_en_ready){

// 	dictionary_iterator(recurso_bloqueado, _calcular_procesos_bloqueados);

// 	calcular_procesos_en_block(char* key, void* value){
// 		t_queue* cola_bloqueados_archivo_n = (t_queue*) value;

// 		if(queue_size(cola_bloqueados_archivo_n) != 0){
// 			procesos_bloqueados += queue_size(cola_bloqueados_archivo_n);
// 		}
// 	}

// 	dictionary_iterator(colas_de_procesos_bloqueados_para_cada_archivo, _calcular_procesos_bloqueados_por_archivo);
// 	sem_post(&m_cola_de_procesos_bloqueados_para_cada_archivo);

// 	sem_wait(&m_colas_de_procesos_bloqueados_por_pf);
// 	procesos_bloqueados += dictionary_size(colas_de_procesos_bloqueados_por_pf);
// 	sem_post(&m_colas_de_procesos_bloqueados_por_pf);

// 	sem_wait(&m_proceso_ejecutando);
// 	if(proceso_ejecutando != NULL){
// 		sem_post(&m_proceso_ejecutando);
// 		procesos_bloqueados ++;
// 	} else {
// 		sem_post(&m_proceso_ejecutando);
// 	}


// 	return procesos_bloqueados + procesos_en_ready;
// }


// void calcular_procesos_en_block(char* key, void* value){
//         int procesos_bloqueados = 0;
// 		t_queue* cola_bloqueados_recurso_n = (t_queue*) value;

// 		if(queue_size(cola_bloqueados_recurso_n) != 0){
// 			procesos_bloqueados += queue_size(cola_bloqueados_recurso_n);
// 		}
// 	}


void actualizar_estado_pcb(t_pcb* a_actualizar, char* estado){

	free(a_actualizar->estado_proceso);
	a_actualizar->estado_proceso = string_new();
	string_append(&(a_actualizar->estado_proceso), estado);
} 


// si el proceso no es NEW, no hace falta el socket_memoria
void agregar_proceso_a_colaReady(int socket_memoria, char* algoritmo_planificacion){
	//sem_wait(&m_cola_new);
	t_pcb* proceso_new_a_ready = (t_pcb *) queue_pop(colaNew);
	//sem_post(&m_cola_new);

	log_info(loggerKernel, "Cambio de Estado: < PID: %d - Estado Anterior: %s - Estado Actual: %s >", proceso_new_a_ready->PID, "NEW", "READY");

	actualizar_estado_pcb(proceso_new_a_ready, "READY");

	//envio a memoria de instrucciones
	t_paquete* paquete_instrucciones = crear_paquete(INICIAR_PROCESO);

	agregar_a_paquete(paquete_instrucciones, proceso_new_a_ready->comando->parametros[0],proceso_new_a_ready->comando->tamanio_parametro1);
	agregar_a_paquete_sin_agregar_tamanio(paquete_instrucciones,&(proceso_new_a_ready->PID),sizeof(int));
	enviar_paquete(paquete_instrucciones,socket_memoria);

	eliminar_paquete(paquete_instrucciones);

    pasar_a_colaReady(proceso_new_a_ready);
}


void pasar_a_colaReady(t_pcb* proceso_bloqueado){

	//sem_wait(&m_cola_ready);

	queue_push(colaReady, proceso_bloqueado);

	char *pids = listar_pids_cola(colaReady);
	//sem_post(&m_cola_ready);


	log_info(logger, "Ingreso a Ready: <Cola Ready %s: [%s]>",valores_config.ALGORITMO_PLANIFICACIÓN, pids);

	free(pids);

	//aviso_planificador_corto_plazo_proceso_en_ready(proceso_bloqueado);

}


// void aviso_planificador_corto_plazo_proceso_en_ready(t_pcb* proceso_en_ready){
// 	if(string_equals_ignore_case(algoritmo_planificacion, "PRIORIDADES")){
// 		//asumo que ya esta el proceso en la cola de ready
// 		reordenar_cola_ready_prioridades();

// 		sem_wait(&m_cola_ready);
// 		t_pcb* proceso_con_mayor_prioridad = queue_peek(cola_ready);//obtengo el proceso sin sacarlo de la cola
// 		sem_post(&m_cola_ready);

// 		log_info(logger, "Manejo ingreso a ready");
// 		sem_wait(&m_proceso_ejecutando);
// 		if(proceso_ejecutando == NULL){//si no hay nadie ejecutando
// 			sem_post(&m_proceso_ejecutando);
// 			log_info(logger, "No hay nadie ejecutando, pongo a ejecutar a otro proceso");

// 			poner_a_ejecutar_otro_proceso();

// 		}else if(proceso_con_mayor_prioridad != NULL && proceso_con_mayor_prioridad->prioridad < proceso_ejecutando->prioridad){//si hay alguien ejecutando pero es de menor prioridad
// 			sem_post(&m_proceso_ejecutando);
// 			log_info(logger, "Hay un proceso ejecutando, pero el que esta en ready es de mayor prioridad, lo desalojo");
// 			char* mensaje = malloc(300);
// 			sem_wait(&m_proceso_ejecutando);
// 			sprintf(mensaje, "Desalojo por proceso de mayor prioridad a %d", proceso_ejecutando->PID);
// 			sem_post(&m_proceso_ejecutando);

// 			notificar_desalojo_cpu_interrupt(mensaje);
// 			free(mensaje);

// 			sem_wait(&m_proceso_ejecutando);
// 			t_pcb* proceso_a_desalojar = proceso_ejecutando;
// 			proceso_ejecutando = proceso_con_mayor_prioridad;
// 			sem_post(&m_proceso_ejecutando);

// 			if(strcmp(proceso_a_desalojar->proceso_estado, "EXEC") == 0){
// 				log_info(logger, "Cambio de Estado: “PID: %d - Estado Anterior: %s - Estado Actual: %s“", proceso_a_desalojar->PID, proceso_a_desalojar->proceso_estado,"READY");
// 				actualizar_estado_a_pcb(proceso_a_desalojar, "READY");

// 				sem_wait(&m_cola_ready);
// 				queue_push(cola_ready, proceso_a_desalojar);
// 				sem_post(&m_cola_ready);

// 				aviso_planificador_corto_plazo_proceso_en_ready(proceso_a_desalojar);

// 				char *pids = listar_pids_cola(cola_ready);
// 				log_info(logger, "Ingreso a Ready: “Cola Ready %s: [%s]“",algoritmo_planificacion, pids);
// 				free(pids);
// 			}


// 			sem_wait(&m_cola_ready);
// 			t_pcb* proceso_a_ejecutar = queue_pop(cola_ready);//saco el proceso de la cola
// 			sem_post(&m_cola_ready);

// 			sem_wait(&m_proceso_ejecutando);
// 			pthread_mutex_lock(&m_pid_desalojado);
// 			pid_desalojado= proceso_a_desalojar->PID;
// 			pthread_mutex_unlock(&m_pid_desalojado);
// 			sem_post(&m_proceso_ejecutando);

// 			log_info(logger, "Proceso de menor prioridad desalojado, pongo a ejecutar al proceso de mayor prioridad");
// 			poner_a_ejecutar_proceso(proceso_a_ejecutar);

// 		} else {//si hay alguien ejecutando y es el proceso no hago nada
// 			log_info(logger, "Hay alguien ejecutando y es el de mayor prioridad, no hago nada");
// 			sem_post(&m_proceso_ejecutando);
// 		}
// 	}else {// si es FIFO Y RR
// 		log_info(logger, "Manejo ingreso a ready");
// 		sem_wait(&m_proceso_ejecutando);
// 		if(proceso_ejecutando == NULL){//si no hay nadie ejecutando
// 			sem_post(&m_proceso_ejecutando);
// 			log_info(logger, "Manejo ingreso a ready - no hay nadie ejecutando, pongo a ejecutar a otro proceso");
// 			poner_a_ejecutar_otro_proceso();
// 		} else {
// 			sem_post(&m_proceso_ejecutando);
// 			log_info(logger, "Manejo ingreso a ready - hay alguien ejecutando, no hago nada");
// 		}
// 	}
// }