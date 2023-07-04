#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

// Gera um vizinho à distância 1 (troca um vértice de cada grupo)
// Parâmetros de entrada: Solução actual, a, Solução vizinha, b, Número de vértices, n
void gera_vizinho(int *a, int *b, int n, int vizinho) {
	int i, p1, p2, aux;

	for(i = 0; i < n; i++)
		b[i] = a[i];

	p1 = random_l_h(0, n - 1);
	do
		p2 = random_l_h(0, n - 1);
	while(p2 == p1);
	aux = b[p1];
	b[p1] = b[p2];
	b[p2] = aux;

	if (!vizinho) {
		int p3, p4;
		do
			p3 = random_l_h(0, n - 1);
		while (p3 == p2 || p3 == p1);
		do
			p4 = random_l_h(0, n - 1);
		while (p4 == p3);
		aux = b[p3];
		b[p3] = b[p4];
		b[p4] = aux;
	}
}

// Trepa colinas first-choice
// Parâmetros de entrada: Solucao, sol, Matriz de adjacencias, mat, Número de vértices, vert, Número de iterações, num_iter
// Parâmetros de saída: Custo da melhor solução encontrada
int trepa_colinas(int *sol, int *mat, int num_vert, int num_iter, int vizinho) {
	int *nova_sol, custo, custo_viz, i;

	nova_sol = malloc(sizeof(int) * num_vert);
	if (nova_sol == NULL) {
		printf("Erro na alocação de memoria.\n");
		exit(1);
	}
	// Avalia solução inicial
	custo = calcula_fit(sol, mat, num_vert);
	for(i = 0; i < num_iter; i++) {
		// Gera solução vizinha
		gera_vizinho(sol, nova_sol, num_vert, vizinho);
		// Avalia solução vizinha
		custo_viz = calcula_fit(nova_sol, mat, num_vert);
		// Fica com a solução vizinha se o custo diminuir (problema de minimização) em relação à solução atual
		if (custo_viz <= custo) {
			substitui(sol, nova_sol, num_vert);
			custo = custo_viz;
		}
	}
	// Liberta a memória usada para guardar a nova solução
	free(nova_sol);
	// Devolve o custo da melhor solução encontrada
	return custo;
}


// EVOLUTIVO

void tournament(PCHROM pop, INFO d, PCHROM parents) {
	int i, x1, x2;

	// Realiza popsize torneios
	for (i = 0; i < d.popsize; i++) {
		x1 = random_l_h(0, d.popsize - 1);
		do
			x2 = random_l_h(0, d.popsize - 1);
		while (x1 == x2);
		// Minimização
		if (pop[x1].fitness < pop[x2].fitness)
			parents[i] = pop[x1];
		else
			parents[i] = pop[x2];
	}
}

// Operadores geneticos a usar na geração dos filhos
// Parâmetros de entrada: estrutura com os pais (parents), estrutura com parâmetros (d), estrutura que guardará os descendentes (offspring)
void genetic_operators(PCHROM parents, INFO d, PCHROM offspring, int troca) {
	recombinacao_dois_pontos_corte_2(parents, d, offspring);
	if (troca) {
        mutacao_por_troca(offspring, d);
	} else {
        mutacao_por_insercao(offspring, d);
	}
}

int pop_contains(PCHROM pop, int i, int val, int point1, int point2) {
	for (int j = point1; j < point2; j++) {
		if (pop[i].p[j] == val) {
			return 1;
		}
	}
	return 0;
}

// Preenche o vector descendentes com o resultado da operação de recombinação com dois pontos de corte
// Parâmetros de entrada: estrutura com os pais (parents), estrutura com parâmetros (d), estrutura que guardará os descendentes (offspring)
void recombinacao_dois_pontos_corte_2(PCHROM parents, INFO d, PCHROM offspring) {
	int i, j, point1, point2, offset1, offset2;

	for (i = 0; i < d.popsize; i += 2) {
		if (rand_01() < d.pr) {
            offset1 = offset2 = 0;
			point1 = random_l_h(0, d.num_vert - 2);
			point2 = random_l_h(point1 + 1, d.num_vert - 1);
			// Copia do parent para offspring
			for (j = point1; j < point2; j++) {
				offspring[i].p[j] = parents[i].p[j];
				offspring[i + 1].p[j] = parents[i + 1].p[j];
			}
			// Copia do parent a partir de point2 até num_vert
			for (j = point2; j < d.num_vert; j++) {
				while (offset1 < d.num_vert) {
					if (pop_contains(offspring, i, parents[i + 1].p[offset1], point1, point2) != 1) {
						offspring[i].p[j] = parents[i + 1].p[offset1];
						offset1++;
						break;
					}
					offset1++;
				}
				while (offset2 < d.num_vert) {
					if (pop_contains(offspring, i + 1, parents[i].p[offset2], point1, point2) != 1) {
						offspring[i + 1].p[j] = parents[i].p[offset2];
						offset2++;
						break;
					}
					offset2++;
				}
			}
            // Volta ao inicio e copia do parent a partir de 0 até point1
			for (j = 0; j < point1; j++) {
				while (offset1 < d.num_vert) {
					if (pop_contains(offspring, i, parents[i + 1].p[offset1], point1, point2) != 1) {
						offspring[i].p[j] = parents[i + 1].p[offset1];
						offset1++;
						break;
					}
					offset1++;
				}
				while (offset2 < d.num_vert) {
					if (pop_contains(offspring, i + 1, parents[i].p[offset2], point1, point2) != 1) {
						offspring[i + 1].p[j] = parents[i].p[offset2];
						offset2++;
						break;
					}
					offset2++;
				}
			}
		} else {
			offspring[i] = parents[i];
			offspring[i + 1] = parents[i + 1];
		}
	}
}

// Preenche o vector descendentes com o resultado da operação de recombinação com dois pontos de corte
// Parâmetros de entrada: estrutura com os pais (parents), estrutura com parâmetros (d), estrutura que guardará os descendentes (offspring)
void recombinacao_dois_pontos_corte(PCHROM parents, INFO d, PCHROM offspring) {
	int i, j, point1, point2;

	for (i = 0; i < d.popsize; i += 2) {
		if (rand_01() < d.pr) {
			point1 = random_l_h(0, d.num_vert - 2);
			point2 = random_l_h(point1 + 1, d.num_vert - 1);
			for (j = 0; j < point1; j++) {
				offspring[i].p[j] = parents[i].p[j];
				offspring[i + 1].p[j] = parents[i + 1].p[j];
			}
			for (j = point1; j < point2; j++) {
				offspring[i].p[j] = parents[i + 1].p[j];
				offspring[i + 1].p[j] = parents[i].p[j];
			}
			for (j = point2; j < d.num_vert; j++) {
				offspring[i].p[j] = parents[i].p[j];
				offspring[i + 1].p[j] = parents[i + 1].p[j];
			}
		} else {
			offspring[i] = parents[i];
			offspring[i + 1] = parents[i + 1];
		}
	}
}

// Mutação por troca
// Parâmetros de entrada: estrutura com os descendentes (offspring) e estrutura com parâmetros (d)
void mutacao_por_troca(PCHROM offspring, INFO d) {
	int i, pos1, pos2, aux;

	for (i = 0; i < d.popsize; i++)
		if (rand_01() < d.pm) {
            pos1 = random_l_h(0, d.num_vert - 1);
			do
				pos2 = random_l_h(0, d.num_vert - 1);
			while (pos1 == pos2);
			aux = offspring[i].p[pos1];
			offspring[i].p[pos1] = offspring[i].p[pos2];
			offspring[i].p[pos2] = aux;
		}
}

void mutacao_por_insercao(PCHROM offspring, INFO d) {
	int i, j, pos1, pos2, aux;
    for (i = 0; i < d.popsize; i++) {
        if (rand_01() < d.pm) {
            pos1 = random_l_h(0, d.num_vert - 2);
            pos2 = random_l_h(pos1 + 1, d.num_vert - 1);
            aux = offspring[i].p[pos1];
            for (j = pos1; j < pos2; j++) {
                offspring[i].p[j] = offspring[i].p[j + 1];
            }
            offspring[i].p[pos2] = aux;
        }
    }
}


// HÍBRIDO

void trepa_colinas_HIB(PCHROM pop, INFO d, int mat[][MAX_VERT], int num_iter, int vizinho) {
	int i, j;
	CHROM v;

	for (i = 0; i < d.popsize; i++) {
		for (j = 0; j < num_iter; j++) {
            gera_vizinho(pop[i].p, v.p, d.num_vert, vizinho);
			v.fitness = eval_individual_2(v.p, d, mat, &v.valido);
            // Minimização
			if (v.fitness <= pop[i].fitness)
				pop[i] = v;
		}
	}
}
