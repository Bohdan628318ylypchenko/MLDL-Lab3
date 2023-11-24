#include "pch.h"

#include "pnn.h"

#include "pnn_alloc_check.h"

#include <stdlib.h>

pnn_reference * pnn_reference_create(int id, int property_count, double * reference)
{
	pnn_reference * obj = (pnn_reference *)malloc(sizeof(pnn_reference));
	pnn_fail_alloc_check(obj);

	obj->id = id;
	obj->property_count = property_count;
	obj->reference = reference;

	return obj;
}

pnn_reference * pnn_reference_free(pnn_reference * obj)
{
	free(obj->reference);
	free(obj);

	return NULL;
}

pnn_class * pnn_class_create(char * class_name, int reference_count, pnn_reference ** reference_arr)
{
	pnn_class * obj = (pnn_class *)malloc(sizeof(pnn_class));
	pnn_fail_alloc_check(obj);

	obj->class_name = class_name;
	obj->reference_count = reference_count;
	obj->reference_arr = reference_arr;

	return obj;
}

pnn_class * pnn_class_free(pnn_class * obj)
{
	free(obj->class_name);
	for (int i = 0; i < obj->reference_count; i++)
		pnn_reference_free(obj->reference_arr[i]);
	free(obj);

	return NULL;
}

pnn_data * pnn_data_create(double sigma, int property_count, int total_reference_count, int class_count,
						 pnn_class ** pnn_class_arr)
{
	pnn_data * obj = (pnn_data *)malloc(sizeof(pnn_data));
	pnn_fail_alloc_check(obj);

	obj->sigma = sigma;
	obj->property_count = property_count;
	obj->total_reference_count = total_reference_count;
	obj->class_count = class_count;
	obj->pnn_class_arr = pnn_class_arr;

	return obj;
}

pnn_data * pnn_data_free(pnn_data * obj)
{
	for (int i = 0; i < obj->class_count; i++)
		pnn_class_free(obj->pnn_class_arr[i]);
	free(obj);

	return NULL;
}
