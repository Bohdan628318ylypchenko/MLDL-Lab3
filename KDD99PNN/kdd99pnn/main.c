#include "kdd99pnn.h"

#include <stdio.h>

static const char * PNN_FILENAME = "pnn";
static const char * OUT_FILENAME = "print.txt";

int main(void)
{
	FILE * input, * output; errno_t error;
	error = fopen_s(&input, PNN_FILENAME, "rb");
	if (input == NULL)
	{
		fprintf(stderr, "Error: can't open input; errno = %d\n", error);
		return 1;
	}
	error = fopen_s(&output, OUT_FILENAME, "wb");
	if (output == NULL)
	{
		fprintf(stderr, "Error: can't open output; errno = %d\n", error);
		return 1;
	}

	pnn_net * net = pnn_net_load(input);
	pnn_net_fprint(net, output);
	net = pnn_net_free(net);

	fclose(input);
	fclose(output);
	
	return 0;
}