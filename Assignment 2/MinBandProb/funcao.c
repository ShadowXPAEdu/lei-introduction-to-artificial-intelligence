#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

int calcula_fit(int *sol, int *mat, int vert) {
	int ct, i, j, aux;
	ct = 0;
	for(i = 0; i < vert; i++) {
		for(j = 0; j < vert; j++) {
			if (*(mat + vert * i + j) == 1) {
				aux = abs(sol[j] - sol[i]);
				if (aux > ct)
					ct = aux;
			}
		}
	}
	return ct;
}


// EVOLUTIVO


// Avaliacao da população
// Parâmetros de entrada: populacao (pop), estrutura com parametros (d) e matriz com dados do problema (mat)
// Parâmetros de saída: Preenche pop com os valores de fitness e de validade para cada solução
void evaluate(PCHROM pop, INFO d, int mat[][MAX_VERT]) {
	int i;

	for (i = 0; i < d.popsize; i++)
		pop[i].fitness = eval_individual_2(pop[i].p, d, mat, &pop[i].valido);
}

float eval_individual_2(int sol[], INFO d, int mat[][MAX_VERT], int *v) {
    int ct, i, j, aux;
    ct = 0;
    for(i = 0; i < d.num_vert; i++) {
		for(j = 0; j < d.num_vert; j++) {
            if (mat[i][j] == 1) {
                aux = abs(sol[i] - sol[j]);
                if (aux > ct) {
                    ct = aux;
                }
            }
		}
	}
    (*v) = 1;
	return ct;
}

// Calcula a qualidade de uma solução
// Parâmetros de entrada: solução (sol), capacidade da mochila (d), matriz com dados do problema (mat) e numero de objectos (v)
// Parâmetros de saída: qualidade da solução (se a capacidade for excedida devolve 0)
float eval_individual(int sol[], INFO d, int mat[][MAX_VERT], int *v) {
	int i;
	float sum_weight, sum_profit;

	sum_weight = sum_profit = 0;
	// Percorre todos os objectos
	for (i = 0; i < d.num_vert; i++) {
		// Verifica se o objecto i esta na mochila
		if (sol[i] == 1) {
			// Actualiza o peso total
			sum_weight += mat[i][0];
			// Actualiza o lucro total
			sum_profit += mat[i][1];
		}
	}

	if (sum_weight > d.num_arest) {
		// Solução inválida
		*v = 0;
		return 0;
	} else {
		// Solução válida
		*v = 1;
		return sum_profit;
	}
}
