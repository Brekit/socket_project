#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define PORT 24687

int main(){
  printf("Holmes, client started");
  struct sockaddr_in address;
  int cli_soc = 0, valread;
  struct sockaddr_in serv_addr;

  const char *msg = "yo nug, was good";

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  char buffer[2048] = {0};
  cli_soc = socket(AF_INET, SOCK_STREAM, 0);

  connect(cli_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  send(cli_soc, msg, strlen(msg), 0);
  printf("homles, we client sent shit");
  valread=read(cli_soc , buffer, 2048);
  printf("%s\n", buffer);
  return 0;

}
