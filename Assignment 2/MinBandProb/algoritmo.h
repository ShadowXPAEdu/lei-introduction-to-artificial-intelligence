#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_VERT 500

void gera_vizinho(int *a, int *b, int n, int vizinho);
int trepa_colinas(int *sol, int *mat, int vert, int num_iter, int vizinho);


// EVOLUTIVO

typedef struct
{
    // Tamanho da população
    int popsize;
    // Probabilidade de mutação
    float pm;
    // Probabilidade de recombinação
    float pr;
    // Tamanho do torneio para seleção do pai da próxima geração
	int tsize;
	// Constante para avaliação com penalização
	float ro;
	// Número de vertices (numGenes)
    int num_vert;
	// Número de arestas
	int num_arest;
	// Número de gerações
    int numGenerations;
} INFO;

typedef struct individual CHROM, *PCHROM;

struct individual
{
    // Solução (objetos que estão dentro da mochila)
    int p[MAX_VERT];
    // Valor da qualidade da solução
	float fitness;
    // 1 se for uma solução válida e 0 se não for
	int valido;
};

void tournament(PCHROM pop, INFO d, PCHROM parents);
void genetic_operators(PCHROM parents, INFO d, PCHROM offspring, int troca);
void recombinacao_dois_pontos_corte(PCHROM parents, INFO d, PCHROM offspring);
void recombinacao_dois_pontos_corte_2(PCHROM parents, INFO d, PCHROM offspring);
int pop_contains(PCHROM pop, int i, int val, int point1, int point2);
void mutacao_por_troca(PCHROM offspring, INFO d);
void mutacao_por_insercao(PCHROM offspring, INFO d);


// HÍBRIDO

void trepa_colinas_HIB(PCHROM pop, INFO d, int mat[][MAX_VERT], int num_iter, int vizinho);
