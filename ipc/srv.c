//https://www.ibm.com/support/knowledgecenter/en/SSB23S_1.1.0.14/gtpc1/unixsock.html
/************************************************************/
/* This is a stream socket server sample program for UNIX   */
/* domain sockets. This program listens for a connection    */
/* from a client program, accepts it, reads data from the   */
/* client, then sends data back to connected UNIX socket.   */
/************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH  "tpf_unix_sock.server"
#define DATA "Hello from server"

int main(void){

    int server_sock, client_sock, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;     
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);       
    int call_cnt =0;         
    
    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1){
        printf("SOCKET ERROR: %d\n", 99);
        exit(1);
    }
    
    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SOCK_PATH); 
    len = sizeof(server_sockaddr);
    
    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc == -1){
        printf("BIND ERROR: %d\n", 99);
        close(server_sock);
        exit(1);
    }
    
    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
    char vic;

    while(vic!='q')
    {
        rc = listen(server_sock, backlog);
        // if (rc == -1){ 
        //     printf("LISTEN ERROR: %d\n", 99);
        //     close(server_sock);
        //     exit(1);
        // }
        printf("socket listening...cnt:%d\n",cnt);
        
        /*********************************/
        /* Accept an incoming connection */
        /*********************************/
        client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
        // if (client_sock == -1){
        //     printf("ACCEPT ERROR: %d\n", 99);
        //     close(server_sock);
        //     close(client_sock);
        //     exit(1);
        // }
        
        /****************************************/
        /* Get the name of the connected socket */
        /****************************************/
        // len = sizeof(client_sockaddr);
        // rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
        // if (rc == -1){
        //     printf("GETPEERNAME ERROR: %d\n", 99);
        //     close(server_sock);
        //     close(client_sock);
        //     exit(1);
        // }
        // else {
        //     printf("Client socket filepath: %s\n", client_sockaddr.sun_path);
        // }
        
        /************************************/
        /* Read and print the data          */
        /* incoming on the connected socket */
        /************************************/
        // printf("waiting to read...\n");
        bytes_rec = recv(client_sock, buf, sizeof(buf), 0);
        // if (bytes_rec == -1){
        //     printf("RECV ERROR: %d\n", 99);
        //     close(server_sock);
        //     close(client_sock);
        //     exit(1);
        // }
        // else {
        //     printf("DATA RECEIVED = %s\n", buf);
        // }
        
        /******************************************/
        /* Send data back to the connected socket */
        /******************************************/
        memset(buf, 0, 256);
        strcpy(buf, DATA);    


    // puts ("Enter text. Include a dot ('.') in a sentence to exit:");

    // do {
    // vic=getchar();
    // putchar(vic);
    // } while (vic != '.');               // for loop execution

        // printf("Sending data...\n");
        rc = send(client_sock, buf, strlen(buf), 0);
        if (rc == -1) {
            printf("SEND ERROR: %d", 99);
            close(server_sock);
            close(client_sock);
            exit(1);
        }   
        else {

            // printf("Data sent! from: cnt: %d\n",call_cnt);
            call_cnt++;
        }
        
        /******************************/
        /* Close the sockets and exit */
        /******************************/

        close(client_sock);
    }

    close(server_sock);    
    return 0;
}
