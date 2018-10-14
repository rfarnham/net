#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

#define AI_FLAG_NUM 10
int flags[AI_FLAG_NUM] = {AI_ADDRCONFIG, AI_ALL, AI_CANONNAME, AI_NUMERICHOST,
                          AI_NUMERICSERV, AI_PASSIVE, AI_V4MAPPED, AI_V4MAPPED_CFG,
                          AI_DEFAULT, AI_UNUSABLE};

const char* flag_names[AI_FLAG_NUM] = {"AI_ADDRCONFIG", "AI_ALL", "AI_CANONNAME", "AI_NUMERICHOST",
                                       "AI_NUMERICSERV", "AI_PASSIVE", "AI_V4MAPPED", "AI_V4MAPPED_CFG",
                                       "AI_DEFAULT", "AI_UNUSABLE"};

void print_flags(int ai_flags) {
  for (int i = 0; i < AI_FLAG_NUM; i++) {
    if (ai_flags & flags[i]) {
      fputs(flag_names[i], stdout);
      putchar(' ');
    }
  }
  putchar('\n');
}

void print_family(int ai_family) {
  switch (ai_family) {
  case AF_INET:
    puts("AF_INET");
    break;
  case AF_INET6:
    puts("AF_INET6");
    break;
  case AF_UNSPEC:
    puts("AF_UNSPEC");
    break;
  default:
    puts("?");
  }
}

void print_socktype(int ai_socktype) {
  switch (ai_socktype) {
  case SOCK_DGRAM:
    puts("SOCK_DGRAM");
    break;
  case SOCK_STREAM:
    puts("SOCK_STREAM");
    break;
  default:
    puts("?");
  }
}

void print_protocol(int ai_protocol) {
  switch (ai_protocol) {
  case IPPROTO_UDP:
    puts("IPPROTO_UDP");
    break;
  case IPPROTO_TCP:
    puts("IPPROTO_TCP");
    break;
  default:
    puts("?");
  }
}

void print_addr(struct sockaddr* ai_addr) {
  fputs("\tai_addr = ", stdout);
  struct sockaddr_in* ipv4;
  struct sockaddr_in6* ipv6;
  switch (ai_addr->sa_family) {
  case AF_INET:
    ipv4 = (struct sockaddr_in*)ai_addr;
    char ipv4_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipv4_address, INET_ADDRSTRLEN);
    puts(ipv4_address);
    break;
  case AF_INET6:
    ipv6 = (struct sockaddr_in6*)ai_addr;
    char ipv6_address[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipv6_address, INET6_ADDRSTRLEN);
    puts(ipv6_address);
    break;
  default:
    puts("?");
  }
}

void print_ai(struct addrinfo* ai) {
  puts("struct addrinfo {");

  printf("\tai_flags = %d: ", ai->ai_flags);
  print_flags(ai->ai_flags);

  printf("\tai_family = %d: ", ai->ai_family);
  print_family(ai->ai_family);
  
  printf("\tai_socktype = %d: ", ai->ai_socktype);
  print_socktype(ai->ai_socktype);

  printf("\tai_protocol = %d: ", ai->ai_protocol);
  print_protocol(ai->ai_protocol);

  printf("\tai_addrlen = %d\n", ai->ai_addrlen);
  print_addr(ai->ai_addr);

  printf("\tai_canonname = %s\n", ai->ai_canonname);
  puts("}");
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <hostname>\n", argv[0]);
    return 1;
  }

  struct addrinfo* res_list;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_CANONNAME;

  int error = getaddrinfo(argv[1], NULL, &hints, &res_list);
  if (error) {
    errx(1, "%s", gai_strerror(error));
    return 1;
  }

  printf("addrinfo for %s\n", argv[1]);
  for (struct addrinfo* res = res_list; res != NULL; res = res->ai_next) {
    print_ai(res);
  }

  freeaddrinfo(res_list);
  return 0;
}
