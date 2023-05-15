#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <string.h>
#include <iostream>

#define BUFFER_SIZE 1024
#define PORT_NUMBER 5437
using namespace std;
string num = "127.0.0.1";

void commandLine(int argc, char *argv[]){
    
    //fix error if user types in num
    string word= argv[1];

    if(argc < 2){
        cout << "\nYou forgot to specify whether the client should be in automatic or manual mode!\n";
        cout << "Try again with either mode specified!\n\n";
        exit(0);
        
    } 
    if(argc > 3){
        cout << "\nThere are too many specifiers!\n";
        cout << "There only should be 2! One of for the mode and another for the optional ini file!\n";
        cout << "Please try again with the appropiate specifiers!\n\n";
        exit(0);
    }
    if(word == "manual"){
        cout << "\nM A N U A L  M O D E !\n\n";
        exit(0);
    }
    if(word == "automatic"){
        cout << "\nA U T O M A T I C \n\n";
        exit(0);
    }
}
int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in serv_addr; 

    commandLine(argc, argv);

    


    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 


    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    close(sockfd);
    return 0;
}
