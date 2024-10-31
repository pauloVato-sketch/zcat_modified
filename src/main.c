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
#define MAX_OBJ 1000
double mat[MAX_IND][MAX_OBJ];

struct INPUT_DATA{
	int n_ind;
	int n_obj;
	int n_var;
	short int zcat_num;
	short int level;
	bool bias_flag;
	bool complicated_flag;
	bool imbalance_flag;
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

ssize_t custom_getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || stream == NULL) {
        return -1; // Error: invalid arguments
    }

    if (*lineptr == NULL) {
        *n = 128; // Set initial size
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            return -1; // Error: memory allocation failed
        }
    }

    size_t pos = 0;
    int c;

    while (1) {
        c = fgetc(stream);
        if (c == EOF) {
            if (pos == 0) {
                return -1; // No input
            }
            break; // End of file reached
        }

        if (pos >= *n - 1) { // Need more space
            *n *= 2; // Double the size
            char *new_ptr = realloc(*lineptr, *n);
            if (new_ptr == NULL) {
                return -1; // Error: memory allocation failed
            }
            *lineptr = new_ptr;
        }

        (*lineptr)[pos++] = (char)c;

        if (c == '\n') {
            break; // End of line reached
        }
    }

    (*lineptr)[pos] = '\0'; // Null-terminate the string
    return pos; // Return the number of characters read
}

struct INPUT_DATA read_config_file(char* filename){
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
	read = custom_getline(&linha, &len, fp);
	// Linha de flags
	if (first_line_num == true && read != -1){
		// N_ind
		split = strtok(linha, " ");
		data.n_ind = atoi(split);
		// N_var
		split = strtok(NULL, " ");
		data.n_var = atoi(split);
		// N_obj
		split = strtok(NULL, " ");
		data.n_obj = atoi(split);
		// Level
		split = strtok(NULL, " ");
		data.level = atoi(split);
		// Bias
		split = strtok(NULL, " ");
		data.bias_flag = atoi(split);
		// Complicated
		split = strtok(NULL, " ");
		data.complicated_flag = atoi(split);
		// Imbalance
		split = strtok(NULL, " ");
		data.imbalance_flag = atoi(split);
		// Zcat_func
		split = strtok(NULL, " \n");
		data.zcat_num = atoi(split);

		printf("\nEntrada:===================\n");
		printf("Qtd individuos = %d, N variaveis decisão= %d, N objetivos = %d, Complicated_PS = %d\n", data.n_ind, data.n_var, data.n_obj, data.complicated_flag);
		printf("Level = %d, Bias = %d, Imbalance = %d, ZCATFunction Number = %d\n", data.level, data.bias_flag, data.imbalance_flag, data.zcat_num);
		first_line_num = false; 
	}
	bool begin = true;

	fclose(fp);

	return data;
}

// Passar NVar distinto e flags
// Escrever arquivo de saida
int main(int argc, char **argv){
	for(int a = 0; a < MAX_IND; a++){
		for(int b = 0; b < MAX_OBJ; b++){
			mat[a][b] = 0;
		}
	}

	printf("\n Nome do arquivo = %s\n", argv[1]);

	struct INPUT_DATA data = read_config_file(argv[1]);
	int i;
	printf("======================================================");
	printf("Data of config: \n");
	printf("	N_Obj: %d\n", data.n_obj);
	printf("	N_Var: %d\n", data.n_var);
	printf("	Level: %d\n", data.level);
	printf("	Bias: %d\n", data.bias_flag);
	printf("	Complicated: %d\n", data.complicated_flag);
	printf("	Imbalance: %d\n", data.imbalance_flag);

	printf("======================================================");
	int nobj = data.n_obj; /* number of objectives */
	int nvar = data.n_var; /* standard decision variables */

	int Level = data.level; /* Level of the problem {1,..,6} */
	int Bias_flag = data.bias_flag; /* Bias flag {0,1}  (True: 1, False: 0)*/
	int Complicated_PS_flag = data.complicated_flag; /* Complicated PS flag {0,1} (True: 1, False: 0) */
	int Imbalance_flag = data.imbalance_flag; /* Imbalance flag {0,1}  (True: 1, False: 0)*/

	double *LB = NULL, *UB = NULL; /* Pointer to the bounds of the problem */
	double *x, *f;	/* Decision variables (x) and objectives (f)*/

	FILE *pf = fopen("input.txt", "r");
	int max_solutions;
	int mop;
	char fname[1024];

	void (*test_problem)(double*, double*, double*, int *); /* test problem */
    
    /* seed for random numbers */
	srand(time(NULL));
		for( int jj =0; jj < data.n_ind; jj++){
		for(int ii=0; ii<data.n_var;ii++){
			if(fscanf(pf, "%lf", &mat[jj][ii] ) == EOF)
				exit(1);
		}
	}
	/* Configuring ZCAT benchmark. LB and UB are pointers to the bounds in the ZCAT structures */
	zcat_set(nvar, nobj, Level, Bias_flag, Complicated_PS_flag, Imbalance_flag, &LB, &UB);
    
    assert(LB!=NULL && UB!=NULL);
	x = (double*) malloc(sizeof(double) * nvar);
	f = (double*) malloc(sizeof(double) * nobj);

	printf("\nLB: ");
	print_double_vector(LB, nvar);
	printf("UB: ");
	print_double_vector(UB, nvar); printf("\n");

	FILE *output_fp = fopen("output.txt", "w+");

	switch (data.zcat_num)
	{
	case 1:{
		printf("CALLING ZCAT1\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat1], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT1(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	case 2:{
		printf("CALLING ZCAT2\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat2], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT2(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	case 3:
		{
		printf("CALLING ZCAT3\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat3], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT3(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 4:
		{
		printf("CALLING ZCAT4\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat4], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT4(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 5:
		{
		printf("CALLING ZCAT5\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat5], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT5(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 6:
		{
		printf("CALLING ZCAT6\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat6], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT6(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 7:
	{
		printf("CALLING ZCAT7\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat7], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT7(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 8:
		{
		printf("CALLING ZCAT8\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat8], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT8(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 9:
		{
		printf("CALLING ZCAT9\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat9], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT9(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 10:
		{
		printf("CALLING ZCAT10\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat10], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT10(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 11:
		{
		printf("CALLING ZCAT11\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat11], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT11(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 12:
		{
		printf("CALLING ZCAT12\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat12], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT12(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 13:
		{
		printf("CALLING ZCAT13\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat13], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT13(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	break;
	case 14:
{
		printf("CALLING ZCAT14\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat14], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT14(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	case 15:
{
		printf("CALLING ZCAT15\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat15], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT15(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	case 16:
{
		printf("CALLING ZCAT16\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat16], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT16(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	case 17:
{
		printf("CALLING ZCAT17\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat17], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT17(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}
	case 18:
{
		printf("CALLING ZCAT18\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat18], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT18(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	case 19:
{
		printf("CALLING ZCAT19\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat19], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT19(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	case 20:
{
		printf("CALLING ZCAT20\n");
		int index = 0;
		print_file_header(output_fp, ZCAT_MOP_STR[zcat20], nobj, nvar);

		/* Example 1: Generating and evaluating a single random solution */
		while(index < data.n_ind){
			for (i = 0; i < nvar; ++i){
				x[i] = mat[index][i];
			}
			ZCAT20(x, f, NULL, NULL);
			print_file_solution(output_fp, x, f, nobj, nvar);
			index++;
		}
		
		break;
		}	
	default:
		break;
	}
	fclose(pf);
	fclose(output_fp);

    

    return 0;
}