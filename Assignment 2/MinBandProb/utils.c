#include "algoritmo.h"
#include "utils.h"

void init_rand() {
	srand((unsigned)time(NULL));
}

// Leitura do ficheiro de input
// Parâmetros de entrada: Nome do ficheiro, nome, Número de vértices, num_vert
// Parâmetros de saída: Matriz de adjacências, mat_adj
int *init_dados(char *nome, int *num_vert) {
	FILE *f;
	int *mat_adj;
	int i, j, k, num_arest;
	char buffer[100];

	f = fopen(nome, "r");
	if (!f) {
		printf("Erro no acesso ao ficheiro dos dados\n");
		exit(1);
	}
	fscanf(f, "%[^\n]", buffer);
	fscanf(f, "%d %d %d", num_vert, num_vert, &num_arest);

	mat_adj = calloc((*num_vert) * (*num_vert), sizeof(int));
	if (!mat_adj) {
		printf("Erro na alocacao de memoria\n");
		exit(1);
	}

	for (i = 0; i < num_arest; i++) {
		fscanf(f, "%d %d", &j, &k);
		*(mat_adj + (*num_vert) * (j - 1) + k - 1) = 1;
		*(mat_adj + (*num_vert) * (k - 1) + j - 1) = 1;
	}
	fclose(f);
	return mat_adj;
}

// Gera a solução inicial
// Parâmetros de entrada: Solução, sol, Número de vertices, v
void gera_sol_inicial(int *sol, int num_vert) {
	int i, x, temp;

	for (i = 0; i < num_vert; i++) {
		sol[i] = i + 1;
	}

	for (i = 0; i < num_vert; i++) {
		x = random_l_h(0, num_vert - 1);

		temp = sol[x];
		sol[x] = sol[i];
		sol[i] = temp;
	}
}

// Escreve solução no ecrã
// Parâmetros de entrada: Solução, sol, Número de vertices, vert
// Alterar
void escreve_sol(int *sol, int num_vert) {
	printf("\n");
	for(int i = 0; i < num_vert; i++)
		printf("%d | ", sol[i]);
	printf("\n");
}

// Copia vector b para a (tamanho n)
// Parâmetros de entrada: Solução que será substituída, a, Solução que irá substituir, b, Número de vertices, n
void substitui(int *a, int *b, int n) {
	int i;
	for (i = 0; i < n; i++)
		a[i] = b[i];
}

int random_l_h(int min, int max) {
	return min + rand() % (max - min + 1);
}


// EVOLUTIVO


// Leitura dos parâmetros e dos dados do problema
// Parâmetros de entrada: Nome do ficheiro e matriz a preencher com os dados dos objectos (peso e valor)
// Parâmetros de saída: Devolve a estrutura com os parâmetros
INFO init_data(char *filename, int mat[][MAX_VERT]) {
	INFO x;
	FILE *f;
	int i, j, k;
	char str[100];

	f = fopen(filename, "rt");
	if (!f) {
		printf("File not found\n");
		exit(1);
	}

	// Leitura dos parâmetros do problema
	x.popsize = DEFAULT_POP;
	x.pm = DEFAULT_PM;
	x.pr = DEFAULT_PR;
	x.tsize = DEFAULT_TSIZE;
	x.numGenerations = DEFAULT_NUMGENER;
	x.ro = DEFAULT_RO;
	fscanf(f, "%[^\n]", str);
	fscanf(f, "%d %d %d", &x.num_vert, &x.num_vert, &x.num_arest);
	if (x.num_vert > MAX_VERT) {
		printf("Número de vertices é maior que MAX_VERT: %d\n", MAX_VERT);
		exit(1);
	}

	// Preenchimento da matriz
	for(i = 0; i < x.num_vert; i++)
		for(j = 0; j < x.num_vert; j++)
			mat[i][j] = 0;

	for(i = 0; i < x.num_arest; i++) {
		fscanf(f, "%d %d", &j, &k);
		mat[j - 1][k - 1] = 1;
		mat[k - 1][j - 1] = 1;
	}

	fclose(f);
	// Devolve a estrutura com os parâmetros
	return x;
}


// Criacao da populacao inicial. O vector e alocado dinamicamente
// Parâmetro de entrada: Estrutura com parâmetros do problema
// Parâmetro de saída: Preenche da estrutura da população apenas o vector binário com os elementos que estão dentro ou fora da mochila
PCHROM init_pop_2(INFO d) {
	int i;
	PCHROM indiv;

	indiv = malloc(sizeof(CHROM) * d.popsize);
	if (indiv == NULL) {
		printf("Erro na alocacao de memoria\n");
		exit(1);
	}

	for (i = 0; i < d.popsize; i++) {
        gera_sol_inicial(indiv[i].p, d.num_vert);
	}

	return indiv;
}


// Criacao da populacao inicial. O vector e alocado dinamicamente
// Parâmetro de entrada: Estrutura com parâmetros do problema
// Parâmetro de saída: Preenche da estrutura da população apenas o vector binário com os elementos que estão dentro ou fora da mochila
PCHROM init_pop(INFO d) {
	int i, j;
	PCHROM indiv;

	indiv = malloc(sizeof(CHROM) * d.popsize);
	if (indiv == NULL) {
		printf("Erro na alocacao de memoria\n");
		exit(1);
	}

	for (i = 0; i < d.popsize; i++) {
		for (j = 0; j < d.num_vert; j++)
			indiv[i].p[j] = flip();
	}
	return indiv;
}

// Simula o lançamento de uma moeda, retornando o valor 0 ou 1
int flip() {
	if ((((float)rand()) / RAND_MAX) < 0.5)
		return 0;
	else
		return 1;
}

// Actualiza a melhor solução encontrada
// Parâmetro de entrada: populacao actual (pop), estrutura com parâmetros (d) e a melhor solucao encontrada até a geraçãoo imediatamente anterior (best)
// Parâmetro de saída: a melhor solucao encontrada até a geração actual
CHROM get_best(PCHROM pop, INFO d, CHROM best) {
	int i;

	for (i = 0; i < d.popsize; i++) {
        // Minimização
		if (pop[i].fitness < best.fitness)
			best = pop[i];
	}
	return best;
}

// Devolve um valor real distribuido uniformemente entre 0 e 1
float rand_01() {
	return ((float)rand()) / RAND_MAX;
}

// Escreve uma solução na consola
// Parâmetro de entrada: populacao actual (pop) e estrutura com parâmetros (d)
void write_best(CHROM x, INFO d) {
	int i;

	printf("\nBest individual: %4.1f\n", x.fitness);
	for (i = 0; i < d.num_vert; i++)
		printf(" %d |", x.p[i]);
	putchar('\n');
}
