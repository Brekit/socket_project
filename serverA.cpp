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
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>

#define AWS_SERVA 21687
#define UDPAWS 24687

int main(){
  printf("The Server A is up and running using UDP port <21687>\n");
  int awsSoc;
  struct sockaddr_in aws, aws2;
  int addrlen = sizeof(int);
  int Vals[3];
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server A socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVA);

  aws2.sin_family = AF_INET;
  aws2.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws2.sin_port = htons(UDPAWS);


  std::ifstream databaseA ("database_a.csv");
  if(!databaseA.is_open()) std::cout << "Error: Couldn't open database" << std::endl;
  std::string link;

  std::string line, field;

  std::vector< std::vector<std::string> > dbA;  // the 2D array
  std::vector<std::string> dbARows;                // array of values for one line only

  while (getline(databaseA,line))    // get next line in file
  {
    dbARows.clear();
    std::stringstream ss(line);
    while (getline(ss,field,','))  // break line into comma delimitted fields
    {
      dbARows.push_back(field);  // add each field to the 1D array
    }
    dbA.push_back(dbARows);  // add the 1D array to the 2D array
  }
  databaseA.close();
  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }
  //int new_socket = accept(awsSoc, (struct sockaddr *)&aws,(socklen_t*)&addrlen);
  //while(true){
    recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&aws, (socklen_t *)&addrlen);
    //recv(awsSoc,Vals, 3*sizeof(int),0);
    //close(awsSoc);
    //printf("The Server A received input <%d>\n", Vals[0]);

    std::stringstream x;
    x << Vals[0];
    std::string numberAsString(x.str());

    std::cout << "Checking for entry in db " << numberAsString << std::endl;


    // print out what was read in
    double dbValues[4];
    char *point;

    for (size_t i=0; i<dbA.size(); ++i)
    {
      if (dbA[i][0] == numberAsString)
      {
        for(int k=0; k<dbA[i].size(); k++)
        {
          const char * c = dbA[i][k].c_str();
          dbValues[k] =  strtod(c, &point);
          std::cout << dbValues[k] << "*\n";
        }
      }
    }

    if (int(dbValues[0])!=0)
    {
      printf("The server A has found < 1 > matches\n");
    }
    else
    {
      printf("The server A has found < 0 > matches\n");
    }


  if (sendto(awsSoc, dbValues, 5*sizeof(double), 0, (struct sockaddr *)&aws2 , sizeof(aws2)) < 0)
  {
    perror("failed to send\n");
    return -1;
  }
  else {
    printf("The server A finished sending the ouput to AWS\n");
  }
//}


}
