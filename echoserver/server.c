#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int bind_one(struct addrinfo* list) {
  // getaddrinfo() returns list of address structures.
  // We try each until we successfully bind(2).
  // If socket(2) (or bind(2)) fails, clean up and try the next one.

  for (struct addrinfo* rp = list; rp != NULL; rp = rp->ai_next) {
    int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) continue;

    if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      return sfd;
    }

    close(sfd); // Otherwise cleanup and continue
  }

  return -1;
}

#define BUF_SIZE 1024
void echo(int sfd) {
  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len = sizeof(peer_addr);
  char buf[BUF_SIZE];

  for (;;) {
    ssize_t nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*) &peer_addr, &peer_addr_len);

    if (nread == -1) continue; // Ignore failed request

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int error = getnameinfo((struct sockaddr*) &peer_addr, peer_addr_len,
                            host, NI_MAXHOST,
                            service, NI_MAXSERV,
                            NI_NUMERICSERV);

    if (error) {
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(error));
    } else {
      printf("Received %ld bytes from %s:%s\n", nread, host, service);
    }

    if (sendto(sfd, buf, nread, 0,
               (struct sockaddr*) &peer_addr,
               peer_addr_len) != nread) {
      fprintf(stderr, "Error sending response: %s\n", strerror(errno));
    }
  }
}
           

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  struct addrinfo *result_list;

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // Datagram socket
  hints.ai_flags = AI_PASSIVE;    // For wildcard IP address on which to bind(2)
  hints.ai_protocol = 0;          // Any protocol
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  int error = getaddrinfo(NULL, argv[1], &hints, &result_list);
  if (error) {
    errx(EXIT_FAILURE, "%s", gai_strerror(error));
  }

  int sfd = bind_one(result_list);
  freeaddrinfo(result_list);

  if (sfd == -1) {
    errx(EXIT_FAILURE, "Could not bind");
  }

  echo(sfd);
}
