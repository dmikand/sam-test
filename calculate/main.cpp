#include <stdio.h>

#define WIDTH	26
#define HEIGHT	99
#define LENGTH	200
const int THRESHOLD = 100000007;
static int value[HEIGHT][WIDTH];

extern void initTable();
extern bool updateCell(int row, int col, char equation[LENGTH], int value[HEIGHT][WIDTH]);

int main_init(){
	for(int i = 0; i < HEIGHT; ++i){
		for(int j = 0; j < WIDTH; ++j){
			value[i][j] = 0;
		}
	}
	int cmd = 0;
	scanf("%d", &cmd);
	return cmd;
}

int calcChecksum(int value[HEIGHT][WIDTH], bool ret){
	int sum = 0;
	for(int i = 0; i < HEIGHT; ++i){
		for(int j = 0; j < WIDTH; ++j){
			sum += value[i][j];
			sum %= THRESHOLD;
		}
	}
	if(!ret) ++sum;
	sum %= THRESHOLD;
	if(sum < 0) sum += THRESHOLD;
	return sum;
}

int main(){
	setbuf(stdout, NULL);
	freopen("sample_input.txt", "r", stdin);
	
	int T;
    int totalScore = 0;
	scanf("%d", &T);
	for(int tc = 1; tc <= T; ++tc){
		initTable();
		int row, col;
		char input[LENGTH];
		int checksumIn = 0;
		int cmd = main_init();
		int score = 0;
		
		for(int i = 0; i < cmd; ++i){
			scanf("%d %d %s %d", &row, &col, input, &checksumIn);
			if(checksumIn < 0) checksumIn += THRESHOLD;
			bool ret = updateCell(row, col, input, value);
			int checksum = calcChecksum(value, ret);
			if(checksumIn == checksum) ++score;
		}	
		printf("#%d %d\n", tc, score);
		totalScore += score;
	}
	printf("TotalScore = %d\n", totalScore);

	return 0;
}
