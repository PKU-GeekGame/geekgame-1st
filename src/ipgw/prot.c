#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int protect(char *input, int in_len, char *output, int *out_len, char *err)
{
	int shift = (in_len * 2 + 1) % 8;
	int mask = (1 << shift) - 1;
	char last = input[in_len-1];
	for (int i = 0; i < in_len; ++i) {
		char cur = input[i];
		output[i] = ((~last) & mask) | (cur & (~mask));
		last = cur;
	}
	*out_len = in_len;
	return 1;
}

int unprotect(char *input, int in_len, char *output, int *out_len, char *err)
{
	int shift = (in_len * 2 + 1) % 8;
	int mask = (1 << shift) - 1;
	for (int i = 0; i < in_len; ++i) {
		char cur = input[i];
		char next = input[(i+1)%in_len];
		output[i] = ((~next) & mask) | (cur & (~mask));
	}
	*out_len = in_len;
	return 1;
}

void output(char *buf, int len)
{
	for (int i = 0; i < len; ++i)
		printf("%c", buf[i]);
}

int main(int argc, char *argv[])
{
	int enc;
	char *name;
	if (argc >= 3 && strcmp(argv[1], "-d") == 0) {
		enc = 0;
		name = argv[2];
	}
	else if (argc >= 2) {
		enc = 1;
		name = argv[1];
	}
	else
		return 0;

	FILE *f = fopen(name, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = malloc(size);
	fread(buf, size, 1, f);

	char *buf2 = malloc(size);
	if (enc)
		protect(buf, size, buf2, &size, NULL);
	else
		unprotect(buf, size, buf2, &size, NULL);
	output(buf2, size);

	return 0;
}
