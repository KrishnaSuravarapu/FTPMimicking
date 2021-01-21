/* Simple udp server with stop and wait functionality */
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<time.h>
#include<sys/time.h>
#include <stdbool.h>
#include <sys/poll.h>
#include <sys/select.h>

#include<sys/poll.h>

#define PDR 10
#define BUFLEN 512  //Max length of buffer
#define PORT0 8880
#define PORT1 8882   //The port on which to send data
#define PORT2 8883
#define PORT3 8888
#define PORT5 8890
#define WINDOW_SIZE 4
#define BUFLEN 512 //Max length of buffer
#define PACKET_SIZE 100
void die(char *s)
{
    perror(s);
    exit(1);
}
 
typedef struct packet
{
    int size;
    int sq_no;
    bool lastpacket;
    bool isack;
    int channelid;
    char data[PACKET_SIZE];
} DATA_PKT;

char* get_time(){
    char * str=(char*) malloc(sizeof(char)*30);
    int rc;
    time_t current1;
    struct tm* timeptr;
    struct timeval tv2;
    current1=time(NULL);
    timeptr=localtime(&current1);
    gettimeofday(&tv2,NULL);
    rc=strftime(str, 30 , " %H : %M : %S",timeptr);
    char ms[8];
    sprintf(ms,".%06ld",tv2.tv_usec);
    strcat(str,ms);
    return str;
}