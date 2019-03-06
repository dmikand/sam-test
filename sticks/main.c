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

int GetMaxWriteSize(char* pattern, int size)
{
	int min = size;
	int cur = 0;
	int i = 0;
	char prev = INIT;

	for (i = 0; i <= size; i++) {
		cur++;

		if (pattern[i] == '*') {
			continue;
		}

		if (prev != pattern[i]) {
			if (prev == INIT) {
				prev = pattern[i];
				continue;
			}
			if (cur < min) 
				min = cur;

			prev = pattern[i];
			cur = 1;
		}

	}

	return min;
}

int PatternEval(char* pattern, int wr, int prevState, int pNum)
{
	int node = 0;
	int state = 0;
	int cur = 0;

	for (node = 0; node < wr; node++) {
		if (node + 1 <= pNum) {
			cur = prevState;
		} else {
			cur = pattern[node];
		}
		if (cur == '*')
			continue;
		if (state == 0) {
			state = cur;
			continue;
		}
		if (state != cur) {
			return FAIL;
		}
	}

	//printf("pattern: %s state: %c\n", pattern, state ? state : 'S');

	return state ? state : 'S';
}

int GetMinCost(char* pattern, int pattern_sz, int prev_state, int wr_size, int step)
{
	int cost = 1;
	int min = -2;
	int newSize = 0;
	int state, lap, i;
	int overLap = wr - 1;
	char* newPattern = NULL;

	if (!pattern) {
		//printf("%s: pattern is NULL\n", __FUNCTION__);
		return -100;
	}

	if (size < 1)
		return 0;

	//printf("pattern \"%s\"\n", pattern);

	if (prevState == INIT) {
		state = PatternEval(pattern, wr_size, 0, 0);
		for (int i = 0; i < wr_size; i++) {
			if (node_list[i] > step) {
				node_lit[i] = step + 1;
			}
		}

		cost = GetMinCost(pattern + wr, pattern_sz - wr_size, state, wr_size);
		if (cost < 0)
			return -1;
		//printf("%s: %c - %d\n", __FUNCTION__, *(newPattern + wr), cost);
		return cost + 1;
	}

	state = PatternEval(data, wr_size, 0, 0);
	node_lit[0] = cost;

	for (data_idx = wr_size; data_idx < data_sz; data_idx += wr_size) {
		tmp_cost = cost;
		/* try to write all variants with current data portion of write head size */
		for (lap = 0; lap <= overLap; lap++) {
			wr_idx = data_idx - lap;
#define KEY_LIST_LEN 3
			int wr_cell = 0;
			/* try to write full write head by write cells */
			for (key_id = 0; key_id < KEY_LIST_LEN; key_id++) {
				for (wr_cell = 0; wr_cell < wr_size; wr_cell++) {
					if ( wr_cell <= lap ) {
						if (data[wr_cell + wr_idx] != prev_state) {
							break;
						}
					} else {
						;
					}
				}
			}
			if (state == FAIL) {
				continue;
			}

			tmp_cost++;
			if (node_list[data_idx - lap].cost < cost + 1) {
				node_list[data_idx - lap].cost = cost + 1;
			} else {
				continue;
			}
		}
	}


	min++;
	//printf("%s: %c - %d\n", __FUNCTION__, *(newPattern + wr), cost);

	return min;
}

void SortArray(int* costArray, int size, int pCost, int oCost)
{
	int i = 0;
	int j = 0;

//for (i = 0; i < size + 15; i+=16) {
//	if (size - j >= 16) {
//		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
//				costArray[j + 0], costArray[j + 1], costArray[j + 2], costArray[j + 3], 
//				costArray[j + 4], costArray[j + 5], costArray[j + 6], costArray[j + 7], 
//				costArray[j + 8], costArray[j + 9], costArray[j + 10], costArray[j + 11], 
//				costArray[j + 12], costArray[j + 13], costArray[j + 14], costArray[j + 15]);
//		j+=16;
//	} else {
//		for (; j < size; j++) {
//			printf("%d\t", costArray[j]);
//		}
//		printf("\n");
//		break;
//	}
//}
#define GET_COST(_i) \
		((size - (_i))* pCost + costArray[(_i)]*oCost)


	int min = 0;
	for (i = 0; i < size; i++) {
		if (costArray[i] > -1) {
			min = GET_COST(i);
			j = i;
			break;
		}
	}

	for (; i < size; i++) {
		if ((costArray[i] != -1) && (min > GET_COST(i))) {
			min = GET_COST(i);
			j = i;
		}
	}

	costArray[0] = min;
	costArray[1] = j;

	return;
}

int GetArguments(char* wrPattern, int* wrPatternSize,
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

void build_graph(char* pattern, int pattern_sz, node_t node_list[], int wr_size)
{
	int node = 0;
	int state = 0;
	int cur = 0;

	for (node = 0; node < pattern_sz; node++) {
		if (node + 1 <= pNum) {
			cur = prevState;
		} else {
			cur = pattern[node];
		}
		if (cur == '*')
			continue;
		if (state == 0) {
			state = cur;
			continue;
		}
		if (state != cur) {
			return FAIL;
		}
	}

	//printf("pattern: %s state: %c\n", pattern, state ? state : 'S');

	return state ? state : 'S';
}

int GetCost(char* pattern, int pattern_sz, int prod_cost, int wr_cost)
{
	int wr_sz_max = 1;
	int wr_size = wr_sz_max;
	int cost[PATTERN_MAX + 1] = {0};
	int cost_min = wr_sz_max*prod_cost + (pattern_sz - wr_sz_max + 1)*wr_cost;
	Node node_list* = NULL;
	Node* node      = NULL;

	wr_sz_max = GetMaxWriteSize(pattern, pattern_sz);

	for (wr_size = wr_sz_max; wr_size >= 1; wr_size--) {
		//printf("wr = %d\n", wr);
		cost_min = wr_size*wr_cost + (pattern_sz - wr_size + 1)*wr_cost;
		p = wr;
		prevState = PatternEval(pattern, wr, 0, 0);
		for (child_cnt = 0; child_cnt < node->child_cnt; child_cnt++) {
			Node* child = 
		}
		cost[wrMax - wr] = GetMinCost(pattern, pSize, INIT, wr);
		memcpy(pattern, bckPattern, pSize);
	}

	SortArray(cost, wrMax, pCost, oCost);
	//printf("wr_min = %d, cost = %d\n", (wrMax - cost[1]), cost[0]);
	int ret = 0;
	ret = cost[0];
	
	return ret;
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

	while (!GetArguments(wrPattern, &wrPatternSize, &productCost, &oneTimeCost)) {
		cost = GetCost(wrPattern, wrPatternSize, productCost, oneTimeCost);
		printf("#%d %d\n", testCnt, cost);
		testCnt++;
	}

	return 0;
}

