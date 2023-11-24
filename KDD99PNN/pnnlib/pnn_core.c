#include "pch.h"

#include "pnn.h"

#include "pnn_alloc_check.h"

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define MAX_SINGLE_THREAD 250

static double act(int property_count, pnn_reference * reference, double * input, double sigma);
static int index_of_largest_prediction(int n, pnn_prediction * prediction_arr);
static int are_class_names_equal(char * class_name1, char * class_name2);

pnn_prediction * pnn_predict(pnn_data * net, double * input)
{
	pnn_prediction * prediction_arr = (pnn_prediction *)malloc(net->class_count * sizeof(pnn_prediction));
	pnn_fail_alloc_check(prediction_arr);

	for (int i = 0; i < net->class_count; i++)
	{
		pnn_class * current_class = net->pnn_class_arr[i];

		prediction_arr[i].class_name = current_class->class_name;
		prediction_arr[i].prediction = 0;

		for (int j = 0; j < current_class->reference_count; j++)
		{
			prediction_arr[i].prediction += act(net->property_count,
												current_class->reference_arr[j], input, net->sigma);
		}
	}

	return prediction_arr;
}

pnn_evaluation * pnn_evaluate(pnn_data * net, pnn_data * data)
{
	pnn_evaluation * evaluation_arr = (pnn_evaluation *)malloc(data->class_count * sizeof(pnn_evaluation));
	pnn_fail_alloc_check(evaluation_arr);

	for (int i = 0; i < data->class_count; i++)
	{
		pnn_class * current_class = data->pnn_class_arr[i];
		pnn_evaluation * current_evaluation = &(evaluation_arr[i]);

		current_evaluation->class_name = current_class->class_name;
		current_evaluation->accuracy = 0;

		if (current_class->reference_count > MAX_SINGLE_THREAD)
		{
			double acc = 0;
			int j;
			#pragma omp parallel for reduction(+:acc)
			for (j = 0; j < current_class->reference_count; j++)
			{
				pnn_reference * current_reference = current_class->reference_arr[j];

				pnn_prediction * prediction_arr = pnn_predict(net, current_reference->reference);
				int k = index_of_largest_prediction(net->class_count, prediction_arr);
				
				if (are_class_names_equal(current_class->class_name, prediction_arr[k].class_name) == 0)
					acc += 1.0;
				
				free(prediction_arr);
			}

			current_evaluation->accuracy = acc / (double)(current_class->reference_count);
		}
		else
		{
			for (int j = 0; j < current_class->reference_count; j++)
			{
				pnn_reference * current_reference = current_class->reference_arr[j];

				pnn_prediction * prediction_arr = pnn_predict(net, current_reference->reference);
				int k = index_of_largest_prediction(net->class_count, prediction_arr);
				
				if (are_class_names_equal(current_class->class_name, prediction_arr[k].class_name) == 0)
					current_evaluation->accuracy += 1.0;
				
				free(prediction_arr);
			}

			current_evaluation->accuracy /= (double)(current_class->reference_count);
		}
	}

	return evaluation_arr;
}

static double act(int property_count, pnn_reference * reference, double * input, double sigma)
{
	double acc = 0, d;
	for (int i = 0; i < property_count; i++)
	{
		d = reference->reference[i] - input[i];
		acc += d * d;
	}
	acc /= -(sigma * sigma);

	return exp(acc);
}

static int index_of_largest_prediction(int n, pnn_prediction * prediction_arr)
{
	int i = 0;
	for (int j = 0; j < n; j++)
	{
		if (prediction_arr[i].prediction < prediction_arr[j].prediction)
			i = j;
	}
	return i;
}

static int are_class_names_equal(char * class_name1, char * class_name2)
{
	char * dash1 = strchr(class_name1, '-');
	char * dash2 = strchr(class_name2, '-');

	size_t n = dash1 - class_name1;
	if (n != dash2 - class_name2)
		return 1;

	return strncmp(class_name1, class_name2, n);
}
