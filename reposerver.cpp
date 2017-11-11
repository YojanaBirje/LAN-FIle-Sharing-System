/*
Yojana Birje
20162130
*/
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<iostream>
#include <arpa/inet.h>
#include <fstream>
#include<new>
#include<map>
#include<vector>
#include <sstream>
#include<time.h>


void service(int,char*); 
void error(const char *msg){
    perror(msg);
    exit(1);
}
void logFile(int n,char *cl){ 

  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  
  ofstream outfile1;
  outfile1.open("repo.log", std::ios_base::app);

  if(n==1){
    outfile1<<"share request from "<<cl <<" "<< asctime(timeinfo);
  }

  if(n==2){
    outfile1<<"search request from "<<cl <<" "<< asctime(timeinfo); 
    outfile1<<"search response send to "<<cl <<" "<< asctime(timeinfo);
  }
   outfile1.close();

}

int main(int argc, char *argv[]){

     int sockfd, newsockfd, portno, pid;
     char *cl=new char;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);  //create socket
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr)); //initialise buffer to zero
     
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET; //address family

     serv_addr.sin_addr.s_addr = INADDR_ANY; // Ip add of machine on which server is running
     serv_addr.sin_port = htons(portno); //convert port no to network byte order

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     while (1) {

          cout <<"Waiting for client";

         newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr , &clilen);
         cl=inet_ntoa(cli_addr.sin_addr);
        
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             service(newsockfd,cl);
             delete cl;
             exit(0);
         }
         else close(newsockfd);
     } 
     close(sockfd);
     return 0; 
}

void service (int sock,char *cl){
   
   while(1){
      ofstream outfile;
      int i,n,len1;
      char buffer[256];
      char path[256];
      char data[256];
      char name[256];
      
      bzero(buffer,256);
      bzero(path,256);
      bzero(name,256);
      bzero(data,256);

      n = read(sock,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      
      //find serarch or share    
      i=0;
      while(buffer[i]!='#'){
        data[i]=buffer[i];
        i++;
      }
      data[i]='\0';

      //if share 
      if(strcmp(data,"share")==0){
           
           logFile(1,cl);

          // extracting file name
          int len=strlen(buffer);
          i=len-1;
          while(buffer[i]!='/')
              i--;
          int k=0;
          for(i=i+1;i<len-1;i++){
             name[k]=buffer[i];
              k++;
          }
          name[k]='\0';

          //file path
          len1=strlen(buffer);
          for(i=6;i<len1;i++)
              path[i-6]=buffer[i];

          string nm(name);
          string pt(path);
          string str;
          str.append(nm);
          str.append(" ");
          str.append(cl);
          str.append(" ");
          str.append(pt);
         

          outfile.open("repo.txt", std::ios_base::app);
          outfile <<str;
          outfile.close();

          n = write(sock,"File Shared Successfully on Repo",32);
          if (n < 0) error("ERROR writing to socket");
      }
      else if(strcmp(data,"search")==0){

        string sendbuf1;

        logFile(2,cl);

        //to know position of #
        i=0;
        while(buffer[i]!='#'){
            data[i]=buffer[i];
            i++;
        }
        data[i]='\0';
        
        int k=0;
        int len=strlen(buffer);
        for(i=i+1;i<len-1;i++){
              name[k]=buffer[i];
              k++;
        }
        name[k]='\0';

         
        string nm(name);
        string word;
        string str3[3];
          
        vector<pair<string,string> > v1;
        vector<pair<string,string> > v2;
        map<string,vector<pair<string,string> > > mymap;

        ifstream Testfile ( "repo.txt" );
    
        k=0;
        while( Testfile ){  
            for(i=0;i<3;i++){
                Testfile>>word;
                str3[i]=word;
            }

            if(str3[0]!=""){
                v1.push_back(make_pair(str3[1],str3[2]));
                mymap[str3[0]].push_back(v1[k]);
                word="";
                k++;
            }
        }
        if(mymap.find(nm)==mymap.end()){
             n = write(sock,"0",1);
            if (n < 0) error("ERROR writing to socket"); 

        }
        else{
          if(mymap.find(nm)!=mymap.end()){
              v2=mymap[nm];

              for(vector<pair<string,string> >::iterator it1=v2.begin(); it1 != v2.end();it1++){
                  sendbuf1.append(it1->first);
                  sendbuf1.append("#");
                  sendbuf1.append(it1->second);
                  sendbuf1.append("#");


              }
         }
          char sendbuf11[1024];
          strcpy(sendbuf11,sendbuf1.c_str());
           
          n = write(sock,sendbuf11,strlen(sendbuf11));
          if (n < 0) error("ERROR writing to socket"); 
      
        }
      }
    }
}