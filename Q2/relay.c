#include"packet.h"


int totalpkts;
bool todrop(int num);
int main(void)
{
    char newch[30];

    printf("NODE_NAME           EVENT_TYPE          TIMESTAMP               PACKET_TYPE         SEQ.NO      SRC         DEST\n");

    totalpkts=0;
    struct pollfd pfds[2];
    struct pollfd pfds2[1];
    struct sockaddr_in myAddr[3], si_other,clientAddr;
    int sock[3], i, slen = sizeof(si_other) , recv_len;
    //char buf[BUFLEN];
    DATA_PKT rcv_pkt,rcv_ack;
    //create a UDP socket
    if ((sock[0]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket()");
    }
      if ((sock[1]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket()");
    }
      if ((sock[2]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket()");
    }
    // zero out the structure
    memset((char *) &myAddr[0], 0, sizeof(myAddr[0]));
     
    myAddr[0].sin_family = AF_INET;
    myAddr[0].sin_port = htons(PORT1);
    myAddr[0].sin_addr.s_addr = htonl(INADDR_ANY);

     memset((char *) &myAddr[1], 0, sizeof(myAddr[1]));
     
    myAddr[1].sin_family = AF_INET;
    myAddr[1].sin_port = htons(PORT2);
    myAddr[1].sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *) &myAddr[2], 0, sizeof(myAddr[2]));
     
    myAddr[2].sin_family = AF_INET;
    myAddr[2].sin_port = htons(PORT5);
    myAddr[2].sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT3);
	si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *) &clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(PORT0);
	clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int clen;
    clen=sizeof(clientAddr);
    int serverlen;
    serverlen=sizeof(si_other);
    //bind socket to port
    if( bind(sock[0] , (struct sockaddr*)&myAddr[0], sizeof(myAddr[0]) ) == -1)
    {
        die("bind");
    }
    if( bind(sock[1] , (struct sockaddr*)&myAddr[1], sizeof(myAddr[1]) ) == -1)
    {
        die("bind");
    }
    if( bind(sock[2] , (struct sockaddr*)&myAddr[2], sizeof(myAddr[2]) ) == -1)
    {
        die("bind");
    }
    int state =0;
    int current;
    int serverpoll;
    int exits;
    exits=1;
    while(exits)
    {
       
    pfds[0].fd = sock[0];
    pfds[0].events = POLLIN;

    pfds[1].fd = sock[1];
    pfds[1].events = POLLIN;

    pfds2[0].fd=sock[2];
    pfds2[0].events=POLLIN;
    
    //printf("hasn't comeout yet \n");
    fork();
    current=poll(pfds,2,-1);
    //printf("cameout\n");
    int recentlydropped=0;
    if(current!=0){
    for(int j=0;j<2;j++){
        
        if(pfds[j].revents==POLLIN){
            if(j==0){
                strcpy(newch,"RELAY1");
            }
            else{
                strcpy(newch,"RELAY2");

            }
            
            //printf("DATA came here\n\n");
            if (recvfrom(sock[j], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *) &myAddr[j], &slen) == -1)
            {
            die("recvfrom()");
            }
            if(todrop(rcv_pkt.sq_no)&&rcv_pkt.sq_no>2*WINDOW_SIZE){
                //printf("DROPPING PACKET with sequence number : %d\n\n",rcv_pkt.sq_no);
                printf("%s            D              %-15s             DATA                %2d        CLIENT          %s\n\n",newch,get_time(),rcv_pkt.sq_no,newch);   
                break;
            }
            // if(recentlydropped==rcv_pkt.sq_no){
            //     if(todrop(recentlydropped*23));
            //         break;
            //     }
            // }
            printf("%s            R              %-15s             DATA                %2d        CLIENT          %s\n\n",newch,get_time(),rcv_pkt.sq_no,newch);
            //printf("Received data is %s with seq no %d  \n",rcv_pkt.data,rcv_pkt.sq_no);
            strcpy(rcv_ack.data,"got ack pkt from server \n\n");
            rcv_ack.sq_no=rcv_pkt.sq_no;
            // if (sendto(sock[j], &rcv_ack, sizeof(rcv_ack), 0 , (struct sockaddr *) &myAddr[j], slen)==-1)
            // {
            // die("sendto()");
            // }
            float quant;
            int comm;
            srand(rcv_ack.sq_no);
            comm=rand();
            comm=comm%3;
            quant=(float)comm;
            if(rcv_ack.sq_no>PACKET_SIZE){
            sleep(quant/1000);
            }
            rcv_pkt.channelid=j;
            if (sendto(sock[2], &rcv_pkt, sizeof(rcv_pkt), 0 , (struct sockaddr *) &si_other, serverlen)==-1)
            {
            die("sendto()");
            }
            printf("%s            S              %-15s             DATA                %2d        %s          SERVER\n\n",newch,get_time(),rcv_pkt.sq_no,newch);

            if(rcv_pkt.lastpacket==true){
                //printf("END OF LOGBOOK\n");
                //wait(0);
                exits=0;
                //return 0;

                exit(0);
            }
        }
    }
    }
            serverpoll=poll(pfds2,1,100);
            if(pfds2[0].revents=POLLIN){
                if (recvfrom(sock[2], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *) &myAddr[2], &slen) == -1)
            {
            die("recvfrom()");
            }
             if(rcv_pkt.channelid==0){
                strcpy(newch,"RELAY1");
            }
            else{
                strcpy(newch,"RELAY2");
            }
            printf("%s            R              %-15s             ACK                %2d        SERVER          %s\n\n",newch,get_time(),rcv_pkt.sq_no,newch);
            //printf("Received ack with sequence number %d \n",rcv_pkt.sq_no);
            rcv_ack.sq_no=rcv_pkt.sq_no;
            if(rcv_pkt.lastpacket==true){
                exit(0);
            }

            rcv_ack.channelid=rcv_pkt.channelid;
            if (sendto(sock[2], &rcv_ack, sizeof(rcv_ack), 0 , (struct sockaddr *) &clientAddr, clen)==-1)
            {
            die("sendto()");
            }
            //printf("Sent done");
            printf("%s            S              %-15s             ACK                %2d        %s          SERVER\n\n",newch,get_time(),rcv_ack.sq_no,newch);
            
             }
            // printf("Data sent to client\n");

    }
    close(sock[0]);
    close(sock[1]);
    return 0;
}
bool todrop(int num){
    srand(time(NULL));
    int new;
    new=rand();
    new=new%100;
    //printf("Random Number is : %d\n",new);
    if(new<=PDR){
        return true;
    }else{
        return false;
    }

}