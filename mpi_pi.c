#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>
#include "mpi_pi.h"
 
int main (int argc, char *argv[]) 
{
	int proc_id, n_procs, limite_inf, limite_sup, n_dentro, i;
	double pi_local, pi_total, x, y, z, error, start_time, end_time;

	struct timeval stime;

	limite_inf   = -1;
	limite_sup   = 1;
	n_dentro = 0;

	/* Inicio MPI */
	MPI_Init(&argc, &argv);

	/* Obtener rango y tamaño del comm_world */
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

	/* Unicamente el maestro imprime en pantalla los datos iniciales */ 
	if (proc_id == MASTER)
	{
		system("clear");
		printf("## Implementación de Montecarlo para el cálculo de PI\n\n");
		printf("## Total de procesadores:			%d\n", n_procs);
		printf("## Total de tiradas al círculo:		%d\n\n", TIRADAS);
 
		start_time = MPI_Wtime();
	}

	/* Procedimiento: Obtener un numero aleatorio, calcular por Pitágoras si está dentro del
	círculo, y si cumple, incrementar la cuenta... */

	gettimeofday(&stime, NULL);
	srand(stime.tv_usec * stime.tv_usec * stime.tv_usec * stime.tv_usec);

	for (i = 1; i <= TIRADAS; i++)
	{
		x = genera_aleatorio(limite_inf, limite_sup);
		y = genera_aleatorio(limite_inf, limite_sup);

		z = pow(x, 2) + pow(y, 2);

		if (z <= 1.0)
			n_dentro+=1;
	}

	/*... Luego calcular PI con la cuenta dada... */
	pi_local = 4.0 * (double)n_dentro/(double)TIRADAS;

	/* Actualizar el resultado, suma de todos los cálculos... */
	MPI_Reduce(&pi_local, &pi_total, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

	/* Unicamente maestro: calcular la media, error, imprimir resultados. */
	if (proc_id == MASTER)
	{
		pi_total = pi_total / n_procs;
		error = fabs((pi_total - PI)/PI) * 100;

		end_time = MPI_Wtime();

		printf("## Valor real:		%11.10f\n", PI);
		printf("## Valor calculado:	%11.10f\n", pi_total);
		printf("## Error:		%10.8f\n", error);
		printf("## Tiempo empleado:	%10.8f\n\n", end_time - start_time);
	}

	/* Terminar espacio MPI */
	MPI_Finalize();

	return 0;
}

double genera_aleatorio(double a, double b) 
{
	return (((b - a) * ((double) rand()/(double) RAND_MAX)) + a);
}
