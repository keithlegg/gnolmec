//for socket demo 

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h> 


#include <sys/types.h> 
#include <netinet/in.h>

 

int TCP_PORT = 0; 


/*
Client:
    Create the socket.
    connect to it.
    recv data.
    If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 7.
    send response.
    Go to step 3.
    Stop.

Server:

    Create the socket.
    bind the socket to an address.
    Mark the socket as listening.
    accept a connection.
    If accepted connection is invalid, go to step 4.
    send data.
    recv response.
    If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 4 to accept a new connection.
    Go to step 6.
*/


//http://www.linuxhowtos.org/C_C++/socket.htm
void sockettest3(void) 
{ 
     int sockfd, newsockfd;

     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        printf("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(TCP_PORT);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              printf("ERROR on binding");

    
     bool stayopen = true;

     while(stayopen)
     {
         listen(sockfd,5);
         clilen = sizeof(cli_addr);
         newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);
         if (newsockfd < 0) 
              printf("ERROR on accept");
         bzero(buffer,256);

         n = read(newsockfd,buffer,255);
         
         //if (n < 0) printf("ERROR reading from socket");
         //printf("%i bytes received, message is %s\n", n, buffer);
     
         if(buffer[0]==0x00)
         {
             printf("NULL received, closing socket \n");            
             close(newsockfd);
             close(sockfd);
             stayopen = false;
         }

         if (strncmp(buffer, "vpfgrd_", 7) == 0)  
         {
             printf("vpfgrd_ command received!!\n ");            
               
         }

         


    }

}




/*
void sockettest(void) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Forcefully attaching socket to the port   
    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
    //                                              &opt, sizeof(opt))) 

    // Forcefully attaching socket to the port   
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port   
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

 
    valread = read( new_socket , buffer, 1); 
    printf("%s\n",buffer );

    

    //send(new_socket , hello , strlen(hello) , 0 ); 
    //printf("Hello message sent\n");

} 




//https://www.thegeekstuff.com/2011/12/c-socket-programming/
void sockettest2(void) 
{ 

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}
*/


