#pragma once

#include <stdio.h>

#define DEFAULT_SIGMA 0.01
#define EPSILON 0.001

typedef struct pnn_reference
{
	int id;
	int property_count;
	double * reference;
} pnn_reference;

typedef struct pnn_class
{
	char * class_name;
	int reference_count;
	struct pnn_reference ** reference_arr;
} pnn_class;

typedef struct pnn_data
{
	double sigma;
	int property_count;
	int total_reference_count;
	int class_count;
	struct pnn_class ** pnn_class_arr;
}pnn_data;

pnn_reference * pnn_reference_create(int id, int property_count, double * reference);
pnn_reference * pnn_reference_free(pnn_reference * obj);

pnn_class * pnn_class_create(char * class_name, int reference_count, pnn_reference ** reference_arr);
pnn_class * pnn_class_free(pnn_class * obj);

pnn_data * pnn_data_create(double sigma, int property_count, int total_reference_count, int class_count,
						 pnn_class ** pnn_class_arr);
pnn_data * pnn_data_free(pnn_data * obj);

pnn_data * pnn_data_load(FILE * f);
void pnn_data_fprint(pnn_data * data, FILE * f);

typedef struct
{
	char * class_name;
	double prediction;
}
pnn_prediction;

typedef struct
{
	char * class_name;
	double accuracy;
}
pnn_evaluation;

pnn_prediction * pnn_predict(pnn_data * data, double * input);
pnn_evaluation * pnn_evaluate(pnn_data * net, pnn_data * data);
