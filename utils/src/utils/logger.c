#include "logger.h"

t_log* logger;

t_log* iniciar_logger(char* direc, char* name, int is_active, t_log_level level )
{
	t_log* nuevo_logger;

	nuevo_logger = log_create(direc, name, is_active, level);
	if(nuevo_logger == NULL){
		perror("ALGO FALLO CON EL LOGGER");
		exit(EXIT_FAILURE);
	};
	return nuevo_logger;
};