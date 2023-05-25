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
#include <random>
#include <algorithm>
#include <string>

using namespace std;

//global variables
int PORT_NUMBER =  8080;
string numStr = "127.0.0.1";
int timeout = 5;

int rowSize = 1;
int colSize = 1;

//starts the client and connections to the server
int setup(){
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "\n Error : Could not create socket \n";
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER);
    serv_addr.sin_addr.s_addr = inet_addr(numStr.c_str());


    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        sleep(timeout);
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            cout << "\nError: waited " << timeout << "seconds and could not connect\n";
            cout << "Please try again later!\n";
            exit(0);
        }
    }
    return sockfd;
}

//automatically buys random tickets
void automatic(){
    //creates the random seed
    random_device rd;
    mt19937 gen(rd());
    bool isRow = true;
    int row, n, x, temp, col;
    int sockfd = setup();

    //receives the size of the board to make sure we don't enter an incorrect size
    recv(sockfd, &rowSize, sizeof(rowSize), 0);
    recv(sockfd, &colSize, sizeof(colSize), 0);

    //function for generating random functions
    uniform_int_distribution<int> distX(0, rowSize-1);
    uniform_int_distribution<int> distY(0, colSize-1);

    while(true){
        //gets the correct random number
        if(isRow){row = distX(gen); isRow = false;}else{ row = distY(gen); isRow = true;}
        if(isRow){col = row;}

        //sends it to the server
        write(sockfd,&row, sizeof(row));

        //checks for error
        if(n < 0) { cout << "\n Read error \n";}
        if(x == 1){ break;}
        if(isRow){
            //receives int from server to see if the ticket was already bought
            recv(sockfd, &x, sizeof(x), 0);
            if(x == 1){
                cout << "\nThe server is unfortunately out of tickets! \n";
                cout << "Better luck next time!\n\n";
                break;
            }else if( x == 2){ continue;}
            cout << "\nSuccessfully purchased ticket for seat["<<temp<<"]["<<col<<"]\n";
            sleep(3);
        }
        temp = row;
    }
}

//reads the given file for ip address, port number and timeout
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

//lets the user pick which tickets that they want
void manual(){
    bool isRow = true;
    int row, n, x, y, col, temp;
    int sockfd = setup();

    //receives the size of the board to make sure we don't enter an incorrect size
    recv(sockfd, &rowSize, sizeof(rowSize), 0);
    recv(sockfd, &colSize, sizeof(colSize), 0);

    while(true){
        //takes in an int from user and check it
        if(isRow){cout << "\nEnter the X!: "; isRow = false; }else{cout << "Enter the Y!: "; isRow = true;}

        cin >> row;
        if(isRow){
            col = row;
            if(!cin || col < 0 || col >= colSize){
                cout << "That col does not exist please try again!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if(!isRow){ isRow = true;}else { isRow = false;}
                continue;
            }

        }else {
            if (!cin || row < 0 || row >= rowSize) {
                cout << "That row does not exist please try again!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (!isRow) { isRow = true; } else { isRow = false; }
                continue;
            }
        }

        //sends that int to the server and checks for errors
        write(sockfd,&row, sizeof(row));
        if(n < 0) { cout << "\n Read error \n";}
        if(x == 1){ break;}

        if(isRow){
            //receives int from user if ticket is already taken or if the server ran out of tickets
            recv(sockfd, &x, sizeof(x), 0);
            if(x == 1){
                cout << "\nThe server is unfortunately out of tickets! \n";
                cout << "Better luck next time!\n\n";
                break;
            }else if (x == 2){
                cout << "\nThat ticket is already taken!\n";
                cout << "Please try again with a ticket that has not already been taken!\n";
                continue;
            }
            cout << "\nSuccessfully purchased ticket for seat["<<temp<<"]["<<col<<"]\n";
        }
        temp = row;
    }
    close(sockfd);
}

//gets the ini file from user on whether they want manual or automatic
void commandLine(int argc, char *argv[]){

    //no specifier
    if(argc < 2){
        cout << "\nYou forgot to specify whether the client should be in automatic or manual mode!\n";
        cout << "Try again with either mode specified!\n\n";
        exit(0);
        
    }
    //too many
    if(argc > 3){
        cout << "\nThere are too many specifiers!\n";
        cout << "There only should be 2! One of for the mode and another for the optional ini file!\n";
        cout << "Please try again with the appropiate specifiers!\n\n";
        exit(0);
    }

    string word = argv[1];

    //determines which function to call
    if(word == "manual"){
        manual();
        exit(0);
    }
    if(word == "automatic"){
        if(argc == 2){
            automatic();
            exit(0);
        }
        cout << "reading ip address and port number from file!\n";
        readFile(argv[2]);
        exit(0);
    }

    //lets user know they did not enter it in correctly
    cout << "\nYou did not enter manual or automatic....\n";
    cout << "Please try again with the correct appropiate specifier!\n\n";
    exit(0);

}
int main(int argc, char *argv[]){

    //calls command line function to get started
    commandLine(argc, argv);
    return 0;
}

