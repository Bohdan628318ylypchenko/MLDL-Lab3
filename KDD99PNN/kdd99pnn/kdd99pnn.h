#pragma once

#include <stdio.h>

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

typedef struct pnn_net
{
	double sigma;
	int property_count;
	int total_reference_count;
	int class_count;
	struct pnn_class ** pnn_class_arr;
}pnn_net;

pnn_reference * pnn_reference_create(int id, int property_count, double * reference);
pnn_reference * pnn_reference_free(pnn_reference * obj);

pnn_class * pnn_class_create(char * class_name, int reference_count, pnn_reference ** reference_arr);
pnn_class * pnn_class_free(pnn_class * obj);

pnn_net * pnn_net_create(double sigma, int property_count, int total_reference_count, int class_count,
						 pnn_class ** pnn_class_arr);
pnn_net * pnn_net_free(pnn_net * obj);

pnn_net * pnn_net_load(FILE * f);
void * pnn_net_fprint(pnn_net * net, FILE * f);
