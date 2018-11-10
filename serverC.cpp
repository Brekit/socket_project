// use UDP port 23687
//computing server.
// use UDP port 21687


//1 UDP, 24000+xxx1 TCP with client, 25000+xxx1 TCP with monitor, 26000+xxx
//UDP 24687
// TCP with Client 25687
//TCP with Monitor 26687
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
#include <iostream>
#define AWS_SERVA 23687

int main(){
  printf("Server A is up and running\n");
  int awsSoc;
  struct sockaddr_in aws;
  //struct sockaddr_storage src_addr;
  //socklen_t src_addr_len=sizeof(src_addr);
  int addrlen = sizeof(int);
  int Vals[3];
  //memset(&hints,0,sizeof(hints));
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVA);

  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }
  int new_socket = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)&addrlen);
  //recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&src_addr,&src_addr_len);
  recv(awsSoc,Vals, 3*sizeof(int),0);
  printf("Link:%d\nSize:%d\nPower:%d\n", Vals[0], Vals[1], Vals[2]);
}
