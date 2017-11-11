/*
Yojana Birje
20162130
*/
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<iostream>
#include<vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include<map>
#include<vector>
#include <vector>


void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void service(int sock2)
{
        char buffer[256];
        char buffer1[256];
        int n;

         bzero(buffer,256);
         n = read(sock2,buffer,255);
         if (n < 0) 
            error("ERROR reading from socket");  
       
         
         ifstream fp;
         fp.open(buffer,std::ifstream::in);
         while(fp.read(buffer,1))
         {
            write(sock2,buffer,1);
            bzero(buffer,256);
         }

}
void logFile(int n,string nm2)
{ 
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  
  ofstream outfile1;
  outfile1.open("client.log", std::ios_base::app);

  if(n==1)
  {
    outfile1<<"Download Request from"<<" "<<nm2<<" "<< asctime(timeinfo);
  }

  if(n==2)
  {
    outfile1<<"File sent to"<<" "<<nm2 <<" "<< asctime(timeinfo);
  }
   outfile1.close();

}


void downloadServer(){
     int sockfd2, newsockfd2, portno2;
     int n,pid2;
     socklen_t clilen2;
     struct sockaddr_in serv_addr2, cli_addr2;
     char buffer[256];
     char buffer1[256];

     sockfd2 = socket(AF_INET, SOCK_STREAM, 0);  //create socket
     if (sockfd2 < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr2, sizeof(serv_addr2)); //initialise buffer to zero
     
     portno2 = 8050;
     serv_addr2.sin_family = AF_INET; //address family

     serv_addr2.sin_addr.s_addr = INADDR_ANY; // Ip add of machine on which server is running
     serv_addr2.sin_port = htons(portno2); //convert port no to network byte order

     if (bind(sockfd2, (struct sockaddr *) &serv_addr2,
              sizeof(serv_addr2)) < 0) 
              error("ERROR on binding");

     listen(sockfd2,5);
     clilen2 = sizeof(cli_addr2);

     while (1) {

         newsockfd2 = accept( sockfd2, (struct sockaddr *) &cli_addr2 , &clilen2);

         if (newsockfd2 < 0) 
             error("ERROR on accept");

         pid2 = fork();
         if (pid2 < 0)
             error("ERROR on fork");
         if (pid2 == 0)  {
             close(sockfd2);
             service(newsockfd2);
             //delete cl;
             exit(0);
         }
         else close(newsockfd2);
     }  
}

 

int main(int argc, char *argv[])
{

    int pid=fork();
    
        if(pid==0){
            downloadServer();
        }
        else
        {
        
    
            int sockfd, portno, n,sockfd1,portno1;
            struct sockaddr_in serv_addr,serv_addr1;
            struct hostent *server,*server1;
            char str1[]={'s','h','a','r','e','#'};
            char str2[]={'s','e','a','r','c','h','#'};

            char buffer[256];
            char temp[256];
            char name[256];
            char recvbuf1[1024];
            char recvbuf2[1024];
            char recvbuf3[1024];
            ofstream fp;
            int i,k,choice;
            char nmArr[256],ipaddr[256];
            vector<string> v;
            string s1,nm,nm1;
    
            if (argc < 3) {
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
            }
            portno = atoi(argv[2]);
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) 
                error("ERROR opening socket");
            server = gethostbyname(argv[1]);
            if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);
                serv_addr.sin_port = htons(portno);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
                error("ERROR connecting");

             while(1){
                cout << "\n1.Search\n2.Share\n3.Exit ";
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                switch(buffer[0]){
                    case'1': cout << "Type string to search: ";
                            bzero(buffer,256);
                            fgets(buffer,255,stdin);
                            i=0;
                            while(i<strlen(buffer)){
                                name[i]=buffer[i];
                                i++;
                            }
                            name[i]='1';
                            name[i+1]='\0';
                            bzero(temp,256);
                            strcat(temp,str2);
                            strcat(temp,buffer);
                            n=write(sockfd,temp,strlen(temp));
                            if (n < 0) 
                                error("ERROR writing to socket");   


                            bzero(recvbuf1,1024);
                            n = read(sockfd,recvbuf1,1024);
                            if (n < 0) 
                                error("ERROR reading from socket"); 
                            v.clear();

                            if(recvbuf1[0]=='0'){
                              cout<<"NO such String ";
                              break;
                            }
                  
                            k=0; 
                           for(i=0;i<n;i++){
                                if(recvbuf1[i]=='#'){
                                    v.push_back(s1);
                                    s1="";
                                    k++;
                                }
                                else{
                                    s1=s1+recvbuf1[i];
                                }
                            }
                            i=1;
                            for(vector<string> :: iterator it1=v.begin();it1!=v.end();it1=it1+2){
                                cout <<i;
                                cout<<" ";
                                i++;
                                cout<< *it1 <<endl;
                            }
                            cout <<">> ";
                            cin>>choice;
                            if(choice>=i){
                              cout<<"Invalid choice";
                              break;
                            }
                            nm1=v[((choice-1)*2)];
                            nm=v[(((choice-1)*2)+1)];
                            logFile(1,nm1);

                            strcpy(ipaddr,nm1.c_str());

                            portno1 = 8050;
                            sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
                            if (sockfd1 < 0) 
                                error("ERROR opening socket");
                            server1 = gethostbyname(ipaddr);
                            if (server1 == NULL) {
                                fprintf(stderr,"ERROR, no such host\n");
                                exit(0);
                            }

                            bzero((char *) &serv_addr1, sizeof(serv_addr1));
                            serv_addr1.sin_family = AF_INET;
                            bcopy((char *)server1->h_addr, 
                            (char *)&serv_addr1.sin_addr.s_addr,
                            server1->h_length);
                            serv_addr1.sin_port = htons(portno1);
                            if (connect(sockfd1,(struct sockaddr *) &serv_addr1,sizeof(serv_addr1)) < 0) 
                                error("ERROR connecting");


                            bzero(buffer,256);
                            
                
                            strcpy(nmArr,nm.c_str());
                            n=write(sockfd1,nmArr,strlen(nmArr));
                            if (n < 0) 
                                error("ERROR writing to socket"); 

                            fp.open(name,std::ifstream::out);
                            while(read(sockfd1,buffer,1))
                            {
                                fp.write(buffer,1);
                                bzero(buffer,256);
                            }
                            fp.close();
                            cout << "Download Complete" <<endl;
                            logFile(2,nm1);               
                            break;

                    case'2': cout << "\nPath:";
                            bzero(buffer,256);
                            fgets(buffer,255,stdin);
                            bzero(temp,256);
                            strcat(temp,str1);
                            strcat(temp,buffer);
                            //cout << temp;
                            n=write(sockfd,temp,strlen(temp));
                            if (n < 0) 
                                error("ERROR writing to socket");
                            bzero(buffer,256);
                            n = read(sockfd,buffer,255);
                            if (n < 0) 
                                error("ERROR reading from socket");
                            cout << buffer;
                            cout <<endl;
                            break;

                    case'3': n=write(sockfd,"3",strlen(temp));
                            if (n < 0) 
                                error("ERROR writing to socket");

                            _exit(0);

                    default: cout << "Wrong option";
                             break;

                }
            }
    
            close(sockfd);
        }
    return 0;
}
