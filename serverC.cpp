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
#include <math.h>
#define AWS_SERVC 23687


double dBmtoWatts(double dbm){
  double Watts;
  double x = dbm/10;
  Watts = pow(10,x)/1000;
  return Watts;
}

struct FusedArray{
  int clientInput[3];
  double dbValues[5];
};

struct ComputeTheseValues{
  double ChannelCap, signalW, noiseW, dProp, dTrans, E2E;
};

struct ComputeTheseValues Compute(double bandwith, double signalIndBm, double noiseIndBm, double distance, double speed, double size ){
  struct ComputeTheseValues computed;
  computed.signalW = dBmtoWatts(signalIndBm);
  computed.noiseW = dBmtoWatts(noiseIndBm);
  computed.ChannelCap = (bandwith*10e6) * (log2 (1+(computed.signalW/computed.noiseW)));
  computed.dProp = (distance*1000)/(speed*10e7)/10e-6;
  computed.dTrans = (size/computed.ChannelCap)/10e-6;
  computed.E2E = computed.dProp+computed.dTrans;
  return computed;
}

int main(){
  printf("Server C is up and running\n");
  int awsSoc;
  double linkRate;
  FusedArray recievedSample;
  struct sockaddr_in aws;
  //struct sockaddr_storage src_addr;
  //socklen_t src_addr_len=sizeof(src_addr);
  int addrlen = sizeof(int);
  int Vals[3];
  //memset(&hints,0,sizeof(hints));
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server C socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVC);

  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }

  int new_socket = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)&addrlen);
  while(true){
  if (recvfrom(awsSoc,&recievedSample, sizeof(recievedSample),0, (struct sockaddr*)&aws, (socklen_t *)&addrlen) < 0)
  {
    perror("Recieve failed");
    return -1;
  }
  //recv(awsSoc,Vals, 3*sizeof(int),0);
  //close(awsSoc);
  printf("The Server C received link information of <%d>, file size <%d>, and signal power <%d>\n", recievedSample.clientInput[0], recievedSample.clientInput[1], recievedSample.clientInput[2]);
          //Compute(double bandwith, double signalIndBm, double noiseIndBm, double distance, double speed, int size ){
  struct ComputeTheseValues Testing = Compute(recievedSample.dbValues[1], recievedSample.clientInput[2], recievedSample.dbValues[4], recievedSample.dbValues[2], recievedSample.dbValues[3], recievedSample.clientInput[1]);
  printf("The Server C finished calculation for link <%d>\n", recievedSample.clientInput[0]);

  //sendto(c_soc, (char*)&Sample, sizeof(Sample), 0, (struct sockaddr *)&serverC , sizeof(serverC))

  if (sendto(awsSoc,&Testing, sizeof(Testing),0, (struct sockaddr*)&aws, sizeof(aws)) < 0)
  {
    perror("Recieve failed");
    return -1;
  }
  else {

  }

}
}
