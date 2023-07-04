#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"
//*
#define DEBUG
//*/
void menu_op_1_2(char *nome_fich, int runs, int num_iter, int vizinho);
void menu_op_3_4(char *nome_fich, int runs, int troca);
void menu_op_5_6_7_8(char *nome_fich, int runs, int num_iter, int vizinho, int troca);
void escreve_fich(char *nome_algoritmo, int runs, int num_iter, int vizinho, int troca, float media, int best_custo, int pop, int num_gener, float pr, float pm, char *nome_fich);

int main(int argc, char *argv[], char **envp) {
	char nome_fich[MAX_FICH];
	char *nome;
	int num_iter, runs, menu_op;
	//setlocale(LC_ALL, "Portuguese");
	setlocale(LC_ALL, "en_US");

	if (argc == 4) {
		strncpy(nome_fich, argv[1], MAX_FICH);
		runs = atoi(argv[2]);
		num_iter = atoi(argv[3]);
	} else if (argc == 3) {
		strncpy(nome_fich, argv[1], MAX_FICH);
		runs = atoi(argv[2]);
		num_iter = DEFAULT_ITER;
	} else if (argc == 2) {
		strncpy(nome_fich, argv[1], MAX_FICH);
		runs = DEFAULT_RUNS;
		num_iter = DEFAULT_ITER;
	} else {
		printf("Nome do ficheiro:\n > ");
		fgets(nome_fich, MAX_FICH, stdin);
		nome = strtok(nome_fich, "\n");
		snprintf(nome_fich, MAX_FICH, "%s", nome);
		runs = DEFAULT_RUNS;
		num_iter = DEFAULT_ITER;
	}

	if (runs <= 0) {
		runs = DEFAULT_RUNS;
		printf("Runs mudado para %d\n", DEFAULT_RUNS);
	}
	if (num_iter <= 0) {
		num_iter = DEFAULT_ITER;
		printf("Iterações mudado para %d\n", DEFAULT_ITER);
	}

	init_rand();

	do {
		printf("Menu\n");
		printf("1 - Trepa colinas normal.\n");
		printf("2 - Trepa colinas alterado.\n");
		printf("3 - Algoritmo evolutivo (Seleção por torneio, Crossover double-point, Mutação por troca).\n");
		printf("4 - Algoritmo evolutivo (Seleção por torneio, Crossover double-point, Mutação por inserção).\n");
		printf("5 - Algoritmo híbrido. 1 e 3\n");
		printf("6 - Algoritmo híbrido. 2 e 3\n");
		printf("7 - Algoritmo híbrido. 1 e 4\n");
		printf("8 - Algoritmo híbrido. 2 e 4\n");
		printf("0 - Sair.\n");

		scanf("%d", &menu_op);
	} while (menu_op < 0 || menu_op > 8);

	switch (menu_op) {
		case 1: {
			menu_op_1_2(nome_fich, runs, num_iter, TRUE);
			break;
		}
		case 2: {
			menu_op_1_2(nome_fich, runs, num_iter, FALSE);
			break;
		}
		case 3: {
			menu_op_3_4(nome_fich, runs, TRUE);
			break;
		}
		case 4: {
			menu_op_3_4(nome_fich, runs, FALSE);
			break;
		}
		case 5: {
			menu_op_5_6_7_8(nome_fich, runs, num_iter, TRUE, TRUE);
			break;
		}
		case 6: {
			menu_op_5_6_7_8(nome_fich, runs, num_iter, FALSE, TRUE);
			break;
		}
		case 7: {
			menu_op_5_6_7_8(nome_fich, runs, num_iter, TRUE, FALSE);
			break;
		}
		case 8: {
			menu_op_5_6_7_8(nome_fich, runs, num_iter, FALSE, FALSE);
			break;
		}
		case 0: {

			break;
		}
		default:
			break;
	}

	return 0;
}

void menu_op_1_2(char *nome_fich, int runs, int num_iter, int vizinho) {
	int custo, best_custo = 0, num_vert, k;
	int *grafo, *sol, *best;
	float mbf = 0.0f;

	grafo = init_dados(nome_fich, &num_vert);

	sol = calloc(num_vert, sizeof(int));
	best = calloc(num_vert, sizeof(int));
	if (sol == NULL || best == NULL) {
		printf("Erro na alocação de memoria.\n");
		exit(1);
	}

	for (k = 0; k < runs; k++) {
		gera_sol_inicial(sol, num_vert);
#ifdef DEBUG
		printf("\nRepeticao [%d]:\n", k + 1);
		printf("Solução inicial:");
		escreve_sol(sol, num_vert);
#endif
		custo = trepa_colinas(sol, grafo, num_vert, num_iter, vizinho);
#ifdef DEBUG
		escreve_sol(sol, num_vert);
		printf("Custo final: %d\n", custo);
#endif
		mbf += custo;

		if(k == 0 || best_custo > custo) {
			best_custo = custo;
			substitui(best, sol, num_vert);
		}
	}
#ifdef DEBUG
	printf("\n\nMBF: %f\n", mbf / k);

	printf("\nMelhor solucao encontrada:\n");
	escreve_sol(best, num_vert);
	printf("Custo final: %d\n", best_custo);
#endif
	free(grafo);
	free(sol);
	free(best);
	char buffer[MAX_FICH];
	if (vizinho)
		snprintf(buffer, MAX_FICH, "Trepa-colinas normal.");
	else
		snprintf(buffer, MAX_FICH, "Trepa-colinas vizinho alterado.");
	escreve_fich(buffer, runs, num_iter, vizinho, FALSE, mbf / k, best_custo, 0, 0, 0, 0, nome_fich);
}


// EVOLUTIVO


void menu_op_3_4(char *nome_fich, int runs, int troca) {
	INFO EA_param;
	int mat[MAX_VERT][MAX_VERT], r, gen_actual;
	float mbf = 0.0f;
	PCHROM pop = NULL, parents = NULL;
	CHROM best_run, best_ever;
	best_ever.fitness = 0;

	EA_param = init_data(nome_fich, mat);

	for (r = 0; r < runs; r++) {
#ifdef DEBUG
		printf("\nRepeticao [%d]:\n", r + 1);
#endif
		pop = init_pop_2(EA_param);
		evaluate(pop, EA_param, mat);
		best_run = pop[0];
		best_run = get_best(pop, EA_param, best_run);
		parents = malloc(sizeof(CHROM) * EA_param.popsize);
		if (parents == NULL) {
			printf("Erro na alocacao de memoria\n");
			exit(1);
		}
		gen_actual = 1;
		while (gen_actual <= EA_param.numGenerations) {
			tournament(pop, EA_param, parents);
			genetic_operators(parents, EA_param, pop, troca);
			evaluate(pop, EA_param, mat);
			best_run = get_best(pop, EA_param, best_run);
			gen_actual++;
		}
#ifdef DEBUG
		printf("\nTerm. Repeticao [%d]:", r + 1);
		write_best(best_run, EA_param);
		printf("\n");
#endif
		mbf += best_run.fitness;
		if (r == 0 || best_run.fitness < best_ever.fitness)
			best_ever = best_run;

		free(parents);
		free(pop);
	}
#ifdef DEBUG
	printf("\n\nMBF: %f\n", mbf / r);
	printf("\nMelhor solucao encontrada");
	write_best(best_ever, EA_param);
#endif
	char buffer[MAX_FICH];
	if (troca)
		snprintf(buffer, MAX_FICH, "Algoritmo evolutivo (Selecao por torneio; Crossover double-point; Mutacao por troca).");
	else
		snprintf(buffer, MAX_FICH, "Algoritmo evolutivo (Selecao por torneio; Crossover double-point; Mutacao por insercao).");
	escreve_fich(buffer, runs, 0, FALSE, troca, mbf / r, best_ever.fitness, DEFAULT_POP, DEFAULT_NUMGENER, DEFAULT_PR, DEFAULT_PM, nome_fich);
}


// HÍBRIDO


void menu_op_5_6_7_8(char *nome_fich, int runs, int num_iter, int vizinho, int troca) {
	INFO EA_param;
	int mat[MAX_VERT][MAX_VERT], r, gen_actual;
	float mbf = 0.0f;
	PCHROM pop = NULL, parents = NULL;
	CHROM best_run, best_ever;
	best_ever.fitness = 0;

	EA_param = init_data(nome_fich, mat);

	for (r = 0; r < runs; r++) {
#ifdef DEBUG
		printf("\nRepeticao [%d]:\n", r + 1);
#endif
		pop = init_pop_2(EA_param);
		evaluate(pop, EA_param, mat);
		trepa_colinas_HIB(pop, EA_param, mat, num_iter, vizinho);
		best_run = pop[0];
		best_run = get_best(pop, EA_param, best_run);
		parents = malloc(sizeof(CHROM) * EA_param.popsize);
		if (parents == NULL) {
			printf("Erro na alocacao de memoria\n");
			exit(1);
		}
		gen_actual = 1;
		while (gen_actual <= EA_param.numGenerations) {
			tournament(pop, EA_param, parents);
			genetic_operators(parents, EA_param, pop, troca);
			evaluate(pop, EA_param, mat);
			trepa_colinas_HIB(pop, EA_param, mat, num_iter, vizinho);
			best_run = get_best(pop, EA_param, best_run);
			gen_actual++;
		}
		trepa_colinas_HIB(pop, EA_param, mat, num_iter, vizinho);
#ifdef DEBUG
		printf("\nTerm. Repeticao [%d]:", r + 1);
		write_best(best_run, EA_param);
		printf("\n");
#endif
		mbf += best_run.fitness;
		if (r == 0 || best_run.fitness < best_ever.fitness)
			best_ever = best_run;

		free(parents);
		free(pop);
	}
#ifdef DEBUG
	printf("\n\nMBF: %f\n", mbf / r);
	printf("\nMelhor solucao encontrada");
	write_best(best_ever, EA_param);
#endif
	char buffer[MAX_FICH];
	if (troca && vizinho) {
		snprintf(buffer, MAX_FICH, "Algoritmo hibrido. 1 e 3");
	} else if (!vizinho && troca) {
		snprintf(buffer, MAX_FICH, "Algoritmo hibrido. 2 e 3");
	} else if (vizinho && !troca)
		snprintf(buffer, MAX_FICH, "Algoritmo hibrido. 1 e 4");
	else
		snprintf(buffer, MAX_FICH, "Algoritmo hibrido. 2 e 4");
	escreve_fich(buffer, runs, num_iter, vizinho, troca, mbf / r, best_ever.fitness, DEFAULT_POP, DEFAULT_NUMGENER, DEFAULT_PR, DEFAULT_PM, nome_fich);
}


// Escrever para ficheiro csv

void escreve_fich(char *nome_algoritmo, int runs, int num_iter, int vizinho, int troca, float media, int best_custo, int pop, int num_gener, float pr, float pm, char *nome_fich) {
	FILE *f;
	time_t unixT;
	char file_name[MAX_FICH];
	char *tf[2] = { "não", "sim" };

	unixT = time(NULL);

	snprintf(file_name, MAX_FICH, "%li%ccsv", unixT, '.');

	f = fopen(file_name, "w");
	if (f == NULL) {
		printf("Não foi possível salvar os dados para um ficheiro.\n");
		exit(1);
	}

    fprintf(f, "Algoritmo usado.\n");
	fprintf(f, "%s,%s\n", nome_algoritmo, nome_fich);
    fprintf(f, "Runs,Numero de Iteracoes,Vizinho Normal,Mutacao por Troca,Populacao,Numero de Geracoes,Probabilidade de Recombinacao,Probabilidade de Mutacao\n");
	fprintf(f, "%d,%d,%s,%s,%d,%d,%.2f,%.2f\n", runs, num_iter, tf[vizinho], tf[troca], pop, num_gener, pr, pm);
    fprintf(f, "Media,Melhor Resultado Encontrado\n");
	fprintf(f, "%.2f,%d", media, best_custo);

	fclose(f);
	printf("\nDados finais escritos para o ficheiro: %s\n", file_name);
}
