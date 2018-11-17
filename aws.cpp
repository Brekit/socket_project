#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>


#define servAPort 21687
#define servBPort 22687
#define servCPort 23687
#define UDPport 24687
#define clientTCP 25687
#define MonitorTCP 26687

//create a FusedArray to merge client supplied inputs and databse found values
struct FusedArray{
  int clientInput[3];
  double dbValues[5];
};

//create a Dataset to send client supplied inputs and calcualted values from server C
struct MonitorDataset{
  int clientInput[3];
  double CalculatedValues[3];
};

//initiatilize values for calculated values from serverC
struct CalculatedValuesFromC{
  double ChannelCap, signalW, noiseW, dProp, dTrans, E2E;
};

//This function takes client supplied input as array and fuses them with values found in database, and sends them to server C for computing
int SendForCompute(int *array1, double *array2, int socket, struct sockaddr_in server){
  struct FusedArray FusePacket;
  for(int i=0; i < 4; i++){
    FusePacket.clientInput[i] = array1[i];
  }
  for(int j=0; j < 5; j++){
    FusePacket.dbValues[j] = array2[j];
  }
  if (sendto(socket, (char*)&FusePacket, sizeof(FusePacket), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server C failed");
    return -1;
  }
  else
  {
    std::cout << "The AWS sent link ID=<" <<FusePacket.clientInput[0] << ">, size=<" << FusePacket.clientInput[1]<<">, power=<" << FusePacket.clientInput[2]<<">, and link information to Backend-Server C using UDP port <24687>" << std::endl;
  }
}
//This function accepts calculated values from server c in a struct format and returns them
struct CalculatedValuesFromC recieveComputed(int socket, struct sockaddr_in *server, int address_length){
  struct CalculatedValuesFromC Dataset;
  if (recvfrom(socket, (void *) &Dataset, sizeof(Dataset),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
  }
  else
  {
    std::cout << "The AWS recieved outputs from Backend-server C using UDP port <24687>" << std::endl;
  }
  return Dataset;
}

// this function returns an array of Values that come from serverA's CSV database
double * recieveFromA(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkAVals[5];
  if (recvfrom(socket,linkAVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");
  }
  else
  {
    if(int(linkAVals[0])==0)
    {
      std::cout << "The AWS recieved <0> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
    else
    {
      std::cout << "The AWS recieved <1> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
  }
  return linkAVals;
}

// this function returns an array of Values that come from serverB's CSV database
double * recieveFromB(int socket, struct sockaddr_in *server, int address_length, char Val){
  static double linkBVals[5];
  if (recvfrom(socket,linkBVals, 5*sizeof(double),0,(struct sockaddr *)&server,(socklen_t*)&address_length) < 0)
  {
    perror("Couldnt recieve from server A");

  }
  else
  {
    if(int(linkBVals[0])==0)
    {
      std::cout << "LinkVal :" << linkBVals[0] << std::endl;
      std::cout << "The AWS recieved <0> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
    else
    {
      std::cout << "LinkVal :" << linkBVals[0] << std::endl;
      std::cout << "The AWS recieved <1> matches from Backend-server <" << Val << "> using UDP port <24687>" << std::endl;
    }
  }
  return linkBVals;
}

// This function accepts an array of client supplied inputs and submits them to server A
int sendDatatoA(int socket, struct sockaddr_in server, int *DataA){
  if (sendto(socket, (char*)DataA, 3*sizeof(int), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server A failed");
    return -1;
  }
  else {
    std::cout << "AWS sent link ID=<" << DataA[0] << "> to server A using UDP over port <24687>" <<std::endl;
    return 0;
  }
}
// This function accepts an array of client supplied inputs and submits them to server B
int sendDatatoB(int socket, struct sockaddr_in server, int *Data){
  if (sendto(socket, (char*)Data, 3*sizeof(int), 0, (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("Send to server A failed");
    return -1;
  }
  else {
    std::cout << "AWS sent link ID=<" << Data[0] << "> to server B using UDP over port <24687>" <<std::endl;
    return 0;
  }
}
//Recieves client supplied inputs
int *recieveClient(int socket){
  static int Values[3];
  recv(socket,Values, 3*sizeof(int),0);
  printf("Link:%d Size:%d Power:%d\n", Values[0], Values[1], Values[2]);
  return Values;
}

//Accepts an array of client supplied inputs and calculated values from serverC and sends them to the monitor
int SendToMonitor(int socket, int *array1, double delayTrans, double delayProp, double E2EDelay){
  struct MonitorDataset Data;
  for (int i=0; i < 3; i++){
    Data.clientInput[i] = array1[i];
  }
  Data.CalculatedValues[0] = delayTrans;
  Data.CalculatedValues[1] = delayProp;
  Data.CalculatedValues[2] = E2EDelay;

  if (send(socket, &Data, sizeof(Data),0) < 0)
  {
    perror("Couldnt send to Monitor!");
  }
  else if(Data.CalculatedValues[0] != 0)
  {
    std::cout << "The AWS sent to detailed results to the monitor using TCP over port <26687>" << std::endl;
    return 0;
  }
  return 0;

}

// Accepts the calculated End to End delay and submits to client
int SendToClient (int socket, double e2eDelay){
  if (e2eDelay!= 0)
  {
    if (send(socket, &e2eDelay, sizeof(e2eDelay),0) < 0)
    {
      perror("Couldnt send to Monitor!");
    }
    else
    {
      std::cout << "The AWS sent delay=<" << e2eDelay << ">ms to the client using TCP port <25687>" << std::endl;
      return 0;
    }
  }
  return 0;
}


int main(){
  //define constants
  CalculatedValuesFromC CalculatedDatasetA, CalculatedDatasetB ;
  int cli_soc, a_soc, b_soc, c_soc, mon_soc, awsAsClient;
  struct sockaddr_in client, serverA, serverB, serverC, monitor, clientAws;
  int *RecievedInputsFromClient;
  double *ResultsFromA;
  double *ResultsFromB;
  double *CalculatedValues;
  int cli_len = sizeof(client);
  int servA_len = sizeof(serverA);
  int servB_len = sizeof(serverB);
  int servC_len = sizeof(serverB);
  int mon_len = sizeof(monitor);

  // ============ Create All the Sockets and Structs which define ports to use============ //
 // The following block of code has been borrowed & modified from Beejs programing guide//

 if((awsAsClient = socket(AF_INET, SOCK_DGRAM,0)) == 0)
 {
   printf("\nerror, Server A socket creation failed");
   return -1;
 }

 if((mon_soc = socket(AF_INET, SOCK_STREAM,0)) == 0)
 {
   printf("\nerror, Monitor socket cretion failed");
   return -1;
 }

 if((cli_soc = socket(AF_INET, SOCK_STREAM,0)) == 0)
 {
   printf("\nerror, client Socket cretion failed");
   return -1;
 }

  client.sin_family = AF_INET;
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  client.sin_port = htons(clientTCP);


  clientAws.sin_family = AF_INET;
  clientAws.sin_addr.s_addr = inet_addr("127.0.0.1");
  clientAws.sin_port = htons(UDPport);


  serverA.sin_family = AF_INET;
  serverA.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA.sin_port = htons(servAPort);

  serverB.sin_family = AF_INET;
  serverB.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB.sin_port = htons(servBPort);

  serverC.sin_family = AF_INET;
  serverC.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC.sin_port = htons(servCPort);


  monitor.sin_family = AF_INET;
  monitor.sin_addr.s_addr = inet_addr("127.0.0.1");
  monitor.sin_port = htons(MonitorTCP);


  // ============ bind sockets============ //

  if (bind(awsAsClient,  (struct sockaddr *)&clientAws, sizeof clientAws) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }

  if (bind(cli_soc,  (struct sockaddr *)&client, sizeof client) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }

  if (bind(mon_soc,  (struct sockaddr *)&monitor, sizeof monitor) < 0)
  {
    perror("\nbind to monitor failed");
    return -1;
  }
  printf("The AWS is up and running\n");
  // ============ Listen from client and send to server A,B,C ============ //
  while(true){
  if (listen(mon_soc,6) < 0)
  {
    perror("\nlisten failed monitor");
    return -1;
  }

  if (listen(cli_soc,6) < 0)
  {
    perror("\nlisten failed client");
    return -1;
  }

  //Create child sockets for client nad monitor
  int cli_child = accept(cli_soc, (struct sockaddr *)&client,(socklen_t*)& cli_len);
  int mon_child = accept(mon_soc, (struct sockaddr *)&monitor,(socklen_t*)& monitor);

  //Recieve input from client
  RecievedInputsFromClient = recieveClient(cli_child);

  //send recieved input from client to server A and B to search & recieve the values back
  sendDatatoA(awsAsClient, serverA, RecievedInputsFromClient);
  ResultsFromA = recieveFromA(awsAsClient, &serverA, servA_len, 'A');
  sendDatatoB(awsAsClient, serverB, RecievedInputsFromClient);
  ResultsFromB = recieveFromB(awsAsClient, &serverB, servB_len, 'B');

  //Check to see which values are empty and which values are full so we can display to client and monitor appropriately
  if ((int(ResultsFromA[0])==0) && (int(ResultsFromA[0])==0))
  {
    //Pass 0's to the function if nothing exists in the array, meaning ServerA/B did not find the link in their csv file
    SendToMonitor(mon_child, RecievedInputsFromClient , 0,0,0);
    SendToClient(cli_child, 0);

  } else if (int(ResultsFromA[0])!=0){
    //Send data to server C for computing if the received values from A/B are not 0
    SendForCompute(RecievedInputsFromClient, ResultsFromA, awsAsClient, serverC);
    struct CalculatedValuesFromC CalculatedDatasetA = recieveComputed(awsAsClient, &serverC, servC_len);
    //Send computed results to client and monitor
    SendToMonitor(mon_child, RecievedInputsFromClient,CalculatedDatasetA.dTrans,CalculatedDatasetA.dProp,CalculatedDatasetA.E2E);
    SendToClient(cli_child, CalculatedDatasetA.E2E);

  } else if (int(ResultsFromB[0])!=0){
    //Send data to server C for computing if the received values from A/B are not 0
      SendForCompute(RecievedInputsFromClient, ResultsFromB, awsAsClient, serverC);
      struct CalculatedValuesFromC CalculatedDatasetB = recieveComputed(awsAsClient, &serverC, servC_len);
      //Send computed results to client and monitor
      SendToMonitor(mon_child, RecievedInputsFromClient,CalculatedDatasetB.dTrans,CalculatedDatasetB.dProp,CalculatedDatasetB.E2E );
      SendToClient(cli_child, CalculatedDatasetB.E2E);
    } else {
    SendToMonitor(mon_child, RecievedInputsFromClient , 0,0,0);

  }
  close(mon_child); //close child sockets
  close(cli_child); //close child sockets
}

}
