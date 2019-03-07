#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PATTERN_MIN       5
#define PATTERN_MAX       100
#define PRODUCT_COST_MAX  100000L
#define ONE_TIME_COST_MAX 100000L
#define SAMPLE_NAME       "./sample"

#define INIT              0
#define FAIL              -1

static int gTestCnt = 0;

int get_arguments(char* wrPattern, int* wrPatternSize,
                 int* productCost, int* oneTimeCost)
{
	int err = 0;
	static FILE* fp = NULL;
	static int isInit = 1;
	char tmp[32];
	char* p = NULL;
	char* str = NULL;

	if (isInit) {
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
		
		gTestCnt = atoi(tmp);
		/* TODO: check constraints */

		isInit = 0;
	}

	if (gTestCnt <= 0) {
		err = -1;
		//printf("No test to go\n");
		fclose(fp);
		return err;
	}

	while (fgets(tmp, 32, fp) && (*tmp == 0 || *tmp == '\n')) {
	}

	//printf("%s", tmp);
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

	*wrPatternSize = atoi(str);
	if (*wrPatternSize < PATTERN_MIN || *wrPatternSize > PATTERN_MAX) {
		printf("Incorrect data len value %d\n", *wrPatternSize);
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

	*productCost = atoi(str);
	if (*productCost < 1 || *productCost > PRODUCT_COST_MAX) {
		printf("Incorrect production cost value %d\n", *productCost);
		fclose(fp);
		exit(1);
	}

	str = p + 1;
	*oneTimeCost = atoi(str);
	if (*oneTimeCost < 1 || *oneTimeCost > ONE_TIME_COST_MAX) {
		printf("Incorrect one-time cost value %d\n", *oneTimeCost);
		fclose(fp);
		exit(1);
	}
	if (!fgets(wrPattern, PATTERN_MAX + 1, fp)) {
		printf("Failed to read writing data\n");
		fclose(fp);
		exit(1);
	}
	//printf("%s", wrPattern);
	
	gTestCnt--;

	return err;
}

typedef struct node_s {
	char* p;
	int idx;
	int min_cost;
} node_t;


#define MAX_STICKS 50
#define MAX_COMPLETE MAX_STICKS / 2

struct complete_s {
	int stick_num;
	int available_sticks[MAX_STICKS];
	int pillar_size;
};

static struct complete_s g_complete[MAX_COMPLETE];
static int g_complete_cnt = 0;

int build_pillars(int available_sticks[], int stick_num, int pillar_size, int prev_diff)
{
	int i = 0, j = 0, k = 0;
	int min_diff = -1;

	for (i=0; i < stick_num; i++) {
		diff = available_sticks[i] - prev_diff;
		if (diff == 0) {
			g_complete_cnt++;
			g_complete[g_complete_cnt].pillar_size = 
				(pillar_size + available_sticks[i]) / 2;
			for (j = 0, k = 0; j < stick_num - 1; j++, k++) {
				if (k == i)
					k++;
				g_complete[g_complete_cnt].available_sticks[j] = available_sticks[k];
			}
			g_complete[g_complete_cnt].stick_num = j;
			continue;
		}

		if (min_diff == -1 || min_diff > diff) {
			min_diff = diff;
		}
	}

}



int max_pillar(void)
{
	int i = 0;
	int max = 0;
	for (i = 0; i <= g_complete_cnt; i++) {
		if (max < g_complete[i].pillar_size) {
			max = g_complete[i].pillar_size;
		}
	}
	return max;
}

int main()
{
	/* Arguments */
	char wrPattern[PATTERN_MAX + 1] = { 0 };
	int  wrPatternSize = PATTERN_MIN;
	int  productCost = 1;
	int  oneTimeCost = 1;
	/* variables */
	int testCnt = 1;
	int cost    = 0;

	while (!get_arguments(wrPattern, &wrPatternSize, &productCost, &oneTimeCost)) {
		cost = get_cost(wrPattern, wrPatternSize, productCost, oneTimeCost);
		printf("#%d %d\n", testCnt, cost);
		testCnt++;
	}

	return 0;
}

