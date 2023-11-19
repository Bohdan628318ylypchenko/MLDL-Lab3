#include "kdd99pnn.h"

#include "kdd99pnn_alloc_check.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SIGMA 0.01

pnn_net * pnn_net_load(FILE * f)
{
	int property_count;
	int total_reference_count;
	int class_count;

	fread(&property_count, sizeof(int), 1, f);
	fread(&total_reference_count, sizeof(int), 1, f);
	fread(&class_count, sizeof(int), 1, f);

	pnn_class ** pnn_class_arr = (pnn_class **)malloc(class_count * sizeof(pnn_class *));
	pnn_fail_alloc_check(pnn_class_arr);
	for (struct
	     {
		     int i;
			 size_t class_name_len; char * class_name;
			 int reference_count;
	     }
	     state_class = { .i = 0, .class_name_len = 0, .class_name = NULL, .reference_count = 0 };
		 state_class.i < class_count;
		 state_class.i++)
	{
		fread(&(state_class.class_name_len), sizeof(size_t), 1, f);

		state_class.class_name = (char *)malloc((state_class.class_name_len + 1) * sizeof(char));
		pnn_fail_alloc_check(state_class.class_name);
		fread(state_class.class_name, sizeof(char), state_class.class_name_len, f);
		state_class.class_name[state_class.class_name_len] = '\0';

		fread(&(state_class.reference_count), sizeof(int), 1, f);

		pnn_reference ** pnn_reference_arr =
			(pnn_reference **)malloc(state_class.reference_count * sizeof(pnn_reference *));
		pnn_fail_alloc_check(pnn_reference_arr);
		for (struct
		     {
			     int j;
				 int reference_id; int property_count; 
				 double * reference;
		     }
		     state_reference = { .j = 0, .reference_id = 0, .property_count = 0, .reference = NULL};
		     state_reference.j < state_class.reference_count;
			 state_reference.j++)
		{
			fread(&(state_reference.reference_id), sizeof(int), 1, f);
			fread(&(state_reference.property_count), sizeof(int), 1, f);

			state_reference.reference = (double *)malloc(state_reference.property_count * sizeof(double));
			pnn_fail_alloc_check(state_reference.reference);
			fread(state_reference.reference, sizeof(double), state_reference.property_count, f);

			pnn_reference_arr[state_reference.j] = pnn_reference_create(state_reference.reference_id, state_reference.property_count,
																		state_reference.reference);
		}

		pnn_class_arr[state_class.i] = pnn_class_create(state_class.class_name, state_class.reference_count, pnn_reference_arr);
	}

	pnn_net * net = pnn_net_create(DEFAULT_SIGMA, property_count, total_reference_count, class_count,
								   pnn_class_arr);

	return net;
}

void * pnn_net_fprint(pnn_net * net, FILE * f)
{
	fprintf(f, "sigma = %lf; property_count = %d; total_reference_count = %d; class_count = %d\n",
			net->sigma, net->property_count, net->total_reference_count, net->class_count);

	for (struct
	     {
		     int i;
			 pnn_class * current_class;
	     }
	     state_class = { .i = 0, .current_class = NULL };
	     state_class.i < net->class_count; state_class.i++)
	{
		state_class.current_class = net->pnn_class_arr[state_class.i];

		fprintf(f, "    class_name = %s; reference_count = %d\n",
				state_class.current_class->class_name, state_class.current_class->reference_count);
		
		for (struct
		     {
		     	int j;
		     	pnn_reference * current_reference;
		     }
		     state_reference = { .j = 0, .current_reference = NULL };
			 state_reference.j < state_class.current_class->reference_count; state_reference.j++)
		{
			state_reference.current_reference = state_class.current_class->reference_arr[state_reference.j];

			fprintf(f, "        id = %d; property_count = %d;",
					state_reference.current_reference->id, state_reference.current_reference->property_count);

			for (int k = 0; k < state_reference.current_reference->property_count; k++)
			{
				fprintf(f, " %.2lf", state_reference.current_reference->reference[k]);
			}
			fputc('\n', f);
		}
	}
}
