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
  if ((cli_soc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Socket creation error\n");
    return -1;
  }

  memset(&serv_addr,'0',sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  {
    printf("\nInvalid Address/ Address not supported\n");
    return -1;
  }

  char buffer[2048] = {0};

  if (connect(cli_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed\n");
    return -1;
  }

  send(cli_soc, msg, strlen(msg), 0);
  printf("homles, we client sent shit");
  valread=read(cli_soc , buffer, 2048);
  printf("%s\n", buffer);
  return 0;

}
