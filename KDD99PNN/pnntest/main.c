extern void test_template(int(*test)(), const char * test_name);
extern int test_pnn_predict();

int main()
{
	test_template(test_pnn_predict, "pnn_predict");

	return 0;
}