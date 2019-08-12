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

int combinations_number(int n)
{
	int i, j, k;
	int n_fact = 1;
	int k_fact = 1;
	int nk_fact = 0;
	int res = 0;

	for  (i = 1; i <= n; i++) {
		n_fact *= i;
	}

	nk_fact = n_fact;
	for (k = 1; k < n; k++) {
		k_fact *= k;
		nk_fact /= (n - k);
		res += n_fact / (k_fact * nk_fact);
	}

	return res;
}

int do_test(struct ctx_test_case* test)
{
	int target;
	int found = 0;
	int* tbl = NULL;
	int  tbl_size = 0;
	int left_idx;
	int weight_num;
	int next_weight;

	tbl_size = combinations_number(test->weight_num);
	tbl = malloc(tbl_size * sizeof(int));

	if (tbl == NULL) {
		printf("failed to allocate %d units for table", tbl_size);
		return -1;
	}

	memset(tbl, sizeof(int) * tbl_size, -1);

	int lvl = 0;

	for (target = test->weight_a; target <= test->weight_b; target++) {
		for (weight_num = 0; weight_num < test->weight_num; weight_num++) {
			if (target == test->weights[weight_num]) {
				found = 1;;
				printf ("weight is found, left %d, right %d\n", 0, target);
				break;
			}
		}

		if (found) {
			break;
		}

		for (left_idx = 0; left_idx < test->weight_num; left_idx++) {
			for (weight_num = left_idx + 1;
				 weght_num < test->weight_num; weight_num++) 
			{
				if (target == test->weights[weight_num]) {
					found = 1;;
					break;
				}
			}
		}

		for (lvl = 0; lvl < test->weight_num - 1; lvl++) {
			for (weight_num = 0; weight_num < test->weight_num; weight_num++) {
				for (next_weight = weight_num + 1;
					next_weight < test->weight_num; next_weight++ )
				{
					int* unit = NULL;
					unit = &tbl[weight_num][lvl][next_weight - weight_num - 1];
					if (*unit != -1) {
						continue;
					}
					unit* = test->weights[weight_num] + test->weights[next_weight];
					
					if (target == test->weights[weight_num]) {
						found = 1;;
						printf ("weight is found, left %d, right %d\n", 0, target);
						break;
					}
				}

			}

			if (found) {
				break;
			}

			for (left_idx = 0; left_idx < test->weight_num; left_idx++) {
				for (weight_num = left_idx + 1;
					 weght_num < test->weight_num; weight_num++) 
				{
					if (target == test->weights[weight_num]) {
						found = 1;;
						break;
					}
				}
			}

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

