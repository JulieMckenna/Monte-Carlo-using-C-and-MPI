#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

int main (int argc, char *argv[])
{
        int numtasks, numworkers, numperworkers, remainder;
        int taskid, source, dest, mtype;
        int i, a, b, N;
        double sum, finalsum, finalcalc, calc, random;
        double starttime, endtime, totaltime;

        MPI_Status status;
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

        numworkers = numtasks-1;

        if (taskid == MASTER)
        {
                starttime = MPI_Wtime();
                a = atoi(argv[1]);
                b = atoi(argv[2]);
                N = atoi(argv[3]);
                //printf("Enter a value for a: ");
                //scanf("%d", &a);
                //printf("Enter a value for b: ");
                //scanf("%d", &b);
                //printf("Enter a value for N: ");
                //scanf("%d", &N);
                //printf("The values of a=%d, b=%d, N=%d\n", a, b, N);
                //printf("The value of (b-a)/N = %lf\n", (double)((b-a)/(double)N));
                finalsum = 0.0;
                numperworkers = N/numworkers;
                if (N % numworkers != 0)
                        remainder = N % numworkers;

                //sending to workers
                mtype= FROM_MASTER;
                for (dest = 1; dest <= numworkers; dest++)
                {
					//printf("Sending values to the workers\n");
                    MPI_Send(&a, 1, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
                    MPI_Send(&b, 1, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
                    MPI_Send(&N, 1, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
                    MPI_Send(&numperworkers, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
                    MPI_Send(&remainder, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
                }

                //recieve from worker
                mtype = FROM_WORKER;
                for (i = 1; i <= numworkers; i++)
                {
                        source = i;
                        MPI_Recv(&sum, 1, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
                        finalsum += sum;
                }
                printf("The final sum = %lf\n", finalsum);
                double abn = (double)((b-a)/(double)N);
                finalcalc = (finalsum)*(abn);
                printf("The convergence is = %lf\n", finalcalc);
                endtime = MPI_Wtime();
                totaltime = endtime - starttime;
                printf("The total time = %lf\n", totaltime);
        }

        if (taskid > MASTER)
        {

                //receive values
                mtype = FROM_MASTER;
                MPI_Recv(&a, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
                MPI_Recv(&b, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
                MPI_Recv(&N, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
                MPI_Recv(&numperworkers, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
                MPI_Recv(&remainder, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

                sum = 0.0;
                srand(time(0) + taskid);
                int times;
                if (taskid <= remainder)
                        times = numperworkers + 1;
                else
                        times = numperworkers;

                for (i = 0; i < times; i++)
                {
                        random = ((rand() / (double)RAND_MAX) * (b-a)) + a;
                        calc = (1.0/(pow((2*3.14), 0.5)))*(exp((-1)*(random*random)/2));
                        //printf("The calculation at random number: %lf = %lf\n", random, calc);
                        sum += calc;
                }
                printf("Sum from worker %d= %lf\n", taskid, sum);
                mtype = FROM_WORKER;
                MPI_Send(&sum, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
        }
        MPI_Finalize();
        return 0;
}
