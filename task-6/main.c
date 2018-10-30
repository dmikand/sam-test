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

int GetMinCost(char* pattern, int size, int prevState, int wr)
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

	newPattern = pattern;
	//printf("pattern \"%s\"\n", pattern);

	if (prevState == INIT) {
		state = PatternEval(newPattern, wr, 0, 0);
		cost = GetMinCost(pattern + wr, size - wr, state, wr);
		if (cost < 0)
			return -1;
		//printf("%s: %c - %d\n", __FUNCTION__, *(newPattern + wr), cost);
		return cost + 1;
	}

	for (lap = 0; lap <= overLap; lap++) {
		/* can be optimized */
		state = PatternEval(pattern, wr, prevState, lap);
		if (state == FAIL) {
			continue;
		}
		newSize = size + lap;
		cost = GetMinCost(pattern + wr - lap, newSize - wr, state, wr);
		if (cost >= 0) {
			if (min == -2) {
				min = cost;
			}
			if (cost < min) {
				min = cost;
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

int GetCost(char* pattern, int pSize, int pCost, int oCost)
{
	int wrMax = 1;
	int wr = wrMax;
	int cost[PATTERN_MAX + 1] = {0};
	char bckPattern[PATTERN_MAX + 1] = {0};

	memcpy(bckPattern, pattern, pSize);

	wrMax = GetMaxWriteSize(pattern, pSize);

	for (wr = wrMax; wr >= 1; wr--) {
		//printf("wr = %d\n", wr);
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

