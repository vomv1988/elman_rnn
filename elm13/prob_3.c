/*
	prob_2.c
	1419700082

	Sean A y B secuencias binarias aleatorias,
	ambas de longitud n.

	En el programa, la probabilidad de que
	cualquier elemento Ai sea 1 es p(Ai == 1) = p1 %
	De forma semejante, p(Bi == 1) = p2 %

	El programa calcula la distribucion binomial
	de una secuencia C = A XOR B a lo largo de
	reps experimentos.

	<vomv1988@gmail.com>

	Compilar con
		$ gcc -lm -o prob_2 prob_2.c
	o
		$ gcc -o prob_2 -lm prob_2.c

	Ejecutar con
		$ ./prob_2 1000 5000 26 31 > dbin.dat
	o
		$ ./prob_2 300 10000 26 31 > dbin.dat
	o
		$ ./prob_2 3000 50000 26 31 > dbin.dat
	etc.

	Graficar con
		$ echo 'plot "dbin.dat" using 1 with lines, "dbin.dat" using 2 with lines' | gnuplot -p

	debug
	Sun Dec 28 20:16:10 CST 2014

	int p = (p1 * p2) + (100 - p1) * (100 - p2);
	p /= 100;
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define PI 3.14159265358979323846

int rp(int p);
int norm(int x, int mu, int sigma, int reps);
double normd(double x, double mu, double sigma2, double reps);

int main(int argc, char *argv[]){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	int reps = atoi(argv[2]);
	int p1 = atoi(argv[3]);
	int p2 = atoi(argv[4]);

	int p = (p1 * p2) + (100 - p1) * (100 - p2);
	p /= 100;
	int n = atoi(argv[1]);
	int mu = n * p;
	int sigma = sqrt(n * p * (100 - p));
//	int sigma2 = n * p * (100 - p);

	int i, j, m;

	int tmin = mu - 3 * sigma;
	tmin /= 100;
	int tmax = mu + 3 * sigma;
	tmax /= 100;

	printf("tmin %d, mu %d, tmax %d\n", tmin, mu / 100, tmax);

/*
	int *pd = (int *) malloc(sizeof(int) * n);
	for(i = 0; i < n; i ++)
		pd[i] = 0;

	for(i = 0; i < reps; i ++){
		m = 0;
		for(j = 0; j < n; j ++)
			if(rp(p1) == rp(p2))
				m ++;
		pd[m] ++;
	}

	for(i = 0; i < n; i ++)
		printf(
			"%d %d %f\n",
			pd[i],
			norm(i, mu, sigma, reps),
			normd((double) i, (double)  mu, (double)  sigma2, (double)  reps)
		);

	free(pd);
*/

	return 0;
}

int rp(int p){
	if(rand() % 100 < p)
		return 1;
	else
		return 0;
}

int norm(int x, int mu, int sigma, int reps){
	mu /= 100;
	sigma /= 100;
	return
		reps * exp(-pow(x - mu, 2) / (2 * pow(sigma, 2))) / (sigma * sqrt(2 * PI))
	;
}

double normd(double x, double mu, double sigma2, double reps){
	mu /= 100;
	double sigma = sqrt(sigma2 / 10000);
	return
		reps * exp(-pow(x - mu, 2) / (2 * pow(sigma, 2))) / (sigma * sqrt(2 * PI))
	;

}
