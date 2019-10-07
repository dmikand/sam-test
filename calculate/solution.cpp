#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH	26
#define HEIGHT	99
#define LENGTH	200
#define ROWS       99
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
	memcpy(num, c, i);
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

	row = extract_const(s, max - 1) - 1;

	return to_index(row, col);
}

/* return:
 * 0 - ok
 * < 0 - error
 */
static inline int to_parse(struct cell_s* cell, char equation[LENGTH])
{
	int i = 0;
	int clause = 0;
	struct value_s* val = NULL;
	char* s = NULL;
	int max_offset = 0;

	printf("Equation: %s\n", equation);
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
			val->exp[clause].type = 1;
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
	stack_ptr->cell = (_cell);                                  \
	stack_ptr->idx =  (_idx);                                   \
	stack_ptr--;                                                \
	if(stack_ptr < stack) {printf("Stack overflow\n"); exit(1);}\
}

#define POP(_cell, _idx)                       \
(                                              \
	++stack_ptr,                               \
	(_cell) = stack_ptr->cell,                 \
	(_idx) = stack_ptr->idx,                   \
	stack_ptr > &stack[ROWS*COLS - 1] ? 0 : 1  \
)

#define UNDETERMINED 1

static inline int calculate_cell(int idx)
{
	struct clause_s* exp    = NULL;
	struct cell_s*   cell   = NULL;
	int              result = 0;
	int              value  = 0;
	int              i      = 0;

	cell = &table[idx];
	printf("calculate cell %d, value count %d\n", idx, cell->value.cnt);
	for (i = 0; i < cell->value.cnt; i++) {
		exp = &cell->value.exp[i];

		if (!exp->type) {
			value = exp->idx;
		} else {
			if (undetermined_table[exp->idx]) {
				return 0;
			}

			if (!table[exp->idx].value.calculated) {
				value = calculate_cell(exp->idx);

			} else {
				value = table[exp->idx].value.val;
			}
		}
		printf("calculate cell %d, value %d\n", idx, value);

		value %= 1000007;

		result += exp->sign ? -(value) : value;
	}

	cell->value.val = result;
	printf("calculated cell %d: %d\n", idx, result);
	cell->value.calculated = 1;


	return result;
}

int compute_cell(int idx)
{
	struct s_entry   stack[ROWS*COLS]         = {0};
	struct s_entry*  stack_ptr                = &stack[ROWS*COLS - 1];
	struct s_entry   u                        = {0};
	struct cell_s*   cell                     = NULL;
	struct cell_s*   c                        = NULL;
	int              is_visited[2][ROWS*COLS] = {0};
	int              is_undetermined          = 0;
	int              link_id                  = 0;
	int              c_idx                    = 0;
	int              ret                      = 0;
	int              i                        = 0;

	cell             = &table[idx];
	cell->is_visited = 1;
	is_visited[0][idx] = 1;

	PUSH(cell, 0);
	/* check for undetermined */
	while (POP(c, c_idx)) {
		link_id = c_idx;
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
				c->value.calculated = 0;
				link_id = 0;
				continue;
			}

			if (is_visited[0][link_id]) {
				is_undetermined = 1;
				undetermined_table[link_id] = 1;
				u.cell = c;
				u.idx  = c_idx;
				link_id = 0;
				continue;
			}

			PUSH(c, link_id + 1);
			/* level up */
			is_visited[0][link_id] = 1;
			c = &table[link_id];
			undetermined_table[link_id] = 0;
			link_id = 0;
		} /* for (link_id; link_id < ROWS*COLS; link_id++) */

		if (is_undetermined) {
			if (u.cell == c) {
				undetermined_table[u.idx] = 1;
				c->value.calculated = 0;
				is_undetermined = 0;
				memset(&is_visited[UNDETERMINED][0], 0, ROWS*COLS);
				u.cell = NULL;
				u.idx = 0;
			}
		}
	} /* while (POP(c, link_id)) */

	if (undetermined_table[idx]) {
		return 0;
	}

	if (table[idx].value.calculated) {
		return 1;
	}

	calculate_cell(idx);

	return 1;
}

void initTable()
{
	memset(table, 0, sizeof(*table)*ROWS*COLS);
	return;
}

bool updateCell(int row, int col, char equation[LENGTH], int ret_val[HEIGHT][WIDTH])
{
	struct value_s* value         = NULL;
	struct value_s* value_prev    = NULL;
	struct cell_s*  cell          = NULL;
	struct cell_s   cell_prev;
	bool            ret           = false;
	bool            no_delete     = false;
	int             del_links[25] = {-1};
	int             undetermined  = 0;
	int             idx           = 0;
	int             i,j,link_cnt;

	idx = to_index(row, col);

	/* get new value */
	memcpy(&cell_prev, &table[idx], sizeof(cell_prev));
	ret = to_parse(&table[idx], equation);
	value_prev = &cell_prev.value;
	value = &table[idx].value;

	link_cnt = 0;

	/* delete unused links */
	for (i = 0; i < value_prev->cnt; i++) {
		if (value_prev->exp[i].type == 1) {
			for (j = 0; j < value->cnt; j++) {
				if (value->exp[j].type == 1) {
					if (value_prev->exp[i].idx == value->exp[j].idx) {
						no_delete = true;
						break;
					}
				}
			}

			if (!no_delete) {
				del_links[link_cnt] = value_prev->exp[i].idx;
				table[del_links[link_cnt]].link_table[idx] = 0;
				link_cnt++;
			} else {
				no_delete = false;
			}
		}
	}

	/* add new links */
	for (i = 0; i < value->cnt; i++) {
		if (value->exp[i].type == 0) {
			continue;
		}

		table[value->exp[i].idx].link_table[idx] = 1;
	}

	/* compute deleted links */
	for (i = 0; i < link_cnt; i++) {
		if (del_links[i] != idx) {
			compute_cell(del_links[i]);
		}
	}

	ret = compute_cell(idx);

	for (i = 0; i < ROWS*COLS; i++) {
		if (undetermined_table[i]) {
			undetermined++;
		}
	}

	int* out = &ret_val[0][0];
	for (i = 0; i < ROWS*COLS; i++) {
		if (undetermined_table[i]) {
			out[i] = undetermined;
		} else {
			out[i] = table[i].value.val;
		}
	}

	printf("cell %d undetermination is %d\n", idx, undetermined);
	return !undetermined;
}

