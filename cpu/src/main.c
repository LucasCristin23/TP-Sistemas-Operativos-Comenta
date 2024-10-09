#include "main.h"


int main() {
    
    //Logger    
    logger = iniciar_logger( "cpu.log", "Logger_CPU", 1 , LOG_LEVEL_INFO);


    //Config
    init_config_cpu();


    //Transformamos el int en string
    int a = valores_config.PUERTO_ESCUCHA_DISPATCH;
    char puerto_dispatch[10];
    sprintf(puerto_dispatch, "%d", a);

    int b = valores_config.PUERTO_MEMORIA;
    char puerto[10];
    sprintf(puerto, "%d", b);

    int c = valores_config.PUERTO_ESCUCHA_INTERRUPT;
    char puerto_interrupt[10];
    sprintf(puerto_interrupt, "%d", c);



    //Iniciamos cpu en modo servidor dispatch e interrupt
    fd_cpu_modo_dispatch = iniciar_servidor(puerto_dispatch, logger, "CPU dispatch");
    fd_cpu_modo_interrupt = iniciar_servidor(puerto_interrupt, logger, "CPU interrupt");

    //Conectar a memoria
    fd_socket_cpu = iniciar_socket_cliente(logger, valores_config.IP_MEMORIA, puerto, "Memoria");
    


/*
*
*   Ahora lo levantamos en modo server
*
*
*/

    //La ponemos a escuchar clientes de kernel dispatch e interrupt
    fd_bidireccional_kernel_dispatch = esperar_cliente(fd_cpu_modo_dispatch, logger, "Kernel dispatch");
    socket_kernel_interrupt = esperar_cliente(fd_cpu_modo_interrupt, logger, "Kernel interrupt");


// Atender a Kernel - Dispatch=====================================================================================================================
    pthread_t hilo_kernel_dispatch; // VER DE LIBERAR LOS HILOS DESPS
    pthread_create(&hilo_kernel_dispatch,NULL,(void*)atender_cpu_kernel_dispatch,(void*)socket_kernel_interrupt); // VER SI ANDA EL PUERTO INTERRUPT
    pthread_detach(hilo_kernel_dispatch); // hace que el hilo ejecute paralelamente y el prgrama siga
    //atender_cpu_kernel_dispatch(); no va porque esta el hilo

//Atender a kernel - Interrupt ========================================================================================
    pthread_t hilo_kernel_interrupt;
    pthread_create(&hilo_kernel_interrupt,NULL, manejarInterrupcion ,NULL); //fijarme si anda, poner socket 
    pthread_detach(hilo_kernel_interrupt); 

//Atender a Memoria ========================================================================================
    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria,NULL,(void*)atender_cpu_memoria,NULL); 
    pthread_join(hilo_memoria, NULL); // el join le dice al hilo principal "que no continue el programa hasta que este hilo termine"
    //pthread_detach(hilo_memoria);     NO SE HACE DETACH PORQUE ES EL ULTIMO

    
    
    
    
    terminar_programa(config, logger, fd_cpu_modo_interrupt); // Falta liberar las conexiones 

    return 0;

}



/*
-------------------------------------------------------------------------------------
----------------------- DECLARACION DE FUNCIONES ------------------------------------
-------------------------------------------------------------------------------------
*/


//Funcion para iniciar el config
void init_config_cpu(void)
{
    config = config_create("./cpu.config"); 
    if (config == NULL){
        perror("ERROR AL CARGAR EL CONFIG!");
        exit(EXIT_FAILURE);
    }
    valores_config.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    valores_config.PUERTO_MEMORIA = config_get_int_value(config, "PUERTO_MEMORIA");
    valores_config.PUERTO_ESCUCHA_DISPATCH = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    valores_config.PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    valores_config.CANTIDAD_ENTRADAS_TLB = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");    
    valores_config.ALGORITMO_TLB = config_get_string_value(config, "ALGORITMO_TLB");
}

// FUNCIONES PARA ATENDER PROCESOS

void atender_cpu_kernel_dispatch(void)
{
        bool control = 1;
        log_info(logger, "Esperando instrucciones de kernel-dispatch");
        while (control) {
		int cod_op = recibir_operacion(fd_bidireccional_kernel_dispatch);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case -1:
			log_error(logger, "el cliente Kernel Dipatch se desconecto. Terminando servidor");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(logger,"Operacion desconocida de kernel.");
			break;
		}
	}
}

// void atender_cpu_kernel_interrupt(void)
// {
//         bool control = 1;
//         log_info(logger, "Esperando instrucciones de kernel-interrupt");
//         while (control) {
// 		int cod_op = recibir_operacion(fd_bidireccional_kernel_interrupt);
// 		switch (cod_op) {
// 		case MENSAJE:
// 			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
// 			break;
// 		case PAQUETE:
//                 //
// 			break;
// 		case -1:
// 			log_error(logger, "el cliente Kernel Interrupt se desconecto. Terminando servidor");
// 			control = 0;
//             //return EXIT_FAILURE;
//             break;
// 		default:
// 			log_warning(logger,"Operacion desconocida de kernel.");
// 			break;
// 		}
// 	}
// }

void atender_cpu_memoria(void)
{
        bool control = 1;
        log_info(logger, "Esperando instrucciones de memoria");
        while (control) {
		int cod_op = recibir_operacion(fd_socket_cpu);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;

        case OPERACION:
            
            break;

		case -1:
			log_error(logger, "el cliente Kernel Dipatch se desconecto. Terminando servidor");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(logger,"Operacion desconocida de kernel.");
			break;
		}
	}
}


void procesar_proceso(t_pcb* proceso) {
    log_info(logger, "CPU está procesando el proceso con PID %d", proceso->PID);
    sleep(proceso->tiempo_restante); // Simular el tiempo de ráfaga
    log_info(logger, "CPU ha terminado de procesar el proceso con PID %d", proceso->PID);
}


// void recibir_mensaje(int socket_cliente) {
//     int size;
//     char* buffer;
//     // Asumimos que recibimos primero el tamaño del mensaje
//     recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
//     buffer = malloc(size);
//     recv(socket_cliente, buffer, size, MSG_WAITALL);
//     log_info(loggerCPU, "Mensaje recibido: %s", buffer);
//     free(buffer);
// }


/*
1. MOV_IN y MOV_OUT

// Función para enviar datos a un socket
int enviar_datos(int sockfd, void *datos, size_t tamano_datos) {
    int bytes_enviados = send(sockfd, datos, tamano_datos, 0);
    if (bytes_enviados == -1) {
        perror("Error al enviar datos");
        return -1;
    }
    return bytes_enviados;
}


2. RESIZE
 
void *redimensionar_memoria(void *ptr, size_t nuevo_tamano) {
    void *nuevo_ptr = realloc(ptr, nuevo_tamano);
    if (nuevo_ptr == NULL) {
        perror("Error al redimensionar memoria");
        // Aca debemos implementar la logica
        exit(EXIT_FAILURE);
    }
    return nuevo_ptr;
}

3. COPY_STRING

void copiar_cadena(char *destino, const char *origen) {
    strcpy(destino, origen);
}

4. IO_STDIN_READ y IO_STDOUT_WRITE
void leer_desde_stdin() {
    char buffer[256];
    printf("Ingrese un mensaje: ");
    fgets(buffer, sizeof(buffer), stdin);
    printf("Mensaje ingresado: %s", buffer);
}

void escribir_en_stdout() {
    printf("Este es un mensaje de salida estándar.\n");
}
*/

void manejarPeticionesInstruccion()
{

}

// Ciclo de instruccion: FETCH - DECODE - EXEC - CHECKT INTERRUMPT
void manejar_peticiones_al_cpu(int socketClienteKernel)      // Vamos a tener que agregar un par mas de cosa
{
    t_contexto_ejec* contexto = recibirContextoEjecucion(socketClienteKernel);
    continuar_ciclo_instruccion = true;

    pid_ejecutando = contexto -> pid;
    while (continuar_ciclo_instruccion)
    {
        // FETCH

        log_info(logger, "Fetch instruccion: 'PID: %d - FETCH - Program Counter: %d'", contexto -> pid, contexto -> registros_CPU -> PC);

        contexto -> instruccion = recibirInstruccionMemoria(contexto -> registros_CPU -> PC, contexto -> pid);

        t_instruccion* instruccion = contexto -> instruccion;

        if (instruccion -> tamanio_parametro1 == 0)
        {
            log_info(logger, "Instruccion Ejecutada: 'PID: %d - Ejecutando: %s'", contexto -> pid, instruccion -> opcode);
        }else if (instruccion -> tamanio_parametro2 == 0)
        {
            log_info(logger, "Instruccion Ejecutada: 'PID: %d - Ejecutando: %s - %s'", contexto -> pid, instruccion -> opcode, instruccion -> parametros[0]);
        }else if (instruccion -> tamanio_parametro3 == 0)
        {
            log_info(logger, "Instruccion Ejecutada: 'PID: %d - Ejecutando: %s - %s %s'", contexto -> pid, instruccion -> opcode, instruccion -> parametros[0], instruccion -> parametros[1]);
        }else
        {
            log_info(logger, "Instruccion Ejecutada: 'PID: %d - Ejecutando: %s - %s %s %s'", contexto -> pid, instruccion -> opcode, instruccion -> parametros[0], instruccion -> parametros[1], instruccion -> parametros[2]);            
        }

        contexto -> registros_CPU -> PC++;
    
        // DECODE Y EXEC

        if (strcmp(instruccion -> opcode, "SET") == 0)
        {
            instruccionSet(&contexto, instruccion);
        }
        if (strcmp(instruccion -> opcode, "SUM") == 0)
        {
            instruccionSum(&contexto, instruccion);
        }
        if (strcmp(instruccion -> opcode, "SUB") == 0)
        {
            instruccionSub(&contexto, instruccion);
        }
        if (strcmp(instruccion -> opcode, "JNZ") == 0)
        {
            instruccionJNZ(&contexto, instruccion);
        }
        if (strcmp(instruccion -> opcode, "MOV_IN") == 0)
        {
            contexto -> registros_CPU -> PC--;
            bool esPageFault = decodificar_direccion_logica(&contexto, 1);
            if(esPageFault)
            {
                continuar_ciclo_instruccion = false;
            }else{
                contexto -> registros_CPU -> PC++;
                instruccionMOVIN(&contexto,instruccion);
            }
        }
        if (strcmp(instruccion -> opcode, "MOV_OUT") == 0)
        {
            contexto -> registros_CPU -> PC--;
            bool esPageFault = decodificar_direccion_logica(&contexto, 0);
            if(esPageFault)
            {
                continuar_ciclo_instruccion = false;
            }else{
                contexto -> registros_CPU -> PC++;
                instruccionMOVOUT(&contexto,instruccion);
            }
        }
        if (strcmp(instruccion->opcode, "WAIT") == 0) {

			devolverKernel(contexto, APROPIAR_RECURSOS, socketClienteKernel);
			continuar_ciclo_instruccion = false;
		}
		if (strcmp(instruccion->opcode, "SIGNAL") == 0) {

			devolverKernel(contexto, DESALOJAR_RECURSOS, fd_cpu_modo_dispatch);
			continuar_ciclo_instruccion = false;
		}

            /*
                    SEGUIIIIIIIIIR
            =================================================*/



        // CHECK INTERRUPT
        if(interrupcion_pendiente && pid_a_desalojar != NULL && string_equals_ignore_case(pid_a_desalojar, string_itoa(contexto -> pid)))
        {
            log_info(logger, "Atendiendo interrupcion a %s y devuelvo al kernel", pid_a_desalojar);
            continuar_ciclo_instruccion = false;
            devolverKernel(contexto, INTERRUPCION, socketClienteKernel);
            interrupcion_pendiente = false;
            free(pid_a_desalojar);
            pid_a_desalojar = NULL;
        }
    }

    pid_ejecutando = 0;
    destruir_contexto_de_ejecucion(contexto);   
}

void recibirInterrupcion(int socketCliente) // En este caso el cliente va a ser el kernel
{
    char* mensaje = recibir_mensaje(socketCliente);

    log_info(logger,"Interrupcion - Motivo: %s",mensaje);

    char** arrayMensaje = string_split(mensaje, " "); 

    pid_a_desalojar = string_array_pop(arrayMensaje); // Nos devuelve el ultimo elemento del array

    interrupcion_pendiente = true;

    if(!continuar_ciclo_instruccion)
    {
        interrupcion_pendiente = false;
        free(pid_a_desalojar);
        pid_a_desalojar = NULL;
        enviar_mensaje("No hay nadie ejecutando", socketCliente, INTERRUPCION);
    }else if(pid_ejecutando && !string_equals_ignore_case(string_itoa(pid_ejecutando), pid_a_desalojar)){
        interrupcion_pendiente = false; 
        free(pid_a_desalojar);
        pid_a_desalojar = NULL;
        enviar_mensaje("El proceso ya fue desalojado, esta ejecutando otro proceso", socketCliente, INTERRUPCION);
    }
    
    free(mensaje);
    string_array_destroy(arrayMensaje);
}

void* manejarInterrupcion(void* args) // Ponemos void* para ponerlo en el hilo
{
    u_int64_t socketCliente = (u_int64_t)args; // Ver como funciona con un int

    while(1)
    {
        int codigoOp = recibir_operacion(socketCliente);

        switch (codigoOp)
        {
        case HANDSHAKE:
            recibir_handshake(socketCliente);
            break;
        case INTERRUPCION:
            recibirInterrupcion(socketCliente);
            break;
        case -1:
            log_error(logger,"El cliente se desconecto, terminando el servidor");
            break;
        default:
            log_warning(logger, "Operacion desconocida");
            break;
        }
    }

    return NULL;
}



// Le devolvemos el contexto al kernel
void devolverKernel(t_contexto_ejec* contexto, op_code opCode, int socketCliente)
{
    t_paquete* paquete = crear_paquete(opCode);
    agregar_a_paquete_sin_agregar_tamanio(paquete, &(contexto->pid), sizeof(int));
    agregar_a_paquete_sin_agregar_tamanio(paquete, &(contexto->registros_CPU->PC), sizeof(int));

    agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->AX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->BX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->CX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->DX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EAX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EBX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->ECX), sizeof(uint32_t));
	agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EDX), sizeof(uint32_t));
    
    agregar_a_paquete(paquete, contexto->instruccion->opcode, contexto->instruccion->tamanio_opcode);
    agregar_a_paquete(paquete, contexto->instruccion->parametros[0], contexto->instruccion->tamanio_parametro1);
    agregar_a_paquete(paquete, contexto->instruccion->parametros[1], contexto->instruccion->tamanio_parametro2);
    agregar_a_paquete(paquete, contexto->instruccion->parametros[2], contexto->instruccion->tamanio_parametro3);

    enviar_paquete(paquete, socketCliente);

    eliminar_paquete(paquete);
}

// Enviamos una peticion a memoria y esta nos devuelve la instrucciones
t_instruccion* recibirInstruccionMemoria(int pc, int pid)
{
    t_paquete* paquete = crear_paquete(INSTRUCCION);

    agregar_a_paquete_sin_agregar_tamanio(paquete, &pc, sizeof(int));
    agregar_a_paquete_sin_agregar_tamanio(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_socket_cpu);             // No estoy seguro de que tengamos que usar 
                                                        // Este socket
    op_code codeOp = recibir_operacion(fd_socket_cpu);

    if (codeOp !=   INSTRUCCION)
    {
        log_error(logger, "No se pudo recibir la instruccion de memoria - Codigo de operacion recibido: %d",codeOp);
        return NULL;
    }

    t_instruccion* instruccion = recibirInstruccion(fd_socket_cpu);

    eliminar_paquete(paquete);
    return instruccion;   
}






/*==========================================================================
=============           MANEJO DE INSTRUCCIONES                 ============= 
===========================================================================*/



int configTamanioPagina;


/*-----------------------------------------------------------------------------
------------ FUNCIONES PARA REALIZAR LAS INSTRUCCIONES ------------------------ 
------------------------------------------------------------------------------*/


u_int32_t obtenerValorReg(char* regLeer, t_contexto_ejec** contextoActual) // el ** es puntero de punteros
{
    u_int32_t valor = -1; // Devolvemos el valor de error si se escribio mal el nombre del registro

    if(strcmp(regLeer, "AX") == 0){
        valor = (*contextoActual) ->registros_CPU->AX; // ponemos (*contectoActual) para obtener el puntero del puntero de punteros
    }else if (strcmp(regLeer, "BX") == 0){
        valor = (*contextoActual) ->registros_CPU ->BX;
    }else if(strcmp(regLeer, "CX") == 0){
        valor = (*contextoActual) ->registros_CPU ->CX;
    }else if(strcmp(regLeer, "DX") == 0){
        valor = (*contextoActual) ->registros_CPU ->DX;
    }else if(strcmp(regLeer, "EAX") == 0){
        valor = (*contextoActual) ->registros_CPU ->EAX;
    }else if(strcmp(regLeer, "EBX") == 0){
        valor = (*contextoActual) ->registros_CPU ->EBX;
    }else if(strcmp(regLeer, "ECX") == 0){
        valor = (*contextoActual) ->registros_CPU ->ECX;
    }else if(strcmp(regLeer, "EDX") == 0){
        valor = (*contextoActual) ->registros_CPU ->EDX;
    }
    
    return valor;

    // nose si hay q agregar PC DI SI 

}

void setearReg(t_contexto_ejec** contexto, char* registro, u_int32_t valor)
{
    // Aca podriamos poner logs para ir viendo como se setean los registros pero no es obligatorio

    if(strcmp(registro,"AX") == 0){
        (*contexto) ->registros_CPU ->AX = valor;
    }else if(strcmp(registro, "BX") == 0){
        (*contexto) -> registros_CPU ->BX = valor;
    }else if(strcmp(registro, "CX") == 0){
        (*contexto) -> registros_CPU ->CX = valor;
    }else if(strcmp(registro, "DX") == 0){
        (*contexto) -> registros_CPU ->DX = valor;
    }else if(strcmp(registro, "EAX") == 0){
        (*contexto) -> registros_CPU ->EAX = valor;
    }else if(strcmp(registro, "EBX") == 0){
        (*contexto) -> registros_CPU ->EBX = valor;
    }else if(strcmp(registro, "ECX") == 0){
        (*contexto) -> registros_CPU ->ECX = valor;
    }else if(strcmp(registro, "EDX") == 0){
        (*contexto) -> registros_CPU ->EDX = valor;
    }
}


void instruccionSet(t_contexto_ejec** contexto, t_instruccion* instruccion)
{
    char * registro = strdup(instruccion->parametros[0]);
    u_int32_t valor = atoi(instruccion ->parametros[1]);
    setearReg(contexto, registro, valor);
}

void instruccionSum(t_contexto_ejec** contexto, t_instruccion* instruccion)
{
    char* registroDestino = strdup(instruccion->parametros[0]);
    char* registroOrigen = strdup(instruccion->parametros[1]);

    u_int32_t destino = obtenerValorReg(registroDestino,contexto);
    u_int32_t origen = obtenerValorReg(registroOrigen, contexto);

    destino+= origen; 

    setearReg(contexto, registroDestino, destino);
    free(registroDestino);
    free(registroOrigen);
}

void instruccionSub(t_contexto_ejec** contexto, t_instruccion* instruccion)
{
    char* registroDestino = instruccion ->parametros[0];
    char* registroOrigen = instruccion ->parametros[1];

    u_int32_t destino = obtenerValorReg(registroDestino, contexto);
    u_int32_t origen = obtenerValorReg(registroOrigen, contexto);

    destino -= origen;

    setearReg(contexto, registroDestino, destino);
    free(registroDestino);
    free(registroOrigen);
}

void instruccionJNZ(t_contexto_ejec** contexto, t_instruccion* instruccion)
{
    int nroInstruccion = atoi(instruccion->parametros[1]);
    char* registro = strdup(instruccion->parametros[0]);

    if (obtenerValorReg(registro,contexto) != 0)
    {
        (*contexto) ->registros_CPU ->PC = nroInstruccion;
    }
    
    free(registro);
}

// Funcion para leer un valor de memoria
u_int32_t leerValorMemoria(int direccionFisica, int pid) 
{
    t_paquete* paquete = crear_paquete(LEER_MEMORIA);
    t_instruccion* instruccion = malloc(sizeof(t_instruccion));

    instruccion ->opcode = string_new();
    string_append(&(instruccion->opcode), "MOV_IN");
    instruccion ->tamanio_opcode = strlen(instruccion->opcode)+1;

    char* bytesEscribir = string_itoa(sizeof(u_int32_t));
    instruccion ->tamanio_parametro1 = strlen(bytesEscribir) + 1;
    instruccion->parametros[0] = bytesEscribir;

    instruccion->parametros[1] = string_itoa(direccionFisica);
    instruccion->tamanio_parametro2 = strlen(instruccion-> parametros[1]) + 1;

    instruccion -> parametros[2] = NULL;
    instruccion ->tamanio_parametro3 = 0;

    agregar_a_paquete_sin_agregar_tamanio(paquete,&pid, sizeof(int));
    agregar_a_paquete(paquete,instruccion->opcode,sizeof(char)*instruccion->tamanio_opcode);

    agregar_a_paquete(paquete, instruccion ->parametros[0],instruccion->tamanio_parametro1);
    agregar_a_paquete(paquete, instruccion ->parametros[1],instruccion->tamanio_parametro2);
    agregar_a_paquete(paquete, instruccion ->parametros[2],instruccion->tamanio_parametro3);

    enviar_paquete(paquete,fd_socket_cpu);
    
    // libero el espacio
    free(instruccion->opcode);
    free(instruccion->parametros[0]);
    free(instruccion->parametros[1]);
    free(instruccion->parametros[2]);
    eliminar_paquete(paquete);

    // Recibimos el valor desde memoria 
    int codigoOperacion = recibir_operacion(fd_socket_cpu);
    void* valorSinFormato;
    u_int32_t valor;

    if (codigoOperacion == LEER_MEMORIA_RESPUESTA)
    {
        int tamanio;
        valorSinFormato = recibir_buffer(&tamanio,fd_socket_cpu);
        memcpy(&valor, valorSinFormato,sizeof(u_int32_t));

        // poner un log para verificar que haya salido todo bien 
    }else{
        // poner un log error
        return -1;
    }
    
    free(valorSinFormato);
    return valor;
}

// Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
void instruccionMOVIN(t_contexto_ejec** contexto, t_instruccion* instruccion) 
{
    char* registro = strdup(instruccion->parametros[0]);
    int direccionFisica = atoi(instruccion->parametros[1]);

    u_int32_t guardar = leerValorMemoria(direccionFisica, (*contexto)->pid);

    // manejo de error 
    if (guardar == -1)
    {
        free(registro);
        return;
    }
    setearReg(contexto, registro, guardar);
    free(registro);
}

int obtenerMarco(int page, int pid)
{
    t_paquete* paquete = crear_paquete(ACCESO_PAGINA);

    agregar_a_paquete_sin_agregar_tamanio(paquete, &page, sizeof(int));
    agregar_a_paquete_sin_agregar_tamanio(paquete, &pid, sizeof(int));

    enviar_paquete(paquete,fd_socket_cpu);
    eliminar_paquete(paquete);

    int tamanio;
    int frame;

    op_code codigoOp = recibir_operacion(fd_socket_cpu);

    if(codigoOp == ACCESO_PAGINA)
    {
        void* buffer = recibir_buffer(&tamanio, fd_socket_cpu);
        memcpy(&frame, buffer,sizeof(int));
        free(buffer);
        // Aca va un log con page hit
        return frame;

    }else if(codigoOp == PAGE_FAULT){
        char* mensaje = recibir_mensaje(fd_socket_cpu);
        // Aca va un log de page_fault
       log_info(logger, "Se recibio: “%s“ de memoria", mensaje);
        free(mensaje);
        return -1;
    }else{
       log_error(logger, "Codigo de operacion inesperado de memoria: %d", codigoOp);
        return -1;
    }
}

void asignarValor(int direccionFisica, u_int32_t valorAsignar, int pid)
{
    t_paquete* paquete = crear_paquete(ESCRIBIR_MEMORIA);
    t_instruccion* instruccion = malloc(sizeof(t_instruccion));

    instruccion->opcode = 	string_new();
	string_append(&(instruccion->opcode), "MOV_OUT");
	instruccion->tamanio_opcode = strlen(instruccion->opcode) +1;

	instruccion->parametros[0] = string_itoa(direccionFisica);
	instruccion->tamanio_parametro1 = strlen(instruccion->parametros[0]) +1;

	char *bytes_a_escribir = string_itoa(sizeof(valorAsignar));
	instruccion->tamanio_parametro2 = strlen(bytes_a_escribir) + 1;
	instruccion->parametros[1] = bytes_a_escribir;

	instruccion->parametros[2] = NULL ;
	instruccion->tamanio_parametro3 = 0;

    agregar_a_paquete_sin_agregar_tamanio(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, instruccion->opcode, instruccion->tamanio_opcode);
    agregar_a_paquete(paquete, instruccion->parametros[0], instruccion->tamanio_parametro1);
    agregar_a_paquete(paquete, instruccion->parametros[1], instruccion->tamanio_parametro2);
    agregar_a_paquete(paquete, instruccion->parametros[2], instruccion->tamanio_parametro3);
    agregar_a_paquete_sin_agregar_tamanio(paquete, &valorAsignar, sizeof(u_int32_t));

    enviar_paquete(paquete, fd_socket_cpu);

    // Libero el espacio 
    free(instruccion->opcode);
    free(instruccion->parametros[0]);
    free(instruccion->parametros[1]);
    free(instruccion->parametros[2]);
    eliminar_paquete(paquete);

    // Manejo la respuesta
    int codigoOp = recibir_operacion(fd_socket_cpu);
    
    if(codigoOp == ESCRIBIR_MEMORIA_RESPUESTA)
    {
        
        char* mensaje = recibir_mensaje(fd_socket_cpu);

        if (strcmp(mensaje, "OK") == 0)
        {
		    log_info(logger,"Escritura Memoria: “PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d“", pid, direccionFisica, valorAsignar);
        }else{
			log_error(logger,"PID: %d - no se pudo escribir en memoria, esto no deberia pasar, poque recibi el mensaje: %s", pid, mensaje);
        }
        free(mensaje);
    }else{
		log_error(logger,"Se recibio cod_op: %d esto no deberia pasar", codigoOp);
    }

}

void instruccionMOVOUT(t_contexto_ejec** contexto, t_instruccion* instruccion)
{
    int direccionFisica = atoi(instruccion->parametros[0]); // NOSE SI LA DIRECCION VIENE EXPRESADA EN UN REGISTRO O EN NUMEROS
    char* registro = strdup(instruccion->parametros[1]);

    u_int32_t valorGuardar = obtenerValorReg(registro, contexto);
    if (valorGuardar == -1)
    {
        // mensaje error 
        free(registro);
        return;
    }

    asignarValor(direccionFisica, valorGuardar, (*contexto)->pid);

    free(registro);
}


int traducir_direccion_logica(int direccion_logica, int pid, t_contexto_ejec* contexto)
{

	int numeroPagina = (int) floor(direccion_logica / configTamanioPagina);

	int desplazamiento = direccion_logica - numeroPagina * configTamanioPagina;

	int marcoPagina = obtenerMarco(numeroPagina,pid);

	if(marcoPagina == -1){

		log_info(logger, "Page Fault: “Page Fault PID: %d - Página: %d“", pid, numeroPagina);
		// AVISO A KERNEL PARA QUE MANEJE EL PAGE fAULT

		t_paquete *paquete = crear_paquete(PAGE_FAULT);


		agregar_a_paquete_sin_agregar_tamanio(paquete, &numeroPagina,sizeof(int));
		agregar_a_paquete_sin_agregar_tamanio(paquete, &(pid),sizeof(int));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->PC), sizeof(int));

		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->AX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->BX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->CX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->DX), sizeof(uint32_t));
        agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EAX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EBX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->ECX), sizeof(uint32_t));
		agregar_a_paquete_sin_agregar_tamanio(paquete,&(contexto->registros_CPU->EDX), sizeof(uint32_t));

		agregar_a_paquete(paquete, contexto->instruccion->opcode,contexto->instruccion->tamanio_opcode);
		agregar_a_paquete(paquete, contexto->instruccion->parametros[0],contexto->instruccion->tamanio_parametro1);
		agregar_a_paquete(paquete, contexto->instruccion->parametros[1],contexto->instruccion->tamanio_parametro2);
		agregar_a_paquete(paquete, contexto->instruccion->parametros[2],contexto->instruccion->tamanio_parametro3);

		enviar_paquete(paquete, fd_cpu_modo_dispatch);

		eliminar_paquete(paquete);

		return -1; // page fault
	} else {
	    log_info(logger, "Obtener Marco: “PID: %d - OBTENER MARCO - Página: %d - Marco: %d“", pid, numeroPagina, marcoPagina);
	}

	return desplazamiento + marcoPagina * configTamanioPagina;
}

bool decodificar_direccion_logica(t_contexto_ejec** contexto, int posicion_parametro_direccion_logica)
{
	int direccion_logica = atoi((*contexto)->instruccion->parametros[posicion_parametro_direccion_logica]);
	free((*contexto)->instruccion->parametros[posicion_parametro_direccion_logica]);
	if(posicion_parametro_direccion_logica == 0){
		(*contexto)->instruccion->tamanio_parametro1 = 0;
	} else if(posicion_parametro_direccion_logica == 1){
		(*contexto)->instruccion->tamanio_parametro2 = 0;
	}

	int direccion_fisica = traducir_direccion_logica(direccion_logica, (*contexto)->pid, *contexto);

	if(direccion_fisica == -1){
		return true;
	}

	char *direccion_fisica_string = string_itoa(direccion_fisica);

	int tam_direccion_fisica_string = strlen(direccion_fisica_string) + 1;
	(*contexto)->instruccion->parametros[posicion_parametro_direccion_logica] = malloc(sizeof(char) * tam_direccion_fisica_string);
	strcpy((*contexto)->instruccion->parametros[posicion_parametro_direccion_logica], direccion_fisica_string);

	if(posicion_parametro_direccion_logica == 0){
		(*contexto)->instruccion->tamanio_parametro1 = tam_direccion_fisica_string;
	} else if(posicion_parametro_direccion_logica == 1){
		(*contexto)->instruccion->tamanio_parametro2 = tam_direccion_fisica_string;
	}

	free(direccion_fisica_string);
	return false;
}
