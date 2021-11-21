#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/mman.h>

uint8_t *data, *code;
uint64_t data_len, code_len;
uint8_t *data_ptr, *code_ptr;
uint8_t **jmp_table;

#define MIN_DATA_LEN	0x1000

uint8_t **jit_code;
uint8_t *jit_buf, *jit_ptr;
uint64_t *exec_count;

#define JIT_BUF_SIZE	0x100000
#define JIT_EXEC_COUNT	16

typedef uint8_t *(*jit_code_t)(uint8_t*);

void init_jit()
{
	jit_code = malloc(sizeof(uint8_t*) * code_len);
	memset(jit_code, 0, sizeof(uint8_t*) * code_len);
	jit_buf = mmap(NULL, JIT_BUF_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	jit_ptr = jit_buf;
	exec_count = malloc(sizeof(uint64_t) * code_len);
	memset(exec_count, 0, sizeof(uint64_t) * code_len);
}

void dump_code(const uint8_t *code, unsigned size)
{
	if (jit_ptr + size > jit_buf + JIT_BUF_SIZE)
	{
		fprintf(stderr, "jit buffer is too small\n");
		exit(-1);
	}
	memcpy(jit_ptr, code, size);
	jit_ptr += size;
}

void read_code(const char *fname)
{
	FILE *f = fopen(fname, "r");
	fseek(f, 0, SEEK_END);
	code_len = ftell(f);
	fseek(f, 0, SEEK_SET);
	code = malloc(code_len);
	fread(code, 1, code_len, f);
	fclose(f);
}

void calc_jump()
{
	uint64_t *stack = malloc(sizeof(uint64_t) * code_len);
	uint8_t top = 0;
	jmp_table = malloc(sizeof(uint8_t*) * code_len);
	for (uint64_t i = 0; i != code_len; ++i)
	{
		if (code[i] == '[')
			stack[top++] = i;
		else if (code[i] == ']')
		{
			if (top == 0)
			{
				fprintf(stderr, "brackets error\n");
				exit(-1);
			}
			uint64_t p = stack[--top];
			jmp_table[p] = code + i;
			jmp_table[i] = code + p;
		}
	}
	if (top != 0)
	{
		fprintf(stderr, "brackets error\n");
		exit(-1);
	}
	free(stack);
}

void realloc_data()
{
	uint64_t pos = data_ptr - data;
	uint64_t new_data_len = data_len * 2;
	if (new_data_len < MIN_DATA_LEN)
		new_data_len = MIN_DATA_LEN;
	data = realloc(data, new_data_len);
	memset(data + data_len, 0, new_data_len - data_len);
	data_len = new_data_len;
	data_ptr = data + pos;
}

#define MOV_RDI_RBX		"\x48\x89\xfb"
#define INC_RBX			"\x48\xff\xc3"
#define DEC_RBX			"\x48\xff\xcb"
#define INC_M_RBX		"\xfe\x03"
#define DEC_M_RBX		"\xfe\x0b"
#define CLEAR_RDI		"\x48\x31\xff"
#define MOV_M_RBX_DI	"\x66\x8b\x3b"
#define MOV_ABS_RAX		"\x48\xb8"
#define CALL_RAX		"\xff\xd0"
#define MOV_AX_M_RBX	"\x66\x89\x03"
#define CMP_0_M_RBX		"\x80\x3b\x00"
#define JE_REL32		"\x0f\x84"
#define JMP_REL32		"\xe9"
#define RET				"\xc3"
#define INT3			"\xcc"
#define PUSH_RBX		"\x53"
#define POP_RBX			"\x5b"
#define MOV_RBX_RAX		"\x48\x89\xd8"

#define DUMP_CODE(code)	dump_code((code), sizeof(code)-1)

#define CALL_ABS(addr) do {			\
	uint64_t _addr = (uint64_t)(addr);	\
	DUMP_CODE(MOV_ABS_RAX); 		\
	dump_code((void*)&_addr, 8);	\
	DUMP_CODE(CALL_RAX); 			\
} while (0)

#define ADDR_OFFSET(from, to, insn)	\
	((int64_t)(to) - (int64_t)(from) - (int64_t)(insn))

#define JUMP_EQ(addr, _jit_ptr) ({				\
	size_t pre_len = sizeof(JE_REL32) - 1;		\
	size_t inst_len = pre_len + 4;				\
	int32_t offset = ADDR_OFFSET(_jit_ptr, addr, inst_len);	\
	memcpy(_jit_ptr, JE_REL32, pre_len);		\
	memcpy(_jit_ptr + pre_len, &offset, 4);		\
	inst_len;									\
})

#define JUMP(addr, _jit_ptr) ({				\
	size_t pre_len = sizeof(JMP_REL32) - 1;		\
	size_t inst_len = pre_len + 4;				\
	int32_t offset = ADDR_OFFSET(_jit_ptr, addr, inst_len);	\
	memcpy(_jit_ptr, JMP_REL32, pre_len);		\
	memcpy(_jit_ptr + pre_len, &offset, 4);		\
	inst_len;									\
})

jit_code_t compile_code(size_t begin_index, size_t end_index)
{
	size_t code_size = end_index - begin_index;
	uint8_t **code_pos = malloc(sizeof(uint8_t*) * code_size);
	uint8_t **jit_fill = malloc(sizeof(uint8_t*) * code_size);
	uint32_t nest_count = 0;

	/* align code */
	while (((uint64_t)jit_ptr & 0xf) != 0)
		DUMP_CODE(INT3);

	jit_code_t ans = (jit_code_t)jit_ptr;

	/* save rbx */
	DUMP_CODE(PUSH_RBX);

	/* rbx = rdi */
	DUMP_CODE(MOV_RDI_RBX);

	for (size_t i = 0; i < code_size; ++i) {
		code_pos[i] = jit_ptr;
		size_t index = begin_index + i;
		char op = code[index];
		if (op == '>') {
			/* rbx++ */
			DUMP_CODE(INC_RBX);
		}
		else if (op == '<') {
			/* rbx-- */
			DUMP_CODE(DEC_RBX);
		}
		else if (op == '+') {
			/* *rbx++ */
			DUMP_CODE(INC_M_RBX);
		}
		else if (op == '-') {
			/* *rbx-- */
			DUMP_CODE(DEC_M_RBX);
		}
		else if (op == '.') {
			/* putchar(*rbx) */
			DUMP_CODE(CLEAR_RDI);
			DUMP_CODE(MOV_M_RBX_DI);
			CALL_ABS(putchar);
		}
		else if (op == ',') {
			/* *rbx = getchar() */
			CALL_ABS(getchar);
			DUMP_CODE(MOV_AX_M_RBX);
		}
		else if (op == '[') {
			nest_count++;
			/* if (*rbx == 0) goto ']'+1 */
			DUMP_CODE(CMP_0_M_RBX);
			size_t jmp_index = jmp_table[index] - code - begin_index;
			/* The corresponding ']' does not exist yet,
			   so we should take the space and fill it later. */
			jit_fill[i] = jit_ptr;
			size_t inst_len = JUMP_EQ(code_pos[jmp_index + 1], jit_ptr);
			jit_ptr += inst_len;
		}
		else if (op == ']') {
			--nest_count;
			/* goto '[' */
			size_t jmp_index = jmp_table[index] - code - begin_index;
			size_t inst_len = JUMP(code_pos[jmp_index], jit_ptr);
			jit_ptr += inst_len;
			/* fill the corresponding '[' part */
			JUMP_EQ(jit_ptr, jit_fill[jmp_index]);
		}
		else
			; /* nothing */
	}

	/* rax = rbx */
	DUMP_CODE(MOV_RBX_RAX);

	/* restore rbx */
	DUMP_CODE(POP_RBX);

	/* return */
	DUMP_CODE(RET);

	if (nest_count != 0) {
		fprintf(stderr, "nest_count error\n");
		exit(-1);
	}

	free(jit_fill);
	free(code_pos);
	return ans;
}

void run_right()
{
	++code_ptr;
	++data_ptr;
	if (data_ptr >= data + data_len)
		realloc_data();
}

void run_left()
{
	++code_ptr;
	if (data_ptr != data)
		--data_ptr;
}

void run_add()
{
	++code_ptr;
	++*data_ptr;
}

void run_sub()
{
	++code_ptr;
	--*data_ptr;
}

void run_put()
{
	++code_ptr;
	putchar(*data_ptr);
}

void run_get()
{
	++code_ptr;
	*data_ptr = getchar();
}

void run_lbracket()
{
	if (*data_ptr != 0)
		++code_ptr;
	else
		code_ptr = jmp_table[code_ptr - code] + 1;
}

void run_rbracket()
{
	code_ptr = jmp_table[code_ptr - code];
}

void run_other()
{
	/* report error? */
	++code_ptr;
}

int run_one()
{
	if (code_ptr == code + code_len)
		return 0;
	char op = *code_ptr;
	size_t index = code_ptr - code;
	if (op == '>')
		run_right();
	else if (op == '<')
		run_left();
	else if (op == '+')
		run_add();
	else if (op == '-')
		run_sub();
	else if (op == '.')
		run_put();
	else if (op == ',')
		run_get();
	else if (op == '[') {
		jit_code_t func = (jit_code_t)jit_code[index];
		if (func == NULL) {
			if (++exec_count[index] >= JIT_EXEC_COUNT) {
				func = compile_code(index, jmp_table[index] + 1 - code);
				jit_code[index] = (uint8_t*)func;
			}
		}
		if (func != NULL) {
			data_ptr = func(data_ptr);
			code_ptr = jmp_table[index] + 1;
		}
		else
			run_lbracket();
	}
	else if (op == ']')
		run_rbracket();
	else
		run_other();
	return 1;
}

void put_flag1()
{
	FILE *f = fopen("flag1", "r");
	fseek(f, 0, SEEK_END);
	int flag_len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *flag = malloc(flag_len);
	fread(flag, 1, flag_len, f);
	fclose(f);
	memcpy(data + data_len - flag_len, flag, flag_len);
	free(flag);
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	if (argc == 1)
	{
		printf("%s <file>\n", argv[0]);
		return 1;
	}
	read_code(argv[1]);
	calc_jump();
	data_ptr = data;
	code_ptr = code;
	realloc_data();
	put_flag1();

	init_jit();

	while (run_one())
	{
		// nothing
	}

	free(code);
	free(data);
	free(jmp_table);
	free(exec_count);
	free(jit_code);
	return 0;
}
