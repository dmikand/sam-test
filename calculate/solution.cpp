#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH	26
#define HEIGHT	99
#define LENGTH	200
const int THRESHOLD = 100000007;
#define ROWS       100
#define COLS       26
#define CLAUSE_MAX 25

struct value_s {
	int cnt;      /* clauses count */
	struct {
		int type; /* type of value: 0 - const, 1 - element */
		int idx;
		int sign; /* 0 - '+', 1 - '-' */
	} exp[CLAUSE_MAX];
};

struct cell_s {
	int link_table[ROWS*COLS];
	struct value_s value;
};

static struct cell_s table[ROWS*COLS];

inline int to_index(int row, int col)
{
	return (col*ROWS + row);
}

#define is_num(_x) ((_x) >= '0' && (_x) <= '9' ? 1 : 0)
#define is_letter(_x) ((_x) >= 'A' && (_x) <= 'Z' ? 1 : 0)
inline int extract_sign(char** s, int max)
{
	char* c   = *s;
	int   ret = 0;

	if (*c <= '-') {
		ret = !!(*c - '+');
		(*s)++;
	}

	return ret;
}

inline int extract_const(char** s, int max)
{
	char* c   = *s;
	char  num[6] = {0};
	int   ret = 0;
	int   i   = 0;

	for (i = 0; i < max && is_num(c[i]); ++i);
	memcpy(num, c, i - 1);
	ret = atoi(num);
	(*s) += i;

	return ret;
}

inline int extract_ref(char** s, int max)
{
	int col = 0;
	int row = 0;

	col = **s - 'A';
	(*s)++;

	row = extract_const(s, max - 1) + 1;

	return to_index(row, col);
}

/* return:
 * 0 - ok
 * < 0 - error
 */
int to_parse(struct cell_s* cell, char equation[LENGTH])
{
	int i = 0;
	int clause = 0;
	struct value_s* val = NULL;
	char* s = NULL;
	int max_offset = 0;

	printf("%s\n", equation);

	if (equation[i] == '=') {
		i++;
	}

	val = &cell->value;
	s = &equation[i];

	for (clause = 0; clause < CLAUSE_MAX; clause++) {
		max_offset = LENGTH - (s - equation);
		if (max_offset == 0 || *s == 0) {
			clause--;
			break;
		}
		val->exp[clause].sign = extract_sign(&s, max_offset);
		max_offset = LENGTH - (s - equation);

		if (is_num(*s)) {
			val->exp[clause].type = 0;
			val->exp[clause].idx = extract_const(&s, max_offset);
		} else if (is_letter(*s)) {
			val->exp[clause].type = 0;
			val->exp[clause].idx = extract_ref(&s, max_offset);
		} else {
			printf("Unknow string %s\n", s);
			return -1;
		}
	}

	val->cnt = clause + 1;

	return 0;
}

int compute_cell(int idx)
{
	int ret = 0;
	int i = 0;
	struct cell_s* cell = NULL;

	cell = table[idx];

	for (i = 0; i < cell->value.cnt; i++) {
		if (!cell->value.exp[i].type) {
			continue;
		}

		table[cell->value.exp[i]].link_table[idx] = 1;

		if (cell->link_table[cell->value.exp[i]]) {
			undetermined = 1;
		}

	}
}
void initTable()
{
	memset(table, 0, sizeof(*table)*ROWS*COLS);
	return;
}

bool updateCell(int row, int col, char equation[LENGTH], int value[HEIGHT][WIDTH])
{
	bool ret = false;

	row++;
	ret = to_parse(&table[to_index(row, col)], equation);

	return ret;
}
