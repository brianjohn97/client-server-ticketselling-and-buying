#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <math.h>
#include <iostream>
#include <mutex>
#include <sys/mman.h>
#include <limits>
#include <fcntl.h>
#include <random>
#include <sys/shm.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>


using namespace std;

#define PORT_NUMBER 8080

int row = 4;
int col = 4;
int totalTickets = row * col;
bool noTickets = false;

//prints the current state of the board
void printBoard(int *map){

    //converts the 1d array to a 2d one
    int convert[row][col];
    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++){
            convert[i][j] = map[i * col + j];
        }
    }

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
    for(int i = 0; i< row;i++){
        for (int j=0;j<col;j++){
            if(convert[i][j] == -1){
                cout << "[ "  << convert[i][j] << "  ]\t";
                continue;}
            cout << "[ "  << convert[i][j] << " ]\t";
        }
        cout << endl;
    }
    cout << endl;
}

// initializes the board with random ticket price value
void initBoard(int *map) {
    //random num generator
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(100, 500);

    //create the board
    int totalEle = row * col;
    for (int i = 0; i < totalEle; i++) {
        map[i] = dist(gen);
    }
}

//gets the size of the map from the user and checks for anything that would cause errors
void getSizeOfMap(int argc, char *argv[], int *board){

    //check if there are too many or too little arguments
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
    //initialize the board
    totalTickets = row * col;
    initBoard(board);
    
}

//checks if there are tickets in the map and returns bool
bool areThereTickets(int *map){
    int totalEle = row * col;

    int counter = 0;
    for (int i = 0; i < totalEle; ++i) {
        if(map[i] != -1){counter ++;}
    }
    if(counter != 0){ return false;}
    return true;
}

//handles the clients
void client(int connfd, int* map){

    int x = 1, y, temp, mark = 2, no = 0, id = connfd -5;
    bool isX = true;

    //send over the size of the board
    write(connfd, &row, sizeof(row));
    write(connfd, &col, sizeof(col));


    //loop that continually accepts new variables for the coordinates
    //updates the ticket map and checks if there are any tickets left
    cout << "\nReady to receive ticket information from client: " << id << "\n\n";

    //prints the current state of the ticket map and accept connections
    printBoard(map);
    while(1){

        if(map[400] == 1){no =1; write(connfd, &no, sizeof(x)); exit(0);}
        //receives the coordinates
        if (recv(connfd, &temp, sizeof(temp), 0)  < 0) {
            cout << "receiver error\n";
            exit(0);
        }
        //determines if its an x or y
        if(isX){
            x = temp;
            isX = false;
            continue;
        }else{
            y = temp;
            isX = true;
        }
        if(isX){
            //checks if that ticket has already been bought
            //if so then send message to client letting them know
            if(map[x * col + y] == -1){
                if(write(connfd, &mark, sizeof(mark)) < 0){
                    cout << "Lost connection\n";exit(0);
                }
                continue;
            }

            //purchases the ticket
            cout << "client: "<< id <<" purchased ticket ["<<x<<"]["<<y<<"]! \n";
            map[x * col + y] = -1;

            //prints current state of tickets and lets clients know where they can connect
            cout << "Waiting for client/buyer on port: " << PORT_NUMBER << "!\n\n";
            printBoard(map);
        }

        //if there are no tickets it prints it to the server and lets the client know
        if (areThereTickets(map)){
            cout << "There are no more Tickets! Sorry!\n\n";
            noTickets = true;
            map[400] = 1;
            if(write(connfd, &x, sizeof(x)) < 0){cout <<"lost connection\n";exit(0);}
            break;
        }
        if(write(connfd, &no, sizeof(no)) < 0){cout << "lost connection\n";exit(0);}
    }
    if(write(connfd, &x, sizeof(x)) < 0){cout << "Lost connection\n";exit(0);}
    close(connfd);
}

//starts the server
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

    //creates the shared memory name and saves the size of it
    const char* name = "/my_shared_memory";
    const int SIZE = sizeof(int)*10;

    //maps out a location for the memory and returns error if its not able to do so
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(fd == -1){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(fd, SIZE) == -1){
        perror("ftruncate");
        exit(1);
    }

    //creates shared memory obj and cast it to the map variable
    int * map = static_cast<int*>(mmap(NULL, sizeof(int)*10, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0));

    getSizeOfMap(argc, argv, map);

    // knows when to quit
    map[400] = -1;

    int listenfd = setup();
    int i = 1, pid;
    printBoard(map);
    while(!noTickets){

        //will wait for a connection
        if(i == 1){
            cout << "\nWaiting for client/buyer on port: " << PORT_NUMBER << "!\n";
            i = 2;
        }

        //accepts the connection forks a process
        //hsa the parent wait for another connection while the child deals with the current connection
        fflush(stdout);
        int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        pid = fork();
        if(pid == 0){
            client(connfd, map);
        }
    }
    //close out the shared memory then wait for all the clients to try
    //one last time before killing the process
    if(munmap(map,sizeof(int)*(row*col)) == -1){
        perror("munmap");
        exit(1);
    }
    if(close(fd) == -1){
        perror("close");
        exit(1);
    }
    close(listenfd);
    sleep(10);
    kill(pid, SIGKILL);
    return 0;
}
