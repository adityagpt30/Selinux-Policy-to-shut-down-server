/* A client */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>

#define	BLENGTH	64

static void
loop(int s)
{
  char buffer[BLENGTH];
  
  for (;;) {

    /* Receive prompt */
    if (recv(s, (void *)buffer, sizeof (buffer), 0) != sizeof (buffer)) {
      break;
    }

    /* Display prompt */
    fputs(buffer, stdout);

    /* Read user response */
    fgets(buffer, sizeof (buffer), stdin);

    /* Send user response */
    send(s, (void *)buffer, sizeof (buffer), 0);
  }
}

int
main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct hostent *host;
  int port;
  int s;

  /* Check usage */
  if (argc != 2) {
    printf("Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Retrieve port number */
  port = strtol(argv[1], NULL, 10);
  
  /* Create an Internet family, stream socket */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* Server listening on localhost interface */
  if ((host = gethostbyname("localhost")) == NULL) {
    perror("gethostbyname()");
    exit(EXIT_FAILURE);
  }

  /* Fill in socket address */
  memset((char *)&server, '\0', sizeof (server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  memcpy((char *)&server.sin_addr, host->h_addr_list[0], host->h_length);

  /* Connect to server */
  if (connect(s, (struct sockaddr *)&server, sizeof (server)) < 0) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  /* Talk to server */
  loop(s);

  /* Close the socket */
  close(s);

  return (0);
}
