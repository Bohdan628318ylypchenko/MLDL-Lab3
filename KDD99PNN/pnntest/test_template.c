#include <stdio.h>
#include <time.h>

void test_template(int(*test)(), const char * test_name)
{
	printf("Test %s started\n", test_name);

	clock_t time_start = clock();
	int result = test();
	clock_t time_end = clock();
	double time = ((double)(time_end - time_start)) / CLOCKS_PER_SEC;

	if (result != 0)
		puts("Failed");
	else
		puts("Succeed.");

	printf("Test %s ended; time = %lf\n\n", test_name, time);
}
