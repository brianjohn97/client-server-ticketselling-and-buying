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
#include <pthread.h>
#include <mutex>
#include <sys/mman.h>
#include <limits>
#include <fcntl.h>
#include <random>
#include <iostream>
#include <vector>
#include <sys/shm.h>
#include <sys/types.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/mman.h>

using namespace std;

#define PORT_NUMBER 8080


pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
int initRow = 20;
int initCol = 20;
int total = initCol * initRow;
int blah = total * 2;
int row = 4;
int col = 4;
int totalTickets = row * col;
bool noTickets = false;


vector<vector<int> > map(100, vector<int>(100));


struct connection_t {
    int sock;
    struct sockaddr address;
    int addr_len;
};

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
    for(int i = 0; i< row;i++){
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
void initBoard() {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(100, 500);

    //create the board

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
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

void client(int connfd, int* board){


    int x = 1, y, temp, mark = 2, no = 0;
    bool check = true, check2 = false;

    //send over the size of the board
    write(connfd, &row, sizeof(row));
    write(connfd, &col, sizeof(col));


    //loop that continually accepts new variables for the coordinates
    //updates the ticket map and checks if there are any tickets left
    cout << "\nReady to receive variable from conndfd: " << connfd << "\n\n";
    while(1){

        if (recv(connfd, &temp, sizeof(temp), 0)  < 0) {
            cout << "receiver error\n";
            exit(0);
        }
        if(check){
            x = temp;
            check = false;
            continue;
        }else{
            y = temp;
            check = true;
        }
        if(check){

            cout << "x: " <<x << "     y: " << y << endl;
            for (int i = 0, j = i+1; i < blah; ++i) {
                if(board[i] == x && board[j] == y){
                    cout << "ALready taken!\n";
                    if(write(connfd, &mark, sizeof(mark)) < 0){cout << "Lost connection\n";exit(0);}
                    check2 = true;
                    break;
                }
            }
            if(check2){check2 = false; continue;}
            cout << "updating ticket information at location ["<<x<<"]["<<y<<"]! \n";
            map[x][y] = -1;
            printBoard();
            for (int i = 0, j= i+1; i < blah; ++i, j++) {
                if(board[i] == -1){
                    board[i] = x;
                    board[j] = y;
                    break;
                }
            }
        }
        if (areThereTickets()){cout << "There are no more Tickets! Sorry!\n\n"; noTickets = true;
            if(write(connfd, &x, sizeof(x)) < 0){cout <<"lost connection\n";exit(0);}
            break;
        }
        if(write(connfd, &no, sizeof(no)) < 0){cout << "lost connection\n";exit(0);}
        for (int i = 0; i < 10; ++i) {
            cout << board[i] << endl;
        }
    }
    if(write(connfd, &x, sizeof(x)) < 0){cout << "Lost connection\n";exit(0);}
    close(connfd);

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

    //getSizeOfMap(argc, argv);

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
    int * board = static_cast<int*>(mmap(NULL, sizeof(int)*10, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0));


    //initializes the board with -1s
    for(int i=0; i< blah;i++){
        board[i] = -1;
    }

    initBoard();
    int listenfd = setup();

    /*
    int pid = fork();
    if(pid == 0){
        cout << "I am in the child\n";
        for(int i = 0; i < 10; i++)
        {
            ClassicalArray[i] = i;
            board[i]  = 10 -i;
        }
        printf("\nI am done in the child\n");
        exit(0);
    }else{
        wait(NULL);
        printf("I am in the parent\n");
        //print the classical array
        for(int i = 0; i < 10; i++)
            printf("%d\t", ClassicalArray[i]);
        printf("\n");
        //print the shared array
        for(int i = 0; i < 10; i++)
            printf("%d\t", board[i]);
        printf("\nI am done in the parent\n");
    }*/



    while(true){
        //prints the current state of the ticket map and accept connections
        printBoard();

        //will wait for a connection
        cout << "Waiting for client/buyer on port: " << PORT_NUMBER << "!\n";
        fflush(stdout);
        int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        //client(connfd);
        int pid = fork();


        if(pid == 0){
            //cout << "child!\n";
            client(connfd, board);
        }else{
            //cout << "parent\n";
        }

        if(noTickets){break;}


        //close(connfd);
    }
    munmap(board,sizeof(int)*blah);
    map.clear();
    close(listenfd);
}
