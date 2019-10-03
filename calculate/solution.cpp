#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH	26
#define HEIGHT	99
#define LENGTH	200
#define ROWS       100
#define COLS       26
#define CLAUSE_MAX 25

const int THRESHOLD = 100000007;

struct clause_s {
	int type; /* type of value: 0 - const, 1 - element */
	int idx;
	int sign; /* 0 - '+', 1 - '-' */
};

struct value_s {
	int calculated;
	int val;
	int cnt;      /* clauses count */
	struct clause_s exp[CLAUSE_MAX];
};

struct cell_s {
	int is_visited;
	int link_table[ROWS*COLS];
	struct value_s value;
};

struct s_entry {
	struct cell_s* cell;
	int idx;
};

static struct cell_s table[ROWS*COLS];
static int undetermined_table[ROWS*COLS];

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

#define PUSH(_cell, _idx)                                       \
{                                                               \
	if(stack_ptr < stack) {printf("Stack overflow\n"); exit(1);}\
	stack_ptr->cell = (_cell);                                  \
	stack_ptr->idx =  (_idx);                                   \
}

#define POP(_cell, _idx)                       \
(                                              \
	++stack_ptr,                               \
	(_cell) = stack_ptr->cell,                 \
	(_idx) = stack_ptr->idx,                   \
	stack_ptr > &stack[ROWS*COLS - 1] ? 1 : 0  \
)

#define UNDETERMINED 1

static inline int calculate_cell(int idx)
{
	struct clause_s* exp    = NULL;
	struct cell_s*   cell   = NULL;
	int              result = 0;
	int              i      = 0;

	cell = &table[idx];
	for (i = 0; i < cell->value.cnt; i++) {
		exp = &cell->value.exp[i];
		if (!exp->type) {
			result += exp->sign ? exp->idx : -(exp->idx);
			continue;
		}

		if (undetermined_table[exp->idx]) {
			return 0;
		}

		if (!table[exp->idx].value.calculated) {
			result += calculate_cell(exp->idx);
		} else {
			result += table[exp->idx].value.val;
		}
	}

	return result;
}

int compute_cell(int idx)
{
	struct clause_s* exp                      = NULL;
	struct s_entry   stack[ROWS*COLS]         = {0};
	struct s_entry*  stack_ptr                = &stack[ROWS*COLS - 1];
	struct s_entry   u                        = {0};
	struct cell_s*   cell                     = NULL;
	struct cell_s*   c                        = NULL;
	int              is_visited[2][ROWS*COLS] = {0};
	int              is_undetermined          = 0;
	int              link_id                  = 0;
	int              ret                      = 0;
	int              i                        = 0;

	cell             = &table[idx];
	cell->is_visited = 1;

	PUSH(cell, 1);
	/* check for undetermined */
	while (POP(c, link_id)) {
		for (link_id; link_id < ROWS*COLS; link_id++) {
			if (!c->link_table[link_id]) {
				continue;
			}

			/* check for circle */
			if (is_undetermined) {
				if (is_visited[UNDETERMINED][link_id]) {
					continue;
				}

				PUSH(c, link_id + 1);
				/* level up */
				is_visited[UNDETERMINED][link_id] = 1;
				c = &table[link_id];
				undetermined_table[link_id] = 1;
				link_id = 1;
				continue;
			}

			if (is_visited[0][link_id]) {
				is_undetermined = 1;
				undetermined_table[link_id] = 1;
				u.cell = c;
				u.idx  = link_id;
				break;
			}

			PUSH(c, link_id + 1);
			/* level up */
			is_visited[0][link_id] = 1;
			c = &table[link_id];
			undetermined_table[link_id] = 0;
			link_id = 1;
		} /* for (link_id; link_id < ROWS*COLS; link_id++) */

		if (is_undetermined) {
			if (u.cell == c) {
				undetermined_table[u.idx] = 1;
				is_undetermined = 0;
				memset(&is_visited[UNDETERMINED][0], 0, ROWS*COLS);
				u.cell = &table[0];
				u.idx = 0;
			}
		}
	} /* while (POP(c, link_id)) */

	if (undetermined_table[idx]) {
		return 1;
	}

	if (table[idx].value.calculated) {
		return 0;
	}

	cell->value.val = calculate_cell(exp->idx);
	cell->value.calculated = 1;

	return 0;
}

void initTable()
{
	memset(table, 0, sizeof(*table)*ROWS*COLS);
	return;
}

bool updateCell(int row, int col, char equation[LENGTH], int ret_val[HEIGHT][WIDTH])
{
	struct value_s* value = NULL;
	struct value_s  value_prev;
	bool            ret   = false;
	int             idx   = 0;
	int             i,j,link_cnt;

	row++;
	idx = to_index(row, col);

	/* get previous references */
	memcpy(&value_prev, &table[idx], sizeof(value_prev));

	/* get new value */
	ret = to_parse(&table[idx], equation);

	/* get link to delete */
	link_cnt = 0;

	for (j = 0; j < value_prev.cnt; j++) {
		if (value_prev.exp[j].type == 1) {
			if (link_cnt == j) {
				link_cnt++;
				continue;
			}
			memcpy(&value_prev.exp[link_cnt], &value_prev.exp[j],
			       sizeof(struct clause_s));
			link_cnt++;
		}
	}
	value_prev.cnt = link_cnt;

	for (i = 0; i < value->cnt; i++) {
		if (value->exp[i].type == 0) {
			continue;
		}

		for (j = 0; j < value_prev.cnt; j++) {
			if (value_prev.exp[j].idx == value->exp[i].idx) {
				value_prev.exp[j].idx = 0;
			}
		}
	}

	/* delete links */
	for (j = 0; j < value_prev.cnt; j++) {
		int c_idx = value_prev.exp[j].idx;
		if (c_idx == 0) {
			continue;
		}
		table[c_idx].link_table[idx] = 0;
	}

	for (j = 0; j < value_prev.cnt; j++) {
		int c_idx = value_prev.exp[j].idx;
		if (c_idx == 0) {
			continue;
		}
		compute_cell(c_idx);
	}

	ret = compute_cell(idx);
	return ret;
}

