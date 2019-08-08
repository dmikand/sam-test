#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SAMPLE_NAME       "./sample"
#define WEIGHT_NUM_MAX 30

/* input arguments */
struct ctx_test_case {
	int weight_num;
	int weight_a;
	int weight_b;
	int weights[30];
};

struct ctx_test {
	int number;
	struct ctx_test_case cases[10];
} g_test;

int get_arguments()
{
	int err = 0;
	static FILE* fp = NULL;
	static int init_f = 1;
	char tmp[32];
	char* p = NULL;
	char* str = NULL;

	if (init_f) {
		/* open file */
		fp = fopen(SAMPLE_NAME, "r");
		if (fp == NULL)
		{
			printf("Error opening file\n");
			exit(1);
		}

		/* read the test count number */
		if (!fgets(tmp, sizeof(tmp), fp)) {
			fclose(fp);
			printf("Error reading test count number\n");
			exit(1);
		}
		
		g_test.number = atoi(tmp);
		/* TODO: check constraints */

		init_f = 0;
	}

	if (g_test.number <= 0) {
		err = -1;
		fclose(fp);
		return err;
	}

	int test_cnt;
	for (test_cnt = 0; test_cnt < g_test.number; test_cnt++) {
		while (fgets(tmp, 32, fp) && (*tmp == 0 || *tmp == '\n')) {
		}

		/* first line of arguments */
		int space = ' ';
		str = tmp;
		p = strchr(str, space);
		if (!p) {
			printf("Failed to parse line: %s\n", str);
			fclose(fp);
			exit(1);
		}

		*p = 0;

		g_test.cases[test_cnt].weight_a = atoi(str);
		if (g_test.cases[test_cnt].weight_a <= 0) {
			printf("Incorrect weight A value %s\n", str);
			fclose(fp);
			exit(1);
		}

		str = p + 1;
		p = strchr(str, space);
		if (!p) {
			printf("Failed to parse line: %s\n", str);
			fclose(fp);
			exit(1);
		}

		*p = 0;

		g_test.cases[test_cnt].weight_b = atoi(str);
		if (g_test.cases[test_cnt].weight_b <= 0) {
			printf("Incorrect weight B value %s\n", str);
			fclose(fp);
			exit(1);
		}

		str = p + 1;
		g_test.cases[test_cnt].weight_num = atoi(str);
		if (g_test.cases[test_cnt].weight_num <= 0
			&& g_test.cases[test_cnt].weight_num >= WEIGHT_NUM_MAX)
		{
			printf("Incorrect weight num value %s\n", str);
			fclose(fp);
			exit(1);
		}

		int weight_num;
		for (weight_num = 0; weight_num < g_test.cases[test_cnt].weight_num;
			weight_num++)
		{
			while (fgets(str, 32, fp) && (*str == 0 || *str == '\n')) {
			}
			g_test.cases[test_cnt].weights[weight_num] = atoi(str);
			if (g_test.cases[test_cnt].weights[weight_num] <= 0) {
				printf("Incorrect weight B value %s\n", str);
				fclose(fp);
				exit(1);
			}
		}
		
	}
	return 0;
}

int do_test(struct ctx_test_case* test)
{
	int target;
	int found = 0;
	for (target = test->weight_a; target <= test->weight_b; target++) {
		int weight_num;
		int left_weights[WEIGHT_NUM_MAX];
		for (weight_num = 1; weght_num < test->weight_num; weight_num++) {
			left_weights[weight_num - 1] = test->weights[weight_num - 1];
			for (lvl = 0; lvl <= weight_num; lvl++) {

				for (i = 0; i <= weight_num - 1; i++) {
					left_sum += 
				}
			}
			if (target == test->weights[weight_num]) {
				found = 1;;
				break;
			}
		}
	}
}

int main()
{
	int err = 0;
	int test_cnt;

	err = get_arguments();
	if (err) {
		return err;
	}

	for (test_cnt = 0; test_cnt < g_test.number; test_cnt++) {
		err = do_test(&g_test.cases[test_cnt]);
	}

	return 0;
}

