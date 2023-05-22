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

using namespace std;

#define PI 4.0*atan(1.0)
#define BUFFER_SIZE 1024
#define PORT_NUMBER 8080


pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int initRow = 100;
int initCol = 100;
int row = 2;
int col = 2;
int totalTickets = row * col;
bool noTickets = false;

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

void blah(int connfd){
    int receivedVariable;
    cout << "Ready to receive varaible!\n";
    while(1){
        if (recv(connfd, &receivedVariable, sizeof(receivedVariable), 0) == -1) {
            cout << "receiver error\n";
            exit(0);
        }
        cout << "\nreceived variable: " << receivedVariable << endl;
    }

}

void * client(void * arg){
    int receivedVariable;
    cout << "Ready to receive varaible!\n";
    int x, y;
    while(1){
        for (int i = 0; i < 2; ++i) {
            /*
            if (recv(connfd, &receivedVariable, sizeof(receivedVariable), 0) == -1) {
                cout << "receiver error\n";
                exit(0);
            }
            cout << "\nreceived variable: " << receivedVariable << endl;
            if (i == 0){x = receivedVariable; continue;}
            y = receivedVariable;
             */
        }
        pthread_mutex_lock(&myMutex);
        if(map[x][y] == -1){cout << "That ticket has already been sold! Sorry!\n"; pthread_mutex_unlock(&myMutex); continue; }
        map[x][y] = -1;
        totalTickets--;
        pthread_mutex_unlock(&myMutex);
    }
}

void updateTickets(int x, int y){
    cout << "updating ticket information at location ["<<x<<"]["<<y<<"]! \n";
    map[x][y] = -1;
}

bool areThereTickets(){
    int counter = 0;
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if(map[i][j] != -1){counter ++;}
        }
    }
    if(counter != 0){ return false;}
    return true;
}

void client(int connfd){
    int row, col, temp;
    bool check = true;

    while(1){
        cout << "Ready to receive varaible!\n";
        if (recv(connfd, &temp, sizeof(temp), 0) == -1) {
            cout << "receiver error\n";
            exit(0);
        }
        if(check){
            row = temp;
            cout << "\nRow: " << row << endl;
            check = false;
            continue;
        }else{
            col = temp;
            cout << "Column: " << row << endl <<  endl;
            check = true;
        }
        if(check){
            updateTickets(row, col);
            printBoard();
        }
        if (areThereTickets()){cout << "There are no more Tickets! Sorry!\n"; noTickets = true; break;}
    }
}

int setup(){
    int listenfd = 0, connfd = 0, n = 0, sockfd = 0;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {cout << "socket error!\n"; exit(0);}

    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cout << "Failed to set SO_REUSEADDR option. " << strerror(errno) << "\n";
        return 1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0){
        std::cout << "Failed to set SO_REUSEPORT option. " << strerror(errno) << "\n";
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT_NUMBER);

    if((bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        close(listenfd);
        cout << "Bind Error!\n" << strerror(errno); exit(0);}

    if((listen(listenfd, 10)) < 0){cout << "Listen Error!\n"; exit(0);}

    return listenfd;
}

int main(int argc, char *argv[]){
    //creates the random seed
    srand(static_cast<unsigned int>(time(0)));
    //getSizeOfMap(argc, argv);
    initBoard();
    int listenfd = setup();


    while(true){
        //prints the current state of the ticket map and accept connections
        printBoard();

        //will wait for a connection
        cout << "Waiting for client/buyer on port: " << PORT_NUMBER << "!\n";
        fflush(stdout);
        int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        client(connfd);
        if(noTickets){break;}

        //sprintf(sendBuff,"Message received from the server: \t%d\t%f\n",121,PI);

        //write(connfd, sendBuff, strlen(sendBuff));

        //close(connfd);
    }
    map.clear();
    close(listenfd);
}
