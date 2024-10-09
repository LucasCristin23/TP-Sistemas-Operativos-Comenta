#include "main.h"

// Esto podriamos ponerlo en el .h para que quede mas limpio el .c
t_queue* paginasReferenciadas; 
t_dictionary* paginasXpid;
t_list* situacionFrames;
void* espacioUsuario;

int main() {

    //----------------			 Iniciamos el Logger 			--------------------------
	
	logger = iniciar_logger("memoria.log", "Logger_Memoria", 1, LOG_LEVEL_INFO);
 
	//----------------			Iniciamos y leemos las config   --------------------------
	init_config_memoria();


	int configTamanioMemoria = valores_config.TAM_MEMORIA;
	int configTamanioPagina = valores_config.TAM_PAGINA;

	// Definimos marcos de páginas
	int marcos = valores_config.TAM_MEMORIA / valores_config.TAM_PAGINA;


	//Transformamos el int en un string 
	int a = valores_config.PUERTO_ESCUCHA;
    char puerto[10];
    sprintf(puerto, "%d", a);

	// Inicio servidor de memoria. ORDEN PARA LEVANTAR MODULOS: MEMORIA - CPU - KERNEL - I/O
	memoria_fd = iniciar_servidor(puerto, logger, "Memoria");

	// Esperar a CPU	
	log_info(logger, "Esperando a CPU");
	socket_cpu = esperar_cliente(memoria_fd, logger, "CPU");
	
	//esperar conexion de kernel
	log_info(logger, "Esperando a Kernel");
	socket_kernel = esperar_cliente(memoria_fd, logger, "Kernel");
	
	//esperar conexion i/o
	log_info(logger, "Esperando a IO");
	socket_io = esperar_cliente(memoria_fd, logger, "I/O");
	
	// Inicializamos el diccionario de instrucciones


	// Inicializamos la paginacion simple
	iniciarPaginacion();


// Atender a CPU  =============================================================================================================================
    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu,NULL,(void*)atender_memoria_cpu,NULL); 
    pthread_detach(hilo_cpu); 
// Atender a Kernel ======================================================================================================================================
    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel,NULL,(void*)atender_memoria_kernel,NULL); 
    pthread_detach(hilo_kernel);
// Atender a I/O ======================================================================================================================================
    pthread_t hilo_io;
    pthread_create(&hilo_io,NULL,(void*)atender_memoria_IO,NULL); 
    pthread_join(hilo_io, NULL);  
	

	
	//Liberamos espacio
	terminar_programa(config_memoria, logger, memoria_fd); 
	return 0;
 
}



/*
-------------------------------------------------------------------------------------
----------------------- DECLARACION DE FUNCIONES ------------------------------------
-------------------------------------------------------------------------------------
*/




// Funcion para iniciar config de Memoria
void init_config_memoria(void)
{
    config_memoria = config_create("./memoria.config");
    if (config_memoria == NULL){
        perror("ERROR AL CARGAR EL CONFIG!");
        exit(EXIT_FAILURE);
	}
	valores_config.PUERTO_ESCUCHA = config_get_int_value(config_memoria, "PUERTO_ESCUCHA");
	valores_config.TAM_MEMORIA = config_get_int_value(config_memoria, "TAM_MEMORIA");
	valores_config.TAM_PAGINA = config_get_int_value(config_memoria, "TAM_PAGINA");
	valores_config.PATH_INSTRUCCIONES = config_get_string_value(config_memoria, "PATH_INSTRUCCIONES");
	valores_config.RETARDO_RESPUESTA = config_get_int_value(config_memoria, "RETARDO_RESPUESTA");
};

// FUNCIONES PARA ATENDER OTROS MODULOS
void atender_memoria_cpu(void){
        bool control = 1;
        log_info(logger, "Esperando instrucciones de CPU");
        while (control) {
		int cod_op = recibir_operacion(socket_cpu);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case HANDSHAKE:
		/* code */
			break;
		case RPTA_HANDSHAKE:
		/* code */
			break;
		case CREAR_PROCESO:
			
			break;
		case TERMINAR_PROCESO:
		/* code */
			break;

		case INSTRUCCION:
			enviar_instrucciones_a_cpu(socket_cpu); // Lo mismo q abajo
			break;
		case ACCESO_PAGINA:
			devolver_marco(socket_cpu); // no estoy completamente seguro de q vaya este socket
			break;
		case PAGE_FAULT: 
			//  PARA ESTE TENGO QUE ENCARAR EL FILESYSTEM
			break;
		case ESCRIBIR_MEMORIA:
			escribir_memoria(socket_cpu);// idem devolver_marco();
			break;
		case LEER_MEMORIA:
			leer_memoria(socket_cpu); // idem devolver_marco();
			break; 

			
		case -1:
			log_error(logger, "el cliente CPU se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(logger,"Operacion desconocida de CPU.");
			break;
		}
	}
}
void atender_memoria_kernel(void){
        bool control = 1;
        log_info(logger, "Esperando instrucciones de Kernel");
        while (control) {
		int cod_op = recibir_operacion(socket_kernel);
		switch (cod_op) {

		case CREAR_PROCESO:
			crear_proceso(socket_kernel);
			break;
		case TERMINAR_PROCESO:
		/* code */
			break;
		case EJECUTAR_SCRIPT:
		
			
			break;	
		case -1:
			log_error(logger, "el cliente Kernel se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(logger,"Operacion desconocida de Kernel.");
			break;
		}
	}
}
void atender_memoria_IO(void){
        bool control = 1;
        log_info(logger, "Esperando instrucciones de I/O");
        while (control) {
		int cod_op = recibir_operacion(socket_io);
		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(fd_bidireccional_kernel_dispatch);
			break;
		case PAQUETE:
                //
			break;
		case HANDSHAKE:
		/* code */
			break;
		case RPTA_HANDSHAKE:
		/* code */
			break;
		case CREAR_PROCESO:
			
			break;
		case TERMINAR_PROCESO:
		/* code */
			break;
		case -1:
			log_error(logger, "el cliente I/O se desconecto. Terminando");
			control = 0;
            //return EXIT_FAILURE;
            break;
		default:
			log_warning(logger,"Operacion desconocida de I/O.");
			break;
		}
	}
}




//Funcion para crear procesos
void crear_proceso(int socket_cliente){

	leer_archivo(socket_cliente);

	

	// proceso_t* proceso = malloc(sizeof(proceso_t));

	// proceso->nombre = malloc(strlen(nombre) + 1); //Agrego el "+1" para el caracter "\0"
	// strcpy(proceso->nombre, nombre);

	// proceso->path_proceso = malloc(strlen(path) + 1); //Agrego el "+1" para el caracter "\0"
	// strcpy(proceso->path_proceso, path);

	// proceso->cod_op = CREAR_PROCESO;

}

//Es la CPU la que le va a pedir las instrucciones a memoria y es la CPU la que les va a indicar que línea del archivo le tienen que mandar.

//Funcion para leer archivo de pseudocdigo (Retorna NULL en caso de error)
void leer_archivo(int socket_cliente){

	char* PATH; //El path pasado por consola, éste se concatena con la direccion del directorio donde van a estar los archivos de pseudocodigo
	int PID;

	recibir_path_pid(socket_cliente, &PATH, &PID);

	char* instrucciones = string_new(); //lo cambié un char* para usar algunas funciones de las commons
	string_append(&instrucciones, valores_config.PATH_INSTRUCCIONES);
	string_append(&instrucciones, "/");
	string_append(&instrucciones, PATH);
	//acá tenemos la ruta relativa, tenemos que sacar la ruta absoluta

	if(string_contains(instrucciones, "./")){
		char* buffer = malloc(100*sizeof(char));
		getcwd(buffer, 100); //getcwd devuelve la ruta absoluta
		string_append(&buffer, "/"); //agrego la barra (/) al final
		instrucciones = string_replace(instrucciones, "./", buffer);
	}else if(string_contains(instrucciones, "~/")){
		instrucciones = string_replace(instrucciones, "~/", "/home/utnso/");
	}


	//abrimos el archivo
	FILE* file = fopen(instrucciones, "r");
	 if(file == NULL){
		log_error(logger, "Error al intentar abrir el archivo!!");
		free(instrucciones);
	 	return;
	 }

	//buffer
	char* buffer_linea; //lo mismo que con la variable instrucciones más arriba
	
	t_list* listaInstrucciones = list_create();

	//leo el pseudocodigo, pongo en la lista
	while(feof(file) == 0)
	{
		buffer_linea = malloc(300); //Para solucionar el segmentation fault
		char* resultado_linea = fgets(buffer_linea, 300, file);

		//si el char esta vacio, hace break.
		if(resultado_linea == NULL)
		{
			log_error(logger, "No se encontró el archivo o éste está vacio");
			break;
		}

		// se borra los '\n'
		if(string_contains(buffer_linea, "\n")){
			char** array_de_cadenas = string_split(buffer_linea, "\n");

			buffer_linea = string_array_pop(array_de_cadenas);


			while(strcmp(buffer_linea, "") == 0){
				buffer_linea = string_array_pop(array_de_cadenas);
			}

			string_array_destroy(array_de_cadenas);
		}

		/*ya tengo la linea del codigo, ahora tengo que separarlos en opcode
		y los parametros. Esta dividido por un espacio*/

		//creo t_instruccion
		t_instruccion* puntero_instruc = malloc(sizeof(t_instruccion));

		//leo el opcode
		char* token = strtok(buffer_linea," ");
		puntero_instruc->opcode = token;

		puntero_instruc->tamanio_opcode = strlen(puntero_instruc->opcode) + 1;

		puntero_instruc->parametros[0] = NULL;
		puntero_instruc->parametros[1] = NULL;
		puntero_instruc->parametros[2] = NULL;

		//leo parametros
		token = strtok(NULL, " ");
		int n = 0;
		while(token != NULL)
		{
			puntero_instruc->parametros[n] = token;
			token = strtok(NULL, " ");
			n++;
		}

		if(puntero_instruc->parametros[0] != NULL){
			puntero_instruc->tamanio_parametro1 = strlen(puntero_instruc->parametros[0])+1;
		} else {
			puntero_instruc->tamanio_parametro1 = 0;
		}
		if(puntero_instruc->parametros[1] != NULL){
			puntero_instruc->tamanio_parametro2 = strlen(puntero_instruc->parametros[1])+1;
		} else {
			puntero_instruc->tamanio_parametro2 = 0;
		}
		if(puntero_instruc->parametros[2] != NULL){
			puntero_instruc->tamanio_parametro3 = strlen(puntero_instruc->parametros[2])+1;
		} else {
			puntero_instruc->tamanio_parametro3 = 0;
		}

		list_add(listaInstrucciones,puntero_instruc);

	}

	dictionary_put(instrucciones_por_pid, string_itoa(PID),listaInstrucciones); //me tira segmentation fault por string_itoa(PID)

	enviar_mensaje("OK", socket_cliente, INICIAR_PROCESO);

	free(instrucciones);
	fclose(file);
}

void enviar_instrucciones_a_cpu(int socketCliente)
{
	int pc;
	int pid;

	recibirProgramCounter(socketCliente, &pid, &pc);

	t_list* listaInstrucciones = dictionary_get(instrucciones_por_pid, string_itoa(pid));

	// Manejo del posible error de que no se encuentre la lista de instrucciones
	if (listaInstrucciones == NULL)
	{
		log_error(logger, "No se encontro la lista de instrucciones");
	}

	// En el caso de que SI encuentre la instruccion
	t_instruccion* instruccion = list_get(listaInstrucciones, pc-1);

	t_paquete* paquete = crear_paquete(INSTRUCCION);
	agregar_a_paquete(paquete, instruccion->opcode, instruccion->tamanio_opcode);
	agregar_a_paquete(paquete, instruccion->parametros[0], instruccion->tamanio_parametro1);
	agregar_a_paquete(paquete, instruccion->parametros[1], instruccion->tamanio_parametro2);
	agregar_a_paquete(paquete, instruccion->parametros[2], instruccion->tamanio_parametro3);

	tiempo_de_espera(valores_config.RETARDO_RESPUESTA);

	enviar_paquete(paquete, socketCliente);

	eliminar_paquete(paquete);
}

void devolver_marco(int socketCliente)
{
	int tamanio;
	void* buffer = recibir_buffer(&tamanio, socketCliente);
	int numeroPagina;
	int PIDnumero;

	memcpy(&numeroPagina, buffer, sizeof(int));
	memcpy(&PIDnumero, buffer + sizeof(int), sizeof(int));

	char* PIDstring = string_itoa(PIDnumero);

	// Creamos la referencia a la pagina
	t_referenciaXpid* paginaReferenciada = malloc(sizeof(t_referenciaXpid));
	paginaReferenciada -> pid = strdup(PIDstring);
	paginaReferenciada -> numeroPagina = numeroPagina;

	queue_push(paginasReferenciadas, paginaReferenciada);

	// Buscamos si esta presente la pagina
	t_list* listaDeMarcos = dictionary_get(paginasXpid,PIDstring);

	t_tabla_de_paginas* entradaPagina = list_get(listaDeMarcos , numeroPagina);

	// Ahora que ya tenemos si la pagina esta o no
	if(entradaPagina -> presencia)
	{
		t_paquete* paquete = crear_paquete(ACCESO_PAGINA);
		
		agregar_a_paquete_sin_agregar_tamanio(paquete, &(entradaPagina -> marco), sizeof(int));

		// Fijarme si es o no obligatorio este log, lo pongo por las dudas
		// log_info(logger, "Acceso la tabla de paginas: 'PID: %d - Pagina: %d - Marco %d'", PIDnumero, numeroPagina, entradaPagina -> marco);

		tiempo_de_espera(valores_config.RETARDO_RESPUESTA);

		enviar_paquete(paquete,socketCliente);

		// Aca no estoy muy seguro de eliminar el paquete, fijarme si se rompe
	}else{
		tiempo_de_espera(valores_config.RETARDO_RESPUESTA);

		enviar_mensaje("No se encontro el marco buscado en memoria", socketCliente, PAGE_FAULT);
	}

	free(buffer);
}


void leer_memoria(int socketCliente)
{
	t_instruccion* instruccion;
	int tamanio;
	void* buffer = recibir_buffer(&tamanio, socketCliente);
	int pid;
	int desplazamiento = 0;
	int direccionFisica;

	memcpy(&pid, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	instruccion = deserializarInstruccion(buffer, &desplazamiento);

	direccionFisica = atoi(instruccion -> parametros[1]);

	void* contenidoPagina = malloc(valores_config.TAM_PAGINA + 1);

	// NO ESTOY SEGURO DE QUE ESTE LOG SEA OBLIGATORIO, LO PONGO POR LA DUDA
	log_info(logger, "Acceso a espacio de usuario: 'PID: %d - Accion: Leer - Direccion fisica: %d'", pid, direccionFisica);

	// Aca lo que hacemos es basicamente copiar el contenido de la pagina
	memcpy(contenidoPagina, espacioUsuario + direccionFisica, valores_config.TAM_PAGINA);

	t_paquete* paquete = crear_paquete(LEER_MEMORIA_RESPUESTA);
	agregar_a_paquete_sin_agregar_tamanio(paquete, contenidoPagina, valores_config.TAM_PAGINA);
	
	tiempo_de_espera(valores_config.RETARDO_RESPUESTA);

	enviar_paquete(paquete, socketCliente);

	eliminar_paquete(paquete);

	// Liberamos espacio
	free(buffer);
	free(instruccion->opcode);
	free(instruccion->parametros[0]);
	free(instruccion->parametros[1]);
	free(instruccion->parametros[2]);
	free(instruccion);
}

void escribir_memoria(int socketCliente)
{
	t_instruccion* instruccion;
	u_int32_t escribir;
	int tamanio;
	void* buffer = recibir_buffer(&tamanio, socketCliente);
	int pid;
	int desplazamiento = 0;
	int direccionFisica;

	memcpy(&pid, buffer, sizeof(int));
	desplazamiento += sizeof(int);

	instruccion = deserializarInstruccion(buffer,&desplazamiento);

	memcpy(&escribir, buffer + desplazamiento, sizeof(u_int32_t));

	direccionFisica = atoi(instruccion->parametros[0]); 

	int numeroFrame = (int)floor(direccionFisica / valores_config.TAM_PAGINA);

	t_list* listaTablaDePagina = dictionary_get(paginasXpid, string_itoa(pid));

	bool verificadorMarco(void* args)
	{
		t_tabla_de_paginas* marco_x = (t_tabla_de_paginas*)args;
		return numeroFrame == marco_x -> marco;
	}

	// No estoy muy seguro de esto
	t_tabla_de_paginas* entradaTablaDePagina = list_find(listaTablaDePagina, verificadorMarco);
	entradaTablaDePagina -> modificado = true; // por si lo necesita algun algoritmo

	log_info(logger, "Acceso a espacio de usuario: 'PID: %d - Accion: Escribir - Direccion fisica: %d'", pid, direccionFisica);

	memcpy(espacioUsuario + direccionFisica, &escribir, sizeof(u_int32_t));

	tiempo_de_espera(valores_config.RETARDO_RESPUESTA);

	enviar_mensaje("OK", socketCliente, ESCRIBIR_MEMORIA_RESPUESTA);

	// Liberamos espacio
	free(buffer);
	free(instruccion -> opcode);
	free(instruccion -> parametros[0]);
	free(instruccion -> parametros[1]);
	free(instruccion -> parametros[2]);
	free(instruccion);
}




// --------------------------- PAGINACION ---------------------------
void iniciarPaginacion()
{
	// Diccionario de pagina por PID
	paginasXpid = dictionary_create();

	// Lista de marcos y modificados
	situacionFrames = list_create();

    tamanioPagina = valores_config.TAM_PAGINA; // Mismo tamaño q los frames
    cantidadFrames = valores_config.TAM_MEMORIA / tamanioPagina;

	for(int i = 0; i < cantidadFrames; i++)
	{
		t_situacion_marco* marco_i = malloc(sizeof(t_situacion_marco));

		marco_i -> numeroMarco = i;
		marco_i -> posicionInicioMarco = i * tamanioPagina;
		marco_i -> libre = true;
		marco_i -> pid = 0;
		list_add(situacionFrames,marco_i);
	}

	// Espacio de usuario 
    espacioUsuario = malloc(valores_config.TAM_MEMORIA);
	if(espacioUsuario == NULL)
    {
        log_error(logger,"Fallo la asignacion de memoria principal \n");
        EXIT_FAILURE;
    }

	// Lista de instrucciones por PID
	instrucciones_por_pid = dictionary_create();

	// Lista de referencias a la pagina
	paginasReferenciadas = queue_create();

    log_info(logger, "La memoria cuenta con: %d frames", cantidadFrames);
}