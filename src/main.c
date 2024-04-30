/*
 * main.c
 *
 *  Created on: Apr 1, 2022
 *      Author: Saul Zapotecas
 *  Updated on: Apr 27, 2024
 *      Author: Paulo do Nascimento
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#include "zcat-tools.h"
#include "zcat-benchmark.h"
#include "zcat-rnd-opt-sol.h"
#include <stdbool.h>
#define MAX_IND 1000
#define MAX_OBJ 15
double mat[MAX_IND][MAX_OBJ];
struct INPUT_DATA{
	int n_ind;
	int n_obj;
	short int zcat_num;
};
void print_file_header(FILE *file, char *mop, int nobj, int nvar)
{
	int j;
	fprintf(file, "# %s: %d objectives, %d decision variables\n# ", mop, nobj,
			nvar);
	for (j = 0; j < nobj; ++j)
		fprintf(file, "<obj%d> ", j + 1);
	for (j = 0; j < nvar; ++j)
		fprintf(file, "<var%d> ", j + 1);
	fprintf(file, "\n");
	return;
}

void print_file_solution(FILE *file, double *x, double *f, int nobj, int nvar)
{
	int j;
	for (j = 0; j < nobj; ++j)
		fprintf(file, "%e ", f[j]);
	for (j = 0; j < nvar; ++j)
		fprintf(file, "%e ", x[j]);
	fprintf(file, "\n");
	return;
}
/* Estrutura do arquivo
num_individuos=linha num_obj=colunas func_zcat_num
...
*/
struct INPUT_DATA read_file(char* filename){
	FILE *fp;
	char* linha = NULL;
	char* split = NULL;
	size_t len = 0;
	ssize_t read;
	bool first_line_num = true;
	struct INPUT_DATA data;
	int index=0, obj=0;
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Leitura do arquivo falhou!");
		exit(1);
	}
	read = getline(&linha, &len, fp);
	if (first_line_num == true && read != -1){
		split = strtok(linha, " ");
		data.n_ind = atoi(split);
		split = strtok(NULL, " ");
		data.n_obj = atoi(split);
		split = strtok(NULL, " \n");
		data.zcat_num = atoi(split);

		printf("N individuos = %d, N objetivos = %d, funcao %d\n", data.n_ind, data.n_obj, data.zcat_num);
		first_line_num = false; 
	}
	bool begin = true;
	for( int jj =0; jj < data.n_ind; jj++){
		for(int ii=0; ii<data.n_obj;ii++){
			if(fscanf(fp, "%lf", &mat[jj][ii] ) == EOF)
				exit(1);
		}
	}
	fclose(fp);
	for( int jj =0; jj < data.n_ind; jj++){
		for(int ii=0; ii<data.n_obj;ii++){
			printf("%.10lf",mat[jj][ii]);
		}
		printf("\n");
	}
	return data;
}

int main(int argc, char **argv){
	for(int a = 0; a < MAX_INPUT; a++){
		for(int b = 0; b < MAX_OBJ; b++){
			mat[a][b] = 0;
		}
	}
	printf("\n Nome do arquivo = %s\n", argv[1]);
	struct INPUT_DATA data = read_file(argv[1]);
	int i;
	int nobj = data.n_obj; /* number of objectives */
	int nvar = nobj; /* standard decision variables */

	int Level = 1; /* Level of the problem {1,..,6} */
	int Bias_flag = 0; /* Bias flag {0,1}  (True: 1, False: 0)*/
	int Complicated_PS_flag = 1; /* Complicated PS flag {0,1} (True: 1, False: 0) */
	int Imbalance_flag = 0; /* Imbalance flag {0,1}  (True: 1, False: 0)*/

	double *LB = NULL, *UB = NULL; /* Pointer to the bounds of the problem */
	double *x, *f;	/* Decision variables (x) and objectives (f)*/

	FILE *pf;
	int max_solutions;
	int mop;
	char fname[1024];

	void (*test_problem)(double*, double*, double*, int *); /* test problem */
    
    /* seed for random numbers */
	srand(time(NULL));

	/* Configuring ZCAT benchmark. LB and UB are pointers to the bounds in the ZCAT structures */
	zcat_set(nvar, nobj, Level, Bias_flag, Complicated_PS_flag, Imbalance_flag, &LB, &UB);
    
    assert(LB!=NULL && UB!=NULL);
	x = (double*) malloc(sizeof(double) * nvar);
	f = (double*) malloc(sizeof(double) * nobj);

	printf("\nLB: ");
	print_double_vector(LB, nvar);
	printf("UB: ");
	print_double_vector(UB, nvar); printf("\n");
	int index = 0;
	/* Example 1: Generating and evaluating a single random solution */
	for (i = 0; i < nvar; ++i) /* Generating random solution */
	{
		x[i] = mat[index][i];
	}
	switch (data.zcat_num)
	{
	case 1:
		printf("CALLING ZCAT1\n");ZCAT1(x, f, NULL, NULL); /* Evaluating random solution */
		break;
	case 2:
		printf("CALL ZCAT2\n");ZCAT2(x, f, NULL, NULL); /* Evaluating random solution */
		break;
	case 3:
		ZCAT3(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 4:
		ZCAT4(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 5:
		ZCAT5(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 6:
		ZCAT6(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 7:
		ZCAT7(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 8:
		ZCAT8(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 9:
		ZCAT9(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 10:
		ZCAT10(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 11:
		ZCAT11(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 12:
		ZCAT12(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 13:
		ZCAT13(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 14:
		ZCAT14(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 15:
		ZCAT15(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 16:
		ZCAT16(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 17:
		ZCAT17(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 18:
		ZCAT18(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 19:
		ZCAT19(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	case 20:
		ZCAT20(x, f, NULL, NULL); /* Evaluating random solution */
	break;
	default:
		break;
	}
	
	printf("===================\n");
	printf("Example 0\n");
	printf("===================\n");
	printf("Decision variables: \n");
	print_double_vector(x, nvar);
	printf("Objective values: \n");
	print_double_vector(f, nobj);
	printf("===================\n");
    
    return 0;
}