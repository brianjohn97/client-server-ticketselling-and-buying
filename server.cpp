#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <math.h>
#include <iostream>
#include <vector>

#define PI 4.0*atan(1.0)
#define BUFFER_SIZE 1024
#define PORT_NUMBER 5437
using namespace std;

int initRow = 100;
int initCol = 100;
int row = 10;
int col = 10;
int totalTickets = row * col;

vector<vector<int> > map(100, vector<int>(100));

//prints the current state of the board
void printBoard(){
    //print the numbers for the columns
    for (int i = 0; i < col; i++){
        if(i == 0){
            cout << "  " <<  i << "\t";
            continue;
        }
        cout << "  " <<  i << "\t";
    }
    cout << endl;

    //print out elements
    for(int i = 0; i< col;i++){
        for (int j=0;j<col;j++){
            if(map[i][j] == -1){
                cout << "[ "  << map[i][j] << "  ]\t";
                continue;}
            cout << "[ "  << map[i][j] << " ]\t";
        }
        cout << endl;
    }
    cout << endl;
}

// initializes the board with random ticket price value
void initBoard(){

    //create the board
    vector<string> temp;
    for(int i=0;i < row;i++){
        for (int j = 0; j < col; j++){
            //random ticket prices
            int ticketPrice = rand() % 400 + 100;
            map[i][j] = ticketPrice;
        }
    }
}

//gets the size of the map from the user and checks for anything that would cause errors
void getSizeOfMap(int argc, char *argv[]){

    //map size of 25 was the max amount of columns that could fit onto macbook air screen without any wrap around.

    if(argc > 3){
        cout << "\nThere are too many arguements for the map size of tickets!\n";
        cout << "Please enter a row size and a column size and thats it!\n";
        cout << "Alternatively you can NOT enter either and use the programs default size. \n\n";
        exit(0);
    }
    if(argc == 2){
        cout << "\nYou have provided the row size but not the column size! please try again with a column size!\n";
        cout << "Alternatively you can NOT enter either and use the programs default size. \n\n";
        exit(0);
    }
    if(argc == 3){
        try{
            row  = stoi(argv[1]);
            col = stoi(argv[2]);
        }
        catch(exception& err){
            cout << "The inputted row and column size are not integer!\n";
            cout << "Please re-enter them in appropriate integer sizes!\n";
            exit(0);
        }
    }
    totalTickets = row * col;
    initBoard();
    
}

int main(int argc, char *argv[]){
    //creates the random seed
    srand(static_cast<unsigned int>(time(0)));
    //getSizeOfMap(argc, argv);
    initBoard();
    printBoard();
    map.clear();

    /*
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[BUFFER_SIZE];
    char secMess[BUFFER_SIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    memset(secMess, '0', sizeof(secMess)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT_NUMBER); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        time_t t = time(NULL);
        struct tm  * tm = localtime (&t);
        char s[64];
        strftime(s,sizeof(s),"%c",tm);
        sprintf(sendBuff,"Message received from the server: %s\t%d\t%f\n",s,121,PI);
        sprintf(secMess,"This is a very special test!");

        
        
        write(connfd, sendBuff, strlen(sendBuff)); 
        write(connfd, secMess, strlen(secMess)); 

        close(connfd);
        sleep(1);
     }*/
}
