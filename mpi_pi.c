#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
 
#define MASTER             0
#define PI      3.1415926535
#define NDARTS     100000000
 
double pseudo_random(double a, double b) 
{
	double r;
	r = ((b - a) * ((double) rand()/(double) RAND_MAX)) + a;
	return r;
}

int main (int argc, char *argv[]) 
{
	int proc_id, n_procs, llimit, ulimit, n_circle, i;
	double pi_current, pi_sum, x, y, z, error, start_time, end_time;

	struct timeval stime;

	llimit   = -1;
	ulimit   = 1;
	n_circle = 0;

	/* Inicio PI */
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
		printf("## Total de tiradas al círculo:		%d\n\n", NDARTS);
 
		start_time = MPI_Wtime();
	}

	/* Procedimiento: Obtener un numero aleatorio, calcular por Pitágoras si está dentro del
	círculo, y si cumple, incrementar la cuenta... */

	gettimeofday(&stime, NULL);
	srand(stime.tv_usec * stime.tv_usec * stime.tv_usec * stime.tv_usec);

	for (i = 1; i <= NDARTS; i++)
	{
		x = pseudo_random(llimit, ulimit);
		y = pseudo_random(llimit, ulimit);

		z = pow(x, 2) + pow(y, 2);

		if (z <= 1.0)
			n_circle+=1;
	}

	/*... Luego calcular PI con la cuenta dada... */
	pi_current = 4.0 * (double)n_circle/(double)NDARTS;

	/* Actualizar el resultado con MPI_Reduce*/
	MPI_Reduce(&pi_current, &pi_sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

	/* Unicamente maestro: calcular la media, error, imprimir resultados. */
	if (proc_id == MASTER)
	{
		pi_sum = pi_sum / n_procs;
		error = fabs((pi_sum - PI)/PI) * 100;

		end_time = MPI_Wtime();

		printf("## Valor real:		%11.10f\n", PI);
		printf("## Valor calculado:	%11.10f\n", pi_sum);
		printf("## Error:		%10.8f\n", error);
		printf("## Tiempo empleado:	%10.8f\n\n", end_time - start_time);
	}

	/* Terminar espacio MPI */
	MPI_Finalize();

	return 0;
}
