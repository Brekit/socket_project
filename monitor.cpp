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

  //define constants to use
  int awsSoc;
  struct MonitorDataset RecievedData;
  struct sockaddr_in aws;
  int addrlen = sizeof(aws);
  int AcceptedValue;
  double CalculatedValues[5];

  //create socket
  if((awsSoc = socket(AF_INET, SOCK_STREAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWSPORT);


  //connect and recieve data from AWS
  printf("The monitor is up and running\n");
  if (connect(awsSoc, (struct sockaddr *)&aws, sizeof(aws)) < 0)
  {
    perror("Connection Failed");
    return -1;
  }

  if (recv(awsSoc, (void *) &RecievedData, sizeof(RecievedData),0) < 0)
  {
    perror("recieve failed");
    return -1;
  }
  if (RecievedData.CalculatedValues[0]==0){
    std::cout << "Found no matches for link <" << RecievedData.clientInput[0] << ">" << std::endl;
  }
  else{
    //output Data
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
    << std::setprecision(4) << RecievedData.CalculatedValues[0]
    << "> ms,"
    << std::endl;

    std::cout<< "Tp = <"
    << std::setprecision(4) << RecievedData.CalculatedValues[1]
    << "> ms,"
    << std::endl;

    std::cout<< "Delay = <"
    << std::setprecision(4) << RecievedData.CalculatedValues[2]
    << "> ms,"
    << std::endl;
  }

}
