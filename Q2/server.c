#include"packet.h"
int lastwritten=0;
int main(void)
{
    printf("NODE_NAME           EVENT_TYPE          TIMESTAMP               PACKET_TYPE         SEQ.NO          SRC            DEST\n\n");
    FILE* fp = fopen("output.txt","w");
    DATA_PKT storedata[10*WINDOW_SIZE];
    bool gotdata[WINDOW_SIZE];
    int numbers[WINDOW_SIZE];
    int currentpacket = -1;
    struct pollfd pfds[1];
    struct sockaddr_in myAddr, si_other[3];
    int sock, i, slen = sizeof(si_other), recv_len;
    //char buf[BUFLEN];
    DATA_PKT rcv_pkt, rcv_ack;
    //create a UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket()");
    }
    // zero out the structure
    memset((char *)&myAddr, 0, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(PORT3);
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *)&si_other[0], 0, sizeof(si_other[0]));
    si_other[0].sin_family = AF_INET;
    si_other[0].sin_port = htons(PORT1);
    si_other[0].sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *)&si_other[1], 0, sizeof(si_other[1]));
    si_other[1].sin_family = AF_INET;
    si_other[1].sin_port = htons(PORT1);
    si_other[1].sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *)&si_other[2], 0, sizeof(si_other[2]));
    si_other[2].sin_family = AF_INET;
    si_other[2].sin_port = htons(PORT5);
    si_other[2].sin_addr.s_addr = inet_addr("127.0.0.1");

    int serverlen;
    serverlen = sizeof(si_other[0]);
    int starting=1;
    //bind socket to port
    if (bind(sock, (struct sockaddr *)&myAddr, sizeof(myAddr)) == -1)
    {
        die("bind");
    }
    int state = 0;
    int current;
    int contents=0;
    int bufferedmax=0;
    int index=0;
    for(int j=0;j<WINDOW_SIZE;j++){

        memset(storedata[j].data,0,PACKET_SIZE);
                }
    while (1)
    {
        pfds[0].fd = sock;
        pfds[0].events = POLLIN;
        current = poll(pfds, 1, -1);
        if (current != 0)
        {
            int j;
            j = rand() % 2;
            if (recvfrom(sock, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&myAddr, &slen) == -1)
            {
                die("recvfrom()");
            }
            char linel[20];
            if(rcv_pkt.channelid==0){
                strcpy(linel,"RELAY1");
            }
            else{
                strcpy(linel,"RELAY2");
            }
            printf("SERVER              R              %-15s             DATA                %2d        %s          SERVER\n\n",get_time(),rcv_pkt.sq_no,linel);
            if(starting==1){
                index=0;
                for(int j=0;j<WINDOW_SIZE;j++){
                    numbers[j]=j;
                    gotdata[j]=false;
                }
                starting=0;
            }
            if(rcv_pkt.sq_no-index<WINDOW_SIZE && rcv_pkt.sq_no!=index){
                //printf("BUFFERING\n");
                storedata[rcv_pkt.sq_no-index]=rcv_pkt;
                numbers[rcv_pkt.sq_no-index]=rcv_pkt.sq_no;
                gotdata[rcv_pkt.sq_no-index]=true;
                //printf("hi\n\n");
               contents++;
            }
            if(rcv_pkt.sq_no<index){
                continue;
            }
            if(rcv_pkt.sq_no==index){
                
                storedata[0]=rcv_pkt;
                gotdata[0]=true;
                contents++;
                int newint;
                while(gotdata[0]==true){
                newint=0;
                if(gotdata[0]==true){
                fwrite(storedata[newint].data,PACKET_SIZE,1,fp);
                lastwritten=storedata[newint].lastpacket;
                //printf("contents is %s:\n",storedata[newint].data);
                }
                
                while(newint<WINDOW_SIZE){
                if(gotdata[newint+1]==true)
                storedata[newint]=storedata[newint+1];
                gotdata[newint]=gotdata[newint+1];
                numbers[newint]=numbers[newint+1];
                newint++;
                }
                contents--;
                gotdata[newint-1]=false;
                numbers[newint-1]=numbers[newint-2]+1;
                index=numbers[0];
                //printf("----------------------------------------- numbers  is %d %d %d %d \n\n",numbers[0],numbers[1],numbers[2],numbers[3]);
                }
                if(index<0){
                    printf("Kindly RUn Again\n");
                    exit(0);
                    
                }
                //printf("NOW INDEX IS %d\n",index);
                //continue;
            }
            
            if(rcv_pkt.lastpacket==true){
                int newint;
                while(newint<contents){
                if(gotdata[newint]==true&& lastwritten!=rcv_pkt.sq_no){
                fwrite(storedata[newint].data,PACKET_SIZE,1,fp);
                }
                }
                exit(0);

            }
            // if(contents==WINDOW_SIZE || rcv_pkt.lastpacket==true){
            //     printf("WRITING TO FILE\n");
            //     for(int j=0;j<contents;j++){
            //         fwrite(storedata[j],PACKET_SIZE,1,fp);
            //         memset(storedata[j],0,PACKET_SIZE);
            //     }
            //     if(rcv_pkt.lastpacket==true){
            //         exit(0);
            //     }
            //     index=bufferedmax+1;
            //     strcpy(storedata[rcv_pkt.sq_no-index],rcv_pkt.data);
            // }
            
            // int k=0;
            // int l=0;
            // while(l<index){
            //     if((storedata[l]-currentpacket)==1)
            //     {
            //      printf("writing it to file\n");
            //      currentpacket=storedata[l];
            //      storedata[l]=0;
            //      k++;
            //     }
            //     }
            // for(int m=0;m<index;m++){
            //     if
            // }

            //}
            strcpy(rcv_ack.data, "ack from server\n\n");
            //printf("Sending Ack for Packet Number :- %d\n\n", rcv_pkt.sq_no);
            rcv_ack.sq_no = rcv_pkt.sq_no;
            rcv_ack.channelid=rcv_pkt.channelid;
            rcv_ack.lastpacket=false;
            if (sendto(sock, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other[2], slen) == -1)
            {
                die("send()");
            }
            printf("SERVER              S              %-15s             ACK                 %2d        SERVER          %s\n\n",get_time(),rcv_pkt.sq_no,linel);

        }
        // if(current==0){
        //     //printf("WRITING TO FILE\n");
        //         fwrite(storedata,PACKET_SIZE,WINDOW_SIZE,fp);
        // }
    }

    close(sock);
    return 0;
}
