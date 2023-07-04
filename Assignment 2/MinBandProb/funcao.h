
int calcula_fit(int *sol, int *mat, int vert);


// EVOLUTIVO

void evaluate(PCHROM pop, INFO d, int mat[][MAX_VERT]);
float eval_individual(int sol[], INFO d, int mat[][MAX_VERT], int *v);
float eval_individual_2(int sol[], INFO d, int mat[][MAX_VERT], int *v);
