#include <utils/t_datos.h>

// Función para crear un nuevo PCB
t_pcb* crearPCB() {
    t_pcb* nuevoPCB = malloc(sizeof(t_pcb)); 
    if (nuevoPCB == NULL) {
        return NULL; // Manejo de error si no se puede asignar memoria
    }
    nuevoPCB->PID = rand() % 10000; // Forzamos q el valor sea entre 0 y 9999
    nuevoPCB->program_counter = 1;
    nuevoPCB->quantum = malloc(sizeof(int));
    nuevoPCB->tiempo_restante = malloc(sizeof(int)); // Inicialmente, el tiempo restante es igual al tiempo de ráfaga
    nuevoPCB->estado_proceso = malloc(8);
    strcpy(nuevoPCB->estado_proceso, "NEW");
    nuevoPCB->registros_cpu = malloc(sizeof(t_reg_cpu));
    nuevoPCB->comando = malloc(sizeof(t_instruccion));
    return nuevoPCB;
}