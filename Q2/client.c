/*
    Simple udp client with stop and wait functionality
*/
#include "packet.h"
int main(int argc, char* argv[])
{
    printf("NODE_NAME       EVENT_TYPE              TIMESTAMP               PACKET_TYPE          SEQ.NO        SRC          DEST\n\n");
    FILE* fp=fopen(argv[1],"r");
    fd_set rfds;
    struct timeval tv;
    DATA_PKT store[WINDOW_SIZE];
    int sent[PACKET_SIZE];
    bool ack[PACKET_SIZE];
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        ack[i] = false;
    }
    struct sockaddr_in serverAddr1, serverAddr2, myAddr;
    int sock, i, slen = sizeof(serverAddr1), slen2 = sizeof(serverAddr2);
    char buf[BUFLEN];
    char message[BUFLEN];
    DATA_PKT send_pkt, rcv_ack;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *)&serverAddr1, 0, sizeof(serverAddr1));
    serverAddr1.sin_family = AF_INET;
    serverAddr1.sin_port = htons(PORT1);
    serverAddr1.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *)&serverAddr2, 0, sizeof(serverAddr2));
    serverAddr2.sin_family = AF_INET;
    serverAddr2.sin_port = htons(PORT2);
    serverAddr2.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *)&myAddr, 0, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(PORT0);
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&myAddr, sizeof(myAddr)) == -1)
    {
        die("bind");
    }
    int pktcount = 0;
    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        store[i].sq_no = pktcount;
        pktcount++;
        if(!feof(fp))
        fread(store[i].data , PACKET_SIZE,1,fp);
        if(feof(fp)){
            store[i].lastpacket=true;
        }
        else{
           store[i].lastpacket=false;
        }
        
        if (i % 2 == 0)
        {
            //printf("sent through 0\n");
            store[i].channelid = 0;
            sent[i] = i;
            if (sendto(sock, &store[i], sizeof(store[i]), 0, (struct sockaddr *)&serverAddr1, slen) == -1)
            {
                die("sendto()");
            }
            printf("CLIENT             S              %-15s             DATA                %2d        CLIENT          RELAY1\n\n",get_time(),store[i].sq_no);

        }
        if (i % 2 == 1)
        {
            //printf("sent through 1\n");
            store[i].channelid = 1;
            sent[i] = i;
            if (sendto(sock, &store[i], sizeof(store[i]), 0, (struct sockaddr *)&serverAddr2, slen) == -1)
            {
                die("sendto()");
            }
            printf("CLIENT             S              %-15s             DATA                %2d        CLIENT          RELAY2\n\n",get_time(),store[i].sq_no);

        }
    }
    int state = 0;
    int med;
    int number = 0;
    int recv_len;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    int new;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    while (!feof(fp))
    {
        new = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (new == 0)
        {

            int k=0;
            while(k<WINDOW_SIZE){
                if(ack[k]==false){
                    if (k%2==0)
                        {
                            //printf("sent through 0\n");
                            if (sendto(sock, &store[k], sizeof(store[k]), 0, (struct sockaddr *)&serverAddr1, slen) == -1)
                            {
                                die("sendto()");
                            }
                    printf("CLIENT            TO              %-15s             DATA                %2d        CLIENT          RELAY1\n\n",get_time(),store[k].sq_no);
                    printf("CLIENT            RE              %-15s             DATA                %2d        CLIENT          RELAY1\n\n",get_time(),store[k].sq_no);

                        }
                        if (k%2==1)
                        {
                            //printf("sent through 1\n");
                            if (sendto(sock, &store[k], sizeof(store[k]), 0, (struct sockaddr *)&serverAddr2, slen) == -1)
                            {
                                die("sendto()");
                            }
                    printf("CLIENT            TO              %-15s             DATA                %2d        CLIENT          RELAY2\n\n",get_time(),store[k].sq_no);
                    printf("CLIENT            RE              %-15s             DATA                %2d        CLIENT          RELAY2\n\n",get_time(),store[k].sq_no);
                        
                        }
                }
            k++;
            }
            tv.tv_sec=2;
            tv.tv_usec=0;
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);
        }
        else{
        if (FD_ISSET(sock, &rfds))
        {
            if (recvfrom(sock, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&myAddr, &slen) == -1)
            {
                die("recvfrom()");
            }
            char newl[20];
            if(rcv_ack.channelid==0){
                strcpy(newl,"RELAY1");
            }
            else{
                strcpy(newl,"RELAY2");
            }
            printf("CLIENT             R              %-15s             DATA                %2d        %s          CLIENT\n\n",get_time(),rcv_ack.sq_no,newl);

            //printf("received data sequence number is %d\n\n", rcv_ack.sq_no);
            if (rcv_ack.sq_no != sent[0])
            {
                //printf("OUT OF ORDER SQUENCE\n\n");
                ack[rcv_ack.sq_no - sent[0]] = true;
            }
            else
            {
                //printf("IN ORDER SQUENCE\n\n");

                ack[0] = true;
                int cont = 0;
                int index = 0;
                while (index < WINDOW_SIZE && ack[index] != false)
                {
                    cont++;
                    index++;
                }
                for (int i = 0; i < WINDOW_SIZE; i++)
                {
                    if (i + index < WINDOW_SIZE)
                    {
                        ack[i] = ack[i + index];
                        sent[i] = sent[i + index];
                        store[i] = store[i + index];
                    }
                    else
                    {
                        ack[i] = false;
                        sent[i] = pktcount;
                        pktcount++;
                        store[i].sq_no = sent[i];
                        store[i].channelid = sent[i] % 2;
                        fread(store[i].data ,PACKET_SIZE,1,fp);
                         if(feof(fp)){
                            store[i].lastpacket=true;
                            }
                        else{
                            store[i].lastpacket=false;
                            }
                       // store[i].lastpacket=false;
                        if (store[i].channelid == 0)
                        {
                            //printf("sent through 0\n");
                            if (sendto(sock, &store[i], sizeof(store[i]), 0, (struct sockaddr *)&serverAddr1, slen) == -1)
                            {
                                die("sendto()");
                            }
                       printf("CLIENT             S              %-15s             DATA                %2d        CLIENT          RELAY1\n\n",get_time(),store[i].sq_no);

                        }
                        if (store[i].channelid == 1)
                        {
                            //printf("sent through 1\n");
                            if (sendto(sock, &store[i], sizeof(store[i]), 0, (struct sockaddr *)&serverAddr2, slen) == -1)
                            {
                                die("sendto()");
                            }
                       printf("CLIENT             S              %-15s             DATA                %2d        CLIENT          RELAY2\n\n",get_time(),store[i].sq_no);

                            

                        }
                    }
                }
                tv.tv_sec=2;
                tv.tv_usec=0;
                FD_ZERO(&rfds);
                FD_SET(sock, &rfds);
            }
        }
    }
    }

    close(sock);
    return 0;
}
