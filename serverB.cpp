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

#define AWS_SERVB 22687
#define UDPAWS 24687

int main(){
  printf("The Server B is up and running using UDP on port <22687>\n");
  int awsSoc;
  struct sockaddr_in aws, aws2;
  int addrlen = sizeof(int);
  int Vals[3];
  if((awsSoc = socket(AF_INET, SOCK_DGRAM,0)) == 0)
  {
    printf("\nerror, Server B socket creation failed");
    return -1;
  }
  aws.sin_family = AF_INET;
  aws.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws.sin_port = htons(AWS_SERVB);

  aws2.sin_family = AF_INET;
  aws2.sin_addr.s_addr = inet_addr("127.0.0.1");
  aws2.sin_port = htons(UDPAWS);
  if (bind(awsSoc, (struct sockaddr *)&aws, sizeof aws) < 0)
  {
    perror("\nbind to socket failed");
    return -1;
  }

  std::string line, field; //initialize values to store the database contents

  std::vector< std::vector<std::string> > dbB;  // the 2D array to store the 1D array in
  std::vector<std::string> dbBRows;             // array of values for one line only

  // Read the csv database. This code is borrowed and modified from stackeroverflow disccusion as indicated in readme
  std::ifstream databaseB ("database_b.csv");
  if(!databaseB.is_open()) std::cout << "Error: Couldn't open database" << std::endl;
  std::string link;

  while (getline(databaseB,line))    // get next line in file
  {
    dbBRows.clear();
    std::stringstream ss(line);
    while (getline(ss,field,','))  // break line into comma delimitted fields
    {
      dbBRows.push_back(field);  // add each field to the 1D array
    }
    dbB.push_back(dbBRows);  // add the 1D array to the 2D array
  }
  databaseB.close();

  recvfrom(awsSoc,Vals, 3*sizeof(int),0, (struct sockaddr*)&aws, (socklen_t *)&addrlen);
  printf("The Server B received input <%d>\n", Vals[0]);

  std::stringstream x;
  x << Vals[0];
  std::string numberAsString(x.str());
  // The follow logic pics out values that match in CSV database and stores them in an array to send to AWS
  double dbValues[4];
  char *point;

  for (size_t i=0; i<dbB.size(); ++i)
  {
    if (dbB[i][0] == numberAsString)
    {
      for(int k=0; k<dbB[i].size(); k++)
      {
        const char * c = dbB[i][k].c_str();
        dbValues[k] =  strtod(c, &point);
      }
    }
  }

  if (int(dbValues[0])!=0)
  {
    printf("The server B has found < 1 > matches\n");
  }
  else
  {
    printf("The server B has found < 0 > matches\n");
  }

  if (sendto(awsSoc, dbValues, 5*sizeof(double), 0, (struct sockaddr *)&aws2 , sizeof(aws2)) < 0)
  {
    perror("failed to send\n");
    return -1;
  }
  else {
    printf("The server B finished sending the ouput to AWS\n");
  }

}
