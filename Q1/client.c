#include"packet.h"

int main(int argc, char* argv[])
{
    FILE* fp=fopen(argv[1],"r");
    printf("CLIENT TRACE\n\n");
    printf("TYPE            SEQ.NO          SIZE        CHANNEL\n\n");
    DATA_PKT send_pkt, rcv_ack, send_pkt2, rcv_ack2;
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    int handle;
    /* CREATE A TCP SOCKET*/

    int sock1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int sock2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock1 < 0)
    {
        die("socket");
    }
    // printf("Socket Number is %2d          ", sock1);
    if (sock2 < 0)
    {
        die("socket");
    }

    /*CONSTRUCT SERVER ADDRESS STRUCTURE*/

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    /*memset() is used to fill a block of memory with a particular value*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);                   //You can change PORT number here
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Specify server's IP address here
    //printf("Address assigned\n");

    /*ESTABLISH CONNECTION*/
    int c1 = connect(sock1, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (c1 < 0)
    {
        die("connection");
    }
    //printf("Connection Established\n");
    int c2 = connect(sock2, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (c2 < 0)
    {
        die("connection");
    }

    int status1=fcntl(sock1,F_SETFL,fcntl(sock1,F_GETFL,0) | O_NONBLOCK);
    if(status1==-1){
        die("fcntl()");
    }
    int status2=fcntl(sock2,F_SETFL,fcntl(sock2,F_GETFL,0) | O_NONBLOCK);
    if(status2==-1){
        die("fcntl()");
    }


    //printf("Connection 2 Established\n");
    int state = 0;
    int sq_no_1 = 0;
    int sq_no_2 = 0;
    int faulty = -1;
    int initial_1=1;
    int initial_2=1;
    while (!(feof(fp)))
    {       

               if(FD_ISSET(sock1, &rfds))
                {
                    //printf("heard from 1\n");
                    if (recv(sock1, &rcv_ack, sizeof(rcv_ack), 0) == -1)
                    {
                        die("recv()");
                    }
                    //printf("not here");
                    //printf("received ack is %2d         \n",rcv_ack.sq_no);
                    if (rcv_ack.sq_no == sq_no_1)
                    {
                        printf("RECV_ACK         %2d                  NULL             %2d         \n", rcv_ack.sq_no,rcv_ack.channelid);
                    }

                }

            if (FD_ISSET(sock2,&rfds))
                {
                    //printf("heard from 2\n");
                    if (recv(sock2, &rcv_ack2, sizeof(rcv_ack2), 0) == -1)
                    {
                        die("recv()");
                    }
                    //printf("not here");
                    ///printf("222 received ack is %2d          whereas seq no is %2d         \n",rcv_ack2.sq_no,sq_no_2);
                    if (rcv_ack2.sq_no == sq_no_2)
                    {
                        printf("RECV_ACK         %2d                  NULL              %2d         \n", rcv_ack2.sq_no,rcv_ack2.channelid);
                        
                        //printf("Received ack seq. no. %2d          FROM LINE %2d         \n", rcv_ack2.sq_no,rcv_ack2.channelid);
                    }
                }
        
        if(initial_1==1 || FD_ISSET(sock1,&rfds)){
            sq_no_1 = max(sq_no_1, sq_no_2);
            if(sq_no_1!=0)
            sq_no_1++;
             //printf("Sending %2d          through line 0\n",sq_no_1);
             //printf("Enter message %2d         : \n", sq_no_1); //wait for sending packet with  seq. no. 0
             fgets(send_pkt.data,PACKET_SIZE,fp);


            //printf("sending data : \n %s \n",send_pkt.data);
             
            send_pkt.sq_no = sq_no_1;
            send_pkt.channelid = 0;
            if (send(sock1, &send_pkt, sizeof(send_pkt), 0) != sizeof(send_pkt))
            {
                die("send()");
            }
            printf("SEND_PKT         %2d                  %ld               %2d         \n", send_pkt.sq_no,sizeof(send_pkt.data),send_pkt.channelid);

            initial_1=0;
            
            }

            if(initial_2==1 || FD_ISSET(sock2,&rfds)){
            sq_no_2 = max(sq_no_1, sq_no_2);
            sq_no_2++;
            //printf("Sending %2d          through line 1\n",sq_no_2);
            //printf("Enter message %2d         : \n", sq_no_2); //wait for sending packet with  seq. no. 0
            fgets(send_pkt2.data,PACKET_SIZE,fp);




            //printf("sending data : \n %s \n",send_pkt2.data);
            send_pkt2.sq_no = sq_no_2;
            send_pkt2.channelid = 1;
            if (send(sock2, &send_pkt2, sizeof(send_pkt2), 0) != sizeof(send_pkt2))
            {
                die("send()");
            }
            printf("SEND_PKT         %2d                  %ld               %2d         \n", send_pkt2.sq_no,sizeof(send_pkt2.data),send_pkt2.channelid);

            initial_2=0;
            }


            FD_ZERO(&rfds);
            FD_SET(sock1, &rfds);
            FD_SET(sock2, &rfds);
            tv.tv_sec=2;
            tv.tv_usec=0;
            handle=select(sock2+1,&rfds,NULL,NULL,&tv);
            tv.tv_sec=2;
            tv.tv_usec=0;


            
            
            if(handle==0){
            //printf("RESENDING 1 : \n\n\n");
                if (send(sock1, &send_pkt, sizeof(send_pkt), 0) != sizeof(send_pkt))
            {
                die("send()");
            }
            printf("RESEND_PKT       %2d                  %ld               %2d         \n", send_pkt.sq_no,sizeof(send_pkt.data),send_pkt.channelid);

            initial_1=0;
            
            //printf("RESENDING 2 : \n\n\n");
                if (send(sock2, &send_pkt2, sizeof(send_pkt2), 0) != sizeof(send_pkt2))
            {
                die("send()");
            }
            printf("SEND_PKT         %2d                  %ld                %2d         \n", send_pkt2.sq_no,sizeof(send_pkt2.data),send_pkt2.channelid);

            initial_2=0;
            FD_ZERO(&rfds);
            FD_SET(sock1, &rfds);
            FD_SET(sock2, &rfds);
            
            handle=select(max(sock2,sock1)+1,&rfds,NULL,NULL,&tv);
            tv.tv_sec=2;
            tv.tv_usec=0;
            }

    }

    close(sock1);
    close(sock2);
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
