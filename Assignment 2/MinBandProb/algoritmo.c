#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

// Gera um vizinho � dist�ncia 1 (troca um v�rtice de cada grupo)
// Par�metros de entrada: Solu��o actual, a, Solu��o vizinha, b, N�mero de v�rtices, n
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
// Par�metros de entrada: Solucao, sol, Matriz de adjacencias, mat, N�mero de v�rtices, vert, N�mero de itera��es, num_iter
// Par�metros de sa�da: Custo da melhor solu��o encontrada
int trepa_colinas(int *sol, int *mat, int num_vert, int num_iter, int vizinho) {
	int *nova_sol, custo, custo_viz, i;

	nova_sol = malloc(sizeof(int) * num_vert);
	if (nova_sol == NULL) {
		printf("Erro na aloca��o de memoria.\n");
		exit(1);
	}
	// Avalia solu��o inicial
	custo = calcula_fit(sol, mat, num_vert);
	for(i = 0; i < num_iter; i++) {
		// Gera solu��o vizinha
		gera_vizinho(sol, nova_sol, num_vert, vizinho);
		// Avalia solu��o vizinha
		custo_viz = calcula_fit(nova_sol, mat, num_vert);
		// Fica com a solu��o vizinha se o custo diminuir (problema de minimiza��o) em rela��o � solu��o atual
		if (custo_viz <= custo) {
			substitui(sol, nova_sol, num_vert);
			custo = custo_viz;
		}
	}
	// Liberta a mem�ria usada para guardar a nova solu��o
	free(nova_sol);
	// Devolve o custo da melhor solu��o encontrada
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
		// Minimiza��o
		if (pop[x1].fitness < pop[x2].fitness)
			parents[i] = pop[x1];
		else
			parents[i] = pop[x2];
	}
}

// Operadores geneticos a usar na gera��o dos filhos
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
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

// Preenche o vector descendentes com o resultado da opera��o de recombina��o com dois pontos de corte
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
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
			// Copia do parent a partir de point2 at� num_vert
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
            // Volta ao inicio e copia do parent a partir de 0 at� point1
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

// Preenche o vector descendentes com o resultado da opera��o de recombina��o com dois pontos de corte
// Par�metros de entrada: estrutura com os pais (parents), estrutura com par�metros (d), estrutura que guardar� os descendentes (offspring)
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

// Muta��o por troca
// Par�metros de entrada: estrutura com os descendentes (offspring) e estrutura com par�metros (d)
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


// H�BRIDO

void trepa_colinas_HIB(PCHROM pop, INFO d, int mat[][MAX_VERT], int num_iter, int vizinho) {
	int i, j;
	CHROM v;

	for (i = 0; i < d.popsize; i++) {
		for (j = 0; j < num_iter; j++) {
            gera_vizinho(pop[i].p, v.p, d.num_vert, vizinho);
			v.fitness = eval_individual_2(v.p, d, mat, &v.valido);
            // Minimiza��o
			if (v.fitness <= pop[i].fitness)
				pop[i] = v;
		}
	}
}
