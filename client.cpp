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

int main(int argc, char* argv[]){
  char *pEnd;
  printf("The client is up and running\n");
  int link = atoi(argv[1]);
  int size = atoi(argv[2]);
  int power = atoi(argv[3]);
 printf("The client sent link=%d, size=%d, power=%d to AWS.\n", link, size, power);

  //struct sockaddr_in address;
  int cli_soc = 0, valread;
  struct sockaddr_in serv_addr;
  const char *msg = "\nClient:Hey, its the client. was good";
  if ((cli_soc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Socket creation error\n");
    return -1;
  }

  memset(&serv_addr,'0',sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
/*
  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  {
    printf("\nInvalid Address/ Address not supported\n");
    return -1;
  }
*/
  char buffer[2048] = {0};

  if (connect(cli_soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connection Failed");
    return -1;
  }
  for (int i=0; i<argc; i++){
    uint32_t un = htonl(atoi(argv[i]));
  send(cli_soc, &un, sizeof(uint32_t), 0);
  }

  printf("\nClient:Sent message to AWSs");
  valread=read(cli_soc , buffer, 2048);
  printf("%s\n", buffer);
  return 0;

}
