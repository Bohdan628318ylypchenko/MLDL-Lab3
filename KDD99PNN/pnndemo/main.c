#include "pnn.h"
#include "pnn_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define EOI '\n'
#define INIT_LEN 16
#define INCR_COEF 2
#define MIN_LEFTOVER 1

#define USAGE "Usage: p = pnn, t = train e = test\n[u]sage | [e]xit | [l]oad <p,t,e> inpath:str | [p]rint <p,t,e> outpath:str | [s]igma s:double | [r] input:'a1,a2,a3....,an' | [v]alidate | [t]rain"
#define COMMAND_PROMPT "Enter command:"

static char * finput_al(FILE * stream);

int main(void)
{
	pnn_data * pnn, * train, * test;
	pnn = train = test = NULL;
	errno_t error;
	char * command;
	puts(USAGE);
	while (1)
	{
		puts(COMMAND_PROMPT);
		command = finput_al(stdin);

		switch (command[0])
		{
			case 'l':
			{
				const char data_name = *(command + 2);
				const char * input_name = command + 4;
				FILE * input = NULL;
				error = fopen_s(&input, input_name, "rb");
				if (input == NULL)
				{
					fprintf(stderr, "Can't open file %s. Error = %d\n", input_name, error);
					break;
				}

				switch (data_name)
				{
					case 'p':
					{
						if (pnn != NULL)
							pnn = pnn_data_free(pnn);
						pnn = pnn_data_load(input);
					}
					break;
					case 't':
					{
						if (train != NULL)
							train = pnn_data_free(train);
						train = pnn_data_load(input);
					}
					break;
					case 'e':
					{
						if (test != NULL)
							test = pnn_data_free(test);
						test = pnn_data_load(input);
					}
					break;
				}

				fclose(input);
			}
			break;
			case 'p':
			{
				const char data_name = *(command + 2);
				const char * print_name = command + 4;
				FILE * print = NULL;
				error = fopen_s(&print, print_name, "w");
				if (print == NULL)
				{
					fprintf(stderr, "Can't open file %s. Error = %d\n", print_name, error);
					break;
				}

				switch (data_name)
				{
					case 'p':
					{
						pnn_data_fprint(pnn, print);
					}
					break;
					case 't':
					{
						pnn_data_fprint(train, print);
					}
					break;
					case 'e':
					{
						pnn_data_fprint(test, print);
					}
					break;
				}

				fclose(print);
			}
			break;
			case 's':
			{
				if (pnn != NULL)
					pnn->sigma = atof(command + 2);
			}
			break;
			case 'r':
			{
				if (pnn == NULL)
					break;

				double * input = (double *)calloc(pnn->property_count, sizeof(double));
				pnn_fail_alloc_check(input);

				char * args = command + 2;
				char * token, * next_token;
				token = strtok_s(args, ",", &next_token);
				for (int i = 0; i < pnn->property_count && token; i++)
				{
					input[i] = atof(token);
					token = strtok_s(NULL, ",", &next_token);
				}

				double start_time = omp_get_wtime();
				pnn_prediction * prediction_arr = pnn_predict(pnn, input);
				double end_time = omp_get_wtime();
				double time = end_time - start_time;
				for (int i = 0; i < pnn->class_count; i++)
					printf("%s : %.18lf\n", prediction_arr[i].class_name, prediction_arr[i].prediction);
				printf("Time: %lf\n", time);

				free(prediction_arr);
				free(input);
			}
			break;
			case 'e':
			{
				if (pnn != NULL)
					pnn = pnn_data_free(pnn);
				if (train != NULL)
					train = pnn_data_free(train);
				if (test != NULL)
					test = pnn_data_free(test);
				return 0;
			}
			break;
			case 'u':
			{
				puts(USAGE);
			}
			break;
			case 'v':
			{
				if (pnn == NULL || test == NULL)
				{
					printf("pnn = %p; ; test = %p; Please load all required data.\n",
						   pnn, test);
					break;
				}

				double time_start = omp_get_wtime();
				pnn_evaluation * eval_test = pnn_evaluate(pnn, test);
				double time_end = omp_get_wtime();
				int zero_count = 0;
				for (int i = 0; i < pnn->class_count; i++)
				{
					printf("Class: %s; eval: %.20lf\n",
						   pnn->pnn_class_arr[i]->class_name, eval_test[i].accuracy);
					if (eval_test[i].accuracy < EPSILON)
						zero_count++;
				}
				printf("zero count: %d\n", zero_count);
				printf("time: %lf\n", time_end - time_start);

				free(eval_test);
			}
			break;
			case 't':
			{
				if (pnn == NULL || train == NULL)
				{
					printf("pnn = %p; ; train = %p; Please load all required data.\n",
						   pnn, train);
					break;
				}

				double time_start = omp_get_wtime();
				pnn_evaluation * eval_train = pnn_evaluate(pnn, train);
				double time_end = omp_get_wtime();
				int zero_count = 0;
				for (int i = 0; i < pnn->class_count; i++)
				{
					printf("Class: %s; eval: %.20lf\n",
						   pnn->pnn_class_arr[i]->class_name, eval_train[i].accuracy);
					if (eval_train[i].accuracy < EPSILON)
						zero_count++;
				}
				printf("zero count: %d\n", zero_count);
				printf("time: %lf\n", time_end - time_start);

				free(eval_train);
			}
			break;
			default:
				puts(USAGE);
				break;
		}
		free(command);
	}
}

static char * finput_al(FILE * stream)
{
	size_t buff_len = INIT_LEN, char_count = 0;
	char * buff = (char *)malloc(buff_len * sizeof(char));
	for (char c; (c = getc(stream)) != EOI;)
	{
		if (ferror(stream))
		{
			fputs("Error while reading input from stream.", stderr);
			abort();
		}

		if (buff_len - char_count <= MIN_LEFTOVER)
		{
			buff_len *= INCR_COEF;
			buff = (char *)realloc(buff, buff_len * sizeof(char));
		}

		buff[char_count++] = c;
	}

	buff[char_count] = '\0';

	return buff;
}