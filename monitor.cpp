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

#define AWSPORT 26687

struct MonitorDataset{
  int clientInput[3];
  double CalculatedValues[3];
};

int main(){
  printf("The monitor is up and running\n");
  int awsSoc;
  struct MonitorDataset RecievedData;
  struct sockaddr_in aws;
  int addrlen = sizeof(aws);
  int AcceptedValue;
  double CalculatedValues[5];

  if((awsSoc = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWSPORT);

  if (bind(awsSoc,  (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }

  if (listen(awsSoc,6) < 0)
  {
    perror("\nlisten failed monitor");
    return -1;
  }

int var = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)& addrlen);

  if (recv(var, (void *) &RecievedData, sizeof(RecievedData),0) < 0)
  {
    perror("recieve failed");
    return -1;
  }
  else{
    std::cout << "The monitor recieved Link ID=<"
    << RecievedData.clientInput[0]
    << ">, size=<"
    << RecievedData.clientInput[1]
    << ">, and power=<"
    << RecievedData.clientInput[2]
    << "> from AWS"
    << std::endl;

    std::cout<< "The result for link <"
    << RecievedData.clientInput[0]
    << "> :"
    << std::endl;

    std::cout<< "Tt = <"
    << std::setprecision(2) << RecievedData.CalculatedValues[0]
    << "> ms,"
    << std::endl;

    std::cout<< "Tp = <"
    << std::setprecision(2) << RecievedData.CalculatedValues[1]
    << "> ms,"
    << std::endl;

    std::cout<< "Delay = <"
    << std::setprecision(2) << RecievedData.CalculatedValues[2]
    << "> ms,"
    << std::endl;


  }

  // printf("The Server A received input:%d, %.2f\n", recievedSample.clientInput[0], recievedSample.dbValues[3]);
  // printf("Link: %.2f\n", Testing.ChannelCap);
  // printf("size: %d\n", recievedSample.clientInput[1]);
  // printf("dProp: %.2f\n", Testing.dProp);
  // printf("dTrans: %.2f\n", Testing.dTrans);

}
