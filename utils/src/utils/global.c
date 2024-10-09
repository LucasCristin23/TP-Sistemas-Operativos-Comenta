#include "global.h"


/*
--------------------------------------------------------------------------------------
--------- EN GLOBAL SE DESARROLLAN TODAS LAS FUNCIONES CLIENTE / SERVIDOR-------------
--------------------------------------------------------------------------------------
*/


// ------------------- CLIENTE -------------------------------------------------------

void crear_buffer(t_paquete* paquete) 					//		ESTA NO
{														//		SE USA
	paquete->buffer = malloc(sizeof(t_buffer));			//		LA USA
	paquete->buffer->size = 0;							//		CREAR_PAQUETE()
	paquete->buffer->stream = NULL;						//
}




t_paquete* crear_paquete(op_code operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = operacion;
	crear_buffer(paquete);
	return paquete;
}



t_paquete_opcode* crear_paquete_operacion(op_code operacion)
{
	t_paquete_opcode* paquete_operacion = malloc(sizeof(t_paquete_opcode));
	paquete_operacion->codigo_operacion = operacion;
	return paquete_operacion;
}


// LLENAR BUFFER 
void agregar_a_paquete_sin_agregar_tamanio(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);

	memcpy(paquete->buffer->stream + paquete->buffer->size , valor, tamanio);

	paquete->buffer->size += tamanio;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}



//ENVIAR PAQUETE
void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}


void enviar_paquete_operacion(t_paquete_opcode* paquete, int socket_cliente) //Sirve para enviar un codigo de operacion (IMPORTANTE: Solo la operacion, nada extra)
{
	int bytes = sizeof(int);
	void* a_enviar = serializar_paquete_operacion(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}


void* serializar_paquete_operacion(t_paquete_opcode* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);


	return magic;
}


void enviar_operacion(op_code operacion, int socket_cliente) //Se le pasa por parametro la operacion a realizar y el socket
{
    t_paquete_opcode* paquete = crear_paquete_operacion(operacion);

    enviar_paquete_operacion(paquete, socket_cliente);

    free(paquete);
	
}


void enviar_mensaje(char* mensaje, int socket_cliente, op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo;

	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) +1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

// Retardo simulado, la entrada es en MILIsegundos y la salida en MICROsegundos
void tiempo_de_espera(int tiempo_a_esperar_en_milisegundos)
{
	usleep(tiempo_a_esperar_en_milisegundos*1000);
}


char* listar_pids_cola(t_queue* cola_con_pids){
	char* pids = string_new();

	t_list* lista_cola = cola_con_pids->elements;

	void unir_pids(void* arg_pcb_n, char* pids){
		t_pcb* pcb_n = (t_pcb*) arg_pcb_n;

		if(string_length(pids) == 0){
			string_append_with_format(&pids, "%d", pcb_n->PID);
		} else {
			string_append_with_format(&pids, ", %d", pcb_n->PID);
		}
	}

	list_iterate(lista_cola, (void*)unir_pids);

	return pids;
}



// ---------------------------- Funciones de SERVIDOR ----------------------------------------------

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

char* recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	int length_buffer = strlen(buffer);
	buffer = realloc(buffer, length_buffer + 2);
	buffer[length_buffer +1] = '\0';

	return buffer;
}


void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}


void recibir_path_pid(int socket_cliente, char** path, int* pid)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		*path = malloc(tamanio);

		memcpy(*path, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;

		memcpy(pid, buffer+desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
	}
	free(buffer);
}

void recibir_handshake(int socketCliente)
{
	int tamanio;
	char* buffer = recibir_buffer(&tamanio, socketCliente);

	if(strcmp(buffer, "OK") == 0)
	{
		enviar_mensaje("OK",socketCliente,HANDSHAKE);
	}else{
		enviar_mensaje("ERROR", socketCliente,HANDSHAKE);
	}

	free(buffer);
}


t_instruccion *deserializarInstruccion(void *buffer, int* desplazamiento){
	t_instruccion * instruccion = malloc(sizeof(t_instruccion));

	memcpy(&(instruccion->tamanio_opcode), buffer + *desplazamiento, sizeof(int));
	*desplazamiento+=sizeof(int);
	instruccion->opcode = malloc( instruccion->tamanio_opcode);
	memcpy( instruccion->opcode, buffer+*desplazamiento,  instruccion->tamanio_opcode);
	*desplazamiento+= instruccion->tamanio_opcode;

	memcpy(&( instruccion->tamanio_parametro1), buffer+*desplazamiento, sizeof(int));
	*desplazamiento+=sizeof(int);
	 instruccion->parametros[0] = malloc( instruccion->tamanio_parametro1);
	memcpy( instruccion->parametros[0], buffer + *desplazamiento,  instruccion->tamanio_parametro1);
	*desplazamiento +=  instruccion->tamanio_parametro1;

	memcpy(&( instruccion->tamanio_parametro2), buffer+*desplazamiento, sizeof(int));
	*desplazamiento+=sizeof(int);
	 instruccion->parametros[1] = malloc( instruccion->tamanio_parametro2);
	memcpy( instruccion->parametros[1], buffer + *desplazamiento,  instruccion->tamanio_parametro2);
	*desplazamiento +=  instruccion->tamanio_parametro2;


	memcpy(&( instruccion->tamanio_parametro3), buffer+*desplazamiento, sizeof(int));
	*desplazamiento+=sizeof(int);
	 instruccion->parametros[2] = malloc( instruccion->tamanio_parametro3);
	memcpy( instruccion->parametros[2], buffer + *desplazamiento,  instruccion->tamanio_parametro3);
	*desplazamiento +=  instruccion->tamanio_parametro3;

	return instruccion;
}

t_instruccion* recibirInstruccion(int socketCliente)
{
	int tamanio;
	void* buffer = recibir_buffer(&tamanio, socketCliente);

	int desplazamiento = 0;
	t_instruccion* instruccion;

	while(desplazamiento < tamanio)
	{
		instruccion = deserializarInstruccion(buffer, &desplazamiento);
	}

	free(buffer);
	return instruccion;
}

void recibirProgramCounter(int socketCliente, int* pid, int* PC)
{
	int tamanio;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&tamanio, socketCliente);
	while(desplazamiento > tamanio)
	{
		memcpy(PC, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(pid, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
	}
	
	free(buffer);
}

t_contexto_ejec* recibirContextoEjecucion(int socketCliente)
{
	int tamanio;
	int desplazamiento = 0;
	void* buffer;
	int pc;

	t_contexto_ejec* contexto = malloc(sizeof(t_contexto_ejec));
	contexto->registros_CPU = malloc(sizeof(t_reg_cpu));

	buffer = recibir_buffer(&tamanio, socketCliente);

	while(desplazamiento < tamanio)
	{
		memcpy(&(contexto->pid), buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&pc, buffer + desplazamiento, sizeof(int)); // SI HAY UN ERROR REVISAR COMO ENVIO
		desplazamiento += sizeof(int);						// EL PROGRAM COUNTER

		memcpy(&(contexto -> registros_CPU -> AX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> BX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> CX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU-> DX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> EAX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> EBX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> ECX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto -> registros_CPU -> EDX), buffer + desplazamiento,sizeof(uint32_t));
		desplazamiento+=sizeof(uint32_t);

		// En el caso de que tambien se haya enviado la instruccion 
		if (desplazamiento < tamanio)
		{
			contexto -> instruccion = deserializarInstruccion(buffer, &desplazamiento);
		}else{
			contexto -> instruccion = NULL;
		}	
	}

	free(buffer);
	return contexto;
}

void destruir_instruccion(t_instruccion* instruccion)
{
    free(instruccion->opcode);

    if(instruccion->tamanio_parametro1 != 0 && instruccion->tamanio_parametro2 != 0 && instruccion->tamanio_parametro3 != 0){

    	free(instruccion->parametros[0]);
		free(instruccion->parametros[1]);
		free(instruccion->parametros[2]);

    } else if(instruccion->tamanio_parametro1 != 0 && instruccion->tamanio_parametro2 != 0 ){

    	free(instruccion->parametros[0]);
		free(instruccion->parametros[1]);

    } else if(instruccion->tamanio_parametro1 != 0 ){

    	free(instruccion->parametros[0]);

    }

    free(instruccion);
}


void destruir_contexto_de_ejecucion(t_contexto_ejec* contexto)
{

	if(contexto->instruccion != NULL){
		destruir_instruccion(contexto->instruccion);
	}

	free(contexto);
}

t_contexto_ejec* deserializar_contexto_de_ejecucion(void *buffer, int size_buffer, int *desplazamiento)
{

	int pc;

	t_contexto_ejec* contexto = malloc(sizeof(t_contexto_ejec));
	contexto->registros_CPU = malloc(sizeof(t_reg_cpu));

	while(*desplazamiento < size_buffer )
	{

		memcpy(&(contexto->pid), buffer+*desplazamiento, sizeof(int));
		*desplazamiento+=sizeof(int);

		memcpy(&pc, buffer + *desplazamiento, sizeof(int));
		*desplazamiento+=sizeof(int);

		memcpy(&(contexto->registros_CPU->AX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->BX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->CX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->DX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->EAX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->EBX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->ECX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		memcpy(&(contexto->registros_CPU->EDX), buffer + *desplazamiento,sizeof(uint32_t));
		*desplazamiento+=sizeof(uint32_t);

		if(*desplazamiento < size_buffer){
			contexto->instruccion = deserializarInstruccion(buffer, desplazamiento);
		} else {
			contexto->instruccion = NULL;
		}

	}

	contexto -> registros_CPU -> PC = pc;

	return contexto;
}

