#ifndef LOGGER_H_
#define LOGGER_H_

#include <commons/log.h>
#include <stdlib.h>
#include <stdio.h>

extern t_log* logger;

t_log* iniciar_logger(char*, char*, int, t_log_level);

#endif