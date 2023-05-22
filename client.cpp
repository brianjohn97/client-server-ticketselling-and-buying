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
#include <fstream>
#include <vector>

using namespace std;
int PORT_NUMBER =  8080;
string numStr = "127.0.0.1";
const char* num = numStr.c_str();
int timeout = 5;

int rowSize = 10;
int colSize = 10;

int setup(){
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "\n Error : Could not create socket \n";
        exit(0);
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER);

    if(inet_pton(AF_INET, num, &serv_addr.sin_addr) <= 0){
        cout << "\n inet_pton error occured\n";
        exit(0);
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        sleep(timeout);
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            cout << "\nError: Tried connecting three times. Each failed!\n";
            cout << "Please try again later!\n";
            exit(0);
        }
    }
    return sockfd;
}
void automatic(){
    //creates the random seed
    srand(static_cast<unsigned int>(time(0)));

    //x is for when the server sends over a 1 indicating that all the tickets have been sold

    bool isRow = true;
    int row, n, x;
    int sockfd = setup();

    while(true){
        if(isRow){x = rand() % rowSize; isRow = false;}else{ x = rand() % colSize; isRow = true;}

        write(sockfd,&row, sizeof(row));
        if(n < 0) { cout << "\n Read error \n";}
        if(x == 1){ break;}
    }
}

void readFile(string fileName){

    ifstream file(fileName);
    string line;

    while(getline(file, line)){
        if(line.substr(0, 4) == "IP ="){
            numStr = line.substr(5);
        }else if(line.substr(0, 6) == "Port ="){
            PORT_NUMBER = stoi(line.substr(7));
        }else if(line.substr(0, 9) == "Timeout ="){
            timeout = stoi(line.substr(10));
        }
    }
    automatic();
}

void manual(){

    //x is for when the server sends over a 1 indicating that all the tickets have been sold

    bool isRow = true;
    int row, n, x;
    int sockfd = setup();
    while(true){

        if(isRow){cout << "\nEnter the X!: "; isRow = false; }else{cout << "\nEnter the Y!: "; isRow = true;}
        cin >> row;

        if(!cin || row < 0 || row >= rowSize){
            cout << "That row does not exist please try again!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if(!isRow){ isRow = true;}else { isRow = false;}
            continue;
        }
        write(sockfd,&row, sizeof(row));
        if(n < 0) { cout << "\n Read error \n";}
        if(x == 1){ break;}
    }
    close(sockfd);
}

void commandLine(int argc, char *argv[]){

    //have the server send over the size of the map


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
        if(argc == 2){
            automatic();
            exit(0);
        }
        readFile(argv[2]);
        exit(0);
    }
    cout << "\nYou did not enter manual or automatic....\n";
    cout << "Please try again with the correct appropiate specifier!\n\n";
    exit(0);

}
int main(int argc, char *argv[])
{

    commandLine(argc, argv);

    //manual();
    return 0;
}
void junk(){
    /*
int x = 0;
while(true){
    //if(send(sockfd, &x, sizeof(x), 0) == -1){cout << "send error!\n";}
    write(sockfd,&x, sizeof(x));
    cout << "Enter the X coordinate for the ticket you would like to purchase!";
    cin >> x;
    cout << x << endl;

    if(n < 0) { cout << "\n Read error \n";}
    sleep(2);
}*/
}
