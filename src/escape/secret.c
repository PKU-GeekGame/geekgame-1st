#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

void init(void)
{
  setvbuf(stdout, NULL, _IOLBF, 0);

  puts("Welcome, please enter some string and "
      "I will process escape characters for you!");
}

void work(void)
{
  size_t n;
  ssize_t m;
  char *from, *to;
  char *buffer = NULL;
  struct {
    char input[128];
    char escaped[128];
    uint32_t id;
  } ctx;

  ctx.id = 0;
  for (;;)
  {
    if (buffer)
      free(buffer);
    buffer = NULL;

    printf("Case #%d:\n", ctx.id++);

    n = 0;
    if ((m = getline(&buffer, &n, stdin)) <= 0)
      break;
    if (buffer[m - 1] == '\n')
      buffer[m - 1] = '\0';
    if (strcmp(buffer, "exit") == 0
        || strcmp(buffer, "quit") == 0)
      break;

    m = strlen(buffer);
    if (m == 0) {
      break;
    } else if (m > sizeof(ctx.input)) {
      puts("Input string is too long!");
      continue;
    }
    strcpy(ctx.input, buffer);

    from = ctx.input;
    to = ctx.escaped;
    while (*from)
    {
      if (from[0] == '\\' && !isspace(from[1]))
        ++from;
      *to++ = *from++;
    }
    *to++ = '\0';

    puts(ctx.escaped);
  }
}

void print_flag(const char *flagfile)
{
  char flag[30];
  int fd;

  fd = open(flagfile, O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  if (read(fd, flag, 29) != 29) {
    perror("read");
    exit(1);
  }

  flag[29] = '\0';
  if (strcmp(flagfile, "./.flag") == 0)
    puts(flag);

  exit(0);
}

int main(int argc, char **argv)
{
  init();
  work();

  return 0;
}
