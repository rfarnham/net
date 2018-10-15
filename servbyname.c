#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

void print_usage(char* prog) {
  fprintf(stderr, "Usage:\t%s -n service [proto]\n\t%s -p port [proto]\n", prog, prog);
}

void print_aliases(char** aliases) {
  for (char** p = aliases; *p != NULL; p++) {
    printf("%s ", *p);
  }
  putchar('\n');
}

void print_ent(struct servent* ent) {
  printf("struct servent {\n");
  printf("\ts_name = %s\n", ent->s_name);
  printf("\ts_alises = ");
  print_aliases(ent->s_aliases);
  printf("\ts_port = %d\n", ent->s_port);
  printf("\ts_proto = %s\n", ent->s_proto);
  printf("}\n");
}

int main(int argc, char** argv) {
  struct servent* ent;
  if (argc < 3) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  } else if (strcmp(argv[1], "-n") == 0) {
    ent = getservbyname(argv[2], argc == 4 ? argv[3] : NULL);
  } else if (strcmp(argv[1], "-p") == 0) {
    int port = atoi(argv[2]);
    ent = getservbyport(port, argc == 4 ? argv[3] : NULL);
  } else {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (ent == NULL) {
    errx(EXIT_FAILURE, "Unknown service");
  }
  
  print_ent(ent);
  
  endservent();

  exit(EXIT_SUCCESS);
}
