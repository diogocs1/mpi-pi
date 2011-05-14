#ifndef _MPI_PI_H_
#define _MPI_PI_H_

/* Valores locales */
#define MASTER             0
#define PI      3.1415926535

/* TIRADAS es una constante definida, se puede cambiar su valor y recompilar */
#define TIRADAS    100000000

/* Funciones */
double genera_aleatorio(double, double);
void obtener_info_sist();

#endif /* _MPI_PI_H_ */
