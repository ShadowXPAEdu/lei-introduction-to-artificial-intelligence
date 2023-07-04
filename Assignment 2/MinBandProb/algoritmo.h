#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_VERT 500

void gera_vizinho(int *a, int *b, int n, int vizinho);
int trepa_colinas(int *sol, int *mat, int vert, int num_iter, int vizinho);


// EVOLUTIVO

typedef struct
{
    // Tamanho da popula��o
    int popsize;
    // Probabilidade de muta��o
    float pm;
    // Probabilidade de recombina��o
    float pr;
    // Tamanho do torneio para sele��o do pai da pr�xima gera��o
	int tsize;
	// Constante para avalia��o com penaliza��o
	float ro;
	// N�mero de vertices (numGenes)
    int num_vert;
	// N�mero de arestas
	int num_arest;
	// N�mero de gera��es
    int numGenerations;
} INFO;

typedef struct individual CHROM, *PCHROM;

struct individual
{
    // Solu��o (objetos que est�o dentro da mochila)
    int p[MAX_VERT];
    // Valor da qualidade da solu��o
	float fitness;
    // 1 se for uma solu��o v�lida e 0 se n�o for
	int valido;
};

void tournament(PCHROM pop, INFO d, PCHROM parents);
void genetic_operators(PCHROM parents, INFO d, PCHROM offspring, int troca);
void recombinacao_dois_pontos_corte(PCHROM parents, INFO d, PCHROM offspring);
void recombinacao_dois_pontos_corte_2(PCHROM parents, INFO d, PCHROM offspring);
int pop_contains(PCHROM pop, int i, int val, int point1, int point2);
void mutacao_por_troca(PCHROM offspring, INFO d);
void mutacao_por_insercao(PCHROM offspring, INFO d);


// H�BRIDO

void trepa_colinas_HIB(PCHROM pop, INFO d, int mat[][MAX_VERT], int num_iter, int vizinho);
