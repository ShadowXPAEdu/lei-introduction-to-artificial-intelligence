#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MAX_FICH 100

#define DEFAULT_TSIZE 2
#define DEFAULT_RO 0.0f


#define DEFAULT_RUNS 30

#define DEFAULT_ITER 100000

#define DEFAULT_POP 10
#define DEFAULT_NUMGENER 2500
#define DEFAULT_PR 0.3f
#define DEFAULT_PM 0.05f

void init_rand();
int *init_dados(char *nome, int *num_vert);
void gera_sol_inicial(int *sol, int num_vert);
void escreve_sol(int *sol, int num_vert);
void substitui(int *a, int *b, int n);
int random_l_h(int min, int max);


// EVOLUTIVO

INFO init_data(char *filename, int mat[][MAX_VERT]);
PCHROM init_pop(INFO d);
PCHROM init_pop_2(INFO d);
int flip();
CHROM get_best(PCHROM pop, INFO d, CHROM best);
float rand_01();
void write_best(CHROM x, INFO d);
