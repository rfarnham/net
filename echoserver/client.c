#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int connect_one(struct addrinfo* list) {
  // getaddrinfo() returns list of address structures.
  // We try each until we successfully connect(2).
  // If socket(2) (or connect(2)) fails, clean up and try the next one.

  for (struct addrinfo* rp = list; rp != NULL; rp = rp->ai_next) {
    int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (sfd == -1) continue;

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
      return sfd; // Success
    }

    close(sfd);
  }
  return -1;
}

void communicate(int sfd, char** msgs) {
  char buf[BUF_SIZE];
  
  for (char** msg = msgs; *msg != NULL; msg++) {
    size_t len = strlen(*msg);
    if (write(sfd, *msg, len) != len) {
      errx(EXIT_FAILURE, "partial/failed write: %ld", len);
    }

    ssize_t nread = read(sfd, buf, BUF_SIZE);
    if (nread == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    buf[nread < BUF_SIZE ? nread : BUF_SIZE - 1] = '\0';
    printf("Received %ld bytes: %s\n", nread, buf);
  }
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // Datagram socket
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          // Any protocol

  struct addrinfo* result_list;

  int error = getaddrinfo(argv[1], argv[2], &hints, &result_list);

  if (error) {
    errx(EXIT_FAILURE, "%s", gai_strerror(error));
  }

  int sfd = connect_one(result_list);
  freeaddrinfo(result_list);

  if (sfd == -1) {
    errx(EXIT_FAILURE, "Could not connect");
  }

  communicate(sfd, argv + 3);
  close(sfd);

  exit(EXIT_SUCCESS);
}
