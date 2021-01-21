#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include<time.h>

#define PACKET_SIZE 100 //Max length of buffer
#define PORT 8882       //The port on which to listen for incoming data
#define PDR 10

void die(char *s)
{
    perror(s);
    exit(1);
}
int max(int a, int b);
typedef struct packet
{
    int size;
    int sq_no;
    bool lastpacket;
    bool isack;
    int channelid;
    char data[PACKET_SIZE];
} DATA_PKT;
