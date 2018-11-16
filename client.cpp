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
#include <iomanip>


struct Send2Client{
  double value;
};

#define PORT 25687
/*
void sendData(int socket, int value){
  send(socket, value, sizeof(value), 0);
}
*/

int main(int argc, char* argv[]){
  char *pEnd;
  printf("The client is up and running\n");
  int link = atoi(argv[1]);
  int size = atoi(argv[2]);
  int power = atoi(argv[3]);
  int Vals[3] = {link, size, power};
  //double AWSData;
  Send2Client AWSData;

   //struct sockaddr_in address;
  int awsSoc = 0, valread;
  struct sockaddr_in aws;
  if ((awsSoc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Socket creation error\n");
    return -1;
  }

  memset(&aws,'0',sizeof(aws));

  aws.sin_family = AF_INET;
  aws.sin_port = htons(PORT);
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");

  char buffer[2048] = {0};

  if (connect(awsSoc, (struct sockaddr *)&aws, sizeof(aws)) < 0)
  {
    perror("Connection Failed");
    return -1;
  }

  if (send(awsSoc, (char*)Vals, 3*sizeof(int), 0) < 0){
    perror("failed to send\n");
    return -1;
  } else {
    printf("The client sent link=%d, size=%d, power=%d to AWS.\n", link, size, power);
  }

  if (recv(awsSoc,  (void*)&AWSData, sizeof(AWSData),0) < 0) {
    perror("Failed to receive from AWS");
  } else if(AWSData.value!=0) {
  std::cout << "The delay for link <" << link << "> is <" << std::setprecision(2) <<  AWSData.value << ">ms" << std::endl;
} else {
  std::cout << "Found no matches for link <" << link << "> " << std::endl;
}




  /*
  valread=read(awsSoc , buffer, 2048);
  printf("%s\n", buffer);
  return 0;
  */

}
