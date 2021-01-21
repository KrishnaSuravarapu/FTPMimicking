/* Simple udp server with stop and wait functionality */

#include"packet.h"
char buffer[100/PDR][PACKET_SIZE];
int totalpkts;
int main(void)
{
    FILE* fp=fopen("output.txt","w");
    fd_set rfds;
    totalpkts = 0;
    printf("SERVER TRACE\n\n");
    printf("TYPE            SEQ.NO          SIZE        CHANNEL\n\n");
    struct sockaddr_in serverAddr, clientAddr1, clientAddr2;
    int s, i, slen = sizeof(clientAddr1), recv_len;
    struct timeval tv;
    //char buf[PACKET_SIZE];
    DATA_PKT rcv_pkt, ack_pkt;
    //create a TCP socket
    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        die("socket");
    }
    // zero out the structure
    memset((char *)&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(s, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        die("socket");
    }
    int temp1 = listen(s, 5);
    if (temp1 < 0)
    {
        printf("Error in listening");
        exit(0);
    }
    //printf("Now Listening\n");

    /*ACCEPTING CONNECTION*/
    int clientLength = sizeof(clientAddr1);
    int s1;
    s1 = accept(s, (struct sockaddr *)&clientAddr1, &clientLength);
    if (clientLength < 0)
    {
        printf("Error in client socket");
        exit(0);
    }
    //printf("connection 2 address is %d\n",clientAddr1.sin_port);

    int clientLength2 = sizeof(clientAddr2);
    int s2 = accept(s, (struct sockaddr *)&clientAddr2, &clientLength2);
    if (clientLength2 < 0)
    {
        printf("Error in client socket");
        exit(0);
    }
    //printf("connection 2 address is %d\n",clientAddr2.sin_port);
    int state = 0;
    int sq_no = 0;
    int maxval = max(s1, s2);
    int handle;
    int skip1=0;
    int skip2=0;
    int index=0;
    for(int i=0;i<100/PDR+1;i++){
                memset(buffer[i],0,PACKET_SIZE);
                }
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(s1, &rfds);
        FD_SET(s2, &rfds);

        
        fflush(stdout);
        handle = select(maxval + 1, &rfds, NULL, NULL,NULL);
        //printf("handle value is --------------%d\n",handle);
        if (FD_ISSET(s1, &rfds))
        {
            //printf("Waiting for packet %d from sender...\n", sq_no);
            //printf("Total packets is %d\n",totalpkts);
            
            //try to receive some data, this is a blocking call
            if (recv_len = recv(s1, &rcv_pkt, sizeof(rcv_pkt), 0) < 0)
            {
                if(index!=0){
                    for(int i=0;i<index;i++){
                        fputs(buffer[i],fp);
                    }
                
                }
                die("recv()");
            }
            totalpkts++;
            
            if (totalpkts % (100 / PDR) == 0)
            {
                skip1=rcv_pkt.sq_no;
                //printf("\n\nskipping as total packets is %d\n\n", totalpkts);
                continue;
            }
            if(rcv_pkt.sq_no==skip1 && skip1!=0){
                //strcpy(buffer[0],rcv_pkt.data);
                skip1=0;
            }
         
            if(skip2 < rcv_pkt.sq_no && skip2!=0){
            strcpy(buffer[rcv_pkt.sq_no-skip2-1],rcv_pkt.data);
            //printf("Buffering S1 Packet received with seq. no. %d from line %d\n", rcv_pkt.sq_no, rcv_pkt.channelid);
            //printf("Packet data is :\n %s\n",rcv_pkt.data);
            //printf("skip2 is %d\n\n",skip2);
            index++;
            }
            else{
            //printf("S1 Packet received with seq. no. %d from line %d \n", rcv_pkt.sq_no, rcv_pkt.channelid);
            //printf("Packet data is :\n %s\n",rcv_pkt.data);
            fputs(rcv_pkt.data,fp);
            }

            if(skip1==0 && skip2==0){
                for(int i=0;i<100/PDR;i++){
                    
                fputs(buffer[i],fp);
                memset(buffer[i],0,PACKET_SIZE);
                }

                ////printf("Stop Buffer bruh \n");
                index=0;
            }
            ack_pkt.sq_no = rcv_pkt.sq_no;
            ack_pkt.channelid=rcv_pkt.channelid;
            sq_no = rcv_pkt.sq_no + 1;
            if (send(s1, &ack_pkt, sizeof(ack_pkt), 0) < 0)
            {
                die("send()");
            }
                        printf("SEND_ACK         %2d                NULL         %d\n", ack_pkt.sq_no,ack_pkt.channelid);

        }
       
        if (FD_ISSET(s2, &rfds))
        {
            //printf("Waiting for packet %d from sender...\n", sq_no);
            //printf("Total packets is %d\n",totalpkts);
            //try to receive some data, this is a blocking call
            if (recv_len = recv(s2, &rcv_pkt, sizeof(rcv_pkt), 0) < 0)
            {
                die("recv()");
            }
            printf("RECV_PKT         %2d                %ld          %d\n", rcv_pkt.sq_no,sizeof(rcv_pkt.data),rcv_pkt.channelid);

            totalpkts++;
            
            if (totalpkts % (100 / PDR) == 0)
            {
                skip2=rcv_pkt.sq_no;
                //printf("\n\nskipping as total packets is %d\n\n", totalpkts);
                continue;
            }
            if(rcv_pkt.sq_no==skip2 && skip2!=0){
               // strcpy(buffer[0],rcv_pkt.data);
                skip2=0;
            }
         
            if(skip1 < rcv_pkt.sq_no && skip1!=0){
            strcpy(buffer[rcv_pkt.sq_no-skip1-1],rcv_pkt.data);
            //printf("Buffering S2 Packet received with seq. no. %d from line %d \n", rcv_pkt.sq_no, rcv_pkt.channelid);
            //printf("Packet data is :\n %s\n",rcv_pkt.data);
            //printf("skip1 is %d\n\n",skip1);
            index++;

            }
            else{
            //printf("S2 Packet received with seq. no. %d from line %d \n", rcv_pkt.sq_no, rcv_pkt.channelid);
            //printf("Packet data is :\n %s\n",rcv_pkt.data);
            fputs(rcv_pkt.data,fp);
            }
            
            if(skip1==0 && skip2==0){
                for(int i=0;i<100/PDR;i++){
                fputs(buffer[i],fp);
                memset(buffer[i],0,PACKET_SIZE);
                }
                //printf("Stop Buffer bruh \n");
                index=0;
            }
            ack_pkt.sq_no = rcv_pkt.sq_no;
            ack_pkt.channelid=rcv_pkt.channelid;
            sq_no = rcv_pkt.sq_no + 1;
            if (send(s2, &ack_pkt, sizeof(ack_pkt), 0) < 0)
            {
                die("send()");
            }
            printf("SEND_ACK         %2d                NULL         %d\n", ack_pkt.sq_no,ack_pkt.channelid);
        }

        continue;
    }
    fclose(fp);
    close(s);
    close(s2);
    return 0;
}
int max(int i, int j)
{
    if (i >= j)
    {
        return i;
    }
    else
    {
        return j;
    }
}
