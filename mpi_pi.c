/*	MPI_PI calcula el valor de PI con error en base a un predefinido. 
	Compilar: 
		$ mpicc -Wall mpi_pi.c -o mpi_pi.x
	Ejecutar:
		$ mpirun -np X mpi_pi.x		#X = cantidad de procesos.

	Alumno: Gonzalo Sánchez Marqués. Matrícula: 200405023.
	Ing. en Computación. FCEFyN, UNC. 2011.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <mpi.h>
#include "mpi_pi.h"
 
int main (int argc, char *argv[]) 
{
	int proc_id, n_procs, limite_inf, limite_sup, n_dentro, i;
	double pi_local, pi_total, x, y, z, error, start_time, end_time;

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

		obtener_info_sist();

		printf("## Total de procesadores: %d\n", n_procs);
		printf("## Comienzo del programa con %d tiradas.\n\n", TIRADAS);
	}

	/* Procedimiento: Obtener un numero aleatorio, calcular por Pitágoras si está dentro del
	círculo, y si cumple, incrementar la cuenta... */

	srand(time(NULL));

	if (proc_id == MASTER)
		start_time = MPI_Wtime();

	for (i = 1; i <= TIRADAS; ++i)
	{
		x = rand()/(double)RAND_MAX;
		y = rand()/(double)RAND_MAX;

		z = pow(x, 2) + pow(y, 2);

		if (z <= 1.0)
			n_dentro+=1;
	}

	/*... Luego calcular PI con la cuenta dada... */
	pi_local = 4.0 * (double)n_dentro/(double)TIRADAS;

	/* Actualizar el resultado, suma de todos los cálculos... */
	MPI_Reduce(&pi_local, &pi_total, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
	if (proc_id != MASTER)
		printf("~ Se ha enviado el valor de PI: %g, del proceso: %d\n", pi_local, proc_id);

	MPI_Barrier(MPI_COMM_WORLD);

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

/* Obtiene la info del sistema y a imprime en pantalla */
void obtener_info_sist()
{
	int dont_care, mpi_subversion, mpi_version;
	struct utsname info;

	uname(&info);
	dont_care = MPI_Get_version(&mpi_version,&mpi_subversion);

	printf("## Arquitectura: \t%s\n",info.machine);
	printf("## Sistema: \t%s\n", info.sysname);
	printf("## Release : \t%s\n",info.release);
	printf("## Version : \t%s\n",info.version);
	printf("## MPI Version : \t%-d.%-d\n",mpi_version,mpi_subversion);
	printf("\n");
	return;
}
