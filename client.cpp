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
#include <limits>

using namespace std;
#define BUFFER_SIZE 1024
#define PORT_NUMBER 5437
string numStr = "127.0.0.1";
const char* num = numStr.c_str();

int rowSize = 10;
int colSize = 10;

void manual(){

    while(true){
        int row, col;

        //get row
        cout << "Please enter the row for the ticket you would like to buy!\n";
        cin >> row;
        if(!cin || row < 0 || row >= rowSize){
            cout << "That row does not exist please try again!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cout << endl;
        cout << row << endl;
    }
}
void automatic(){

}
void commandLine(int argc, char *argv[]){

    cout << "1\n";
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

    string word = argv[1];

    if(word == "manual"){
        cout << "manual\n";
        manual();
        exit(0);
    }
    if(word == "automatic"){

        cout << "automatic\n";
        automatic();
        exit(0);
    }
    cout << "\nYou did not enter manual or automatic....\n";
    cout << "Please try again with the correct appropiate specifier!\n\n";
    exit(0);

}
int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in serv_addr;
    //commandLine(argc, argv);

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(&serv_addr, '0', sizeof(serv_addr));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "\n Error : Could not create socket \n";
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER);


    if(inet_pton(AF_INET, num, &serv_addr.sin_addr) <= 0){
        cout << "\n inet_pton error occured\n";
        return 1;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
       cout << "\n Error : Connect Failed \n";
       return 1;
    }


    int var = 45;

    if(send(sockfd, &var, sizeof(var), 0) == -1){cout << "send error!\n";exit(0);}

    if(n < 0) { cout << "\n Read error \n";}

    close(sockfd);
    return 0;
}
