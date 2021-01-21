--------------------------- SELECTIVE REPEAT PROTOCOL OVER UDP -------------------

1. Here 2 ports are created for two relay nodes . These RELAY nodes receive data from client using these ports.
2.  Relay nodes then proceed to send the data to server.
3. Server replies back to RELAY1 and RELAY2 using same nodes they communicated from.
4.  RELAY NODES then fowarard ACK they received from server through sockets bind to their ports.
5. Whenever a packet is received at Relay nodes . It is forked .
6. The forked process then takes care of transmission of data through respective node as they knew channelid from received packet.
7. timeout is implemented in RELAY NODES using POLL system call and CLIENT NODE using SELECT system call
8. POLL has two sockets which stands for two channels of RELAY nodes.
9. Now the forked process adds delay using sleep.
10. argument to sleep is given to be (rand%3/1000).This gives us value of mostly [0-2]millisecods
11. POLL in RELAY node also takes care of ack coming from SERVER NODE
12. CLIENT may recieve acks which might not be in order.
13. If we get the first ack we slide the window
14. We are buffering the acks until timeout if they are not for first packet sent.
15. After timeout unacknowleged packets are sent again and timer is started again.
16. In CLIENT i have implemented timer using SELECT system call.
17. This timer will be reset only upon retransmission or arrival of first packet.
18. To Run the Program First run RELAY NODE and SERVER NODE
19. To Run RELAY NODE type gcc relay.c and then ./a.out
20. To RUN SERVER NODE type gcc server.c and then ./a.out 
21. Server Creates a file "output.txt" wherein all the data to be copied will appear
22. Now RUN CLIENT NODE . Type gcc client.txt and then ./a.out filename.txt {file to be copied}
23. Server Node is Crashing sometimes due to Segmentation fault . I Couldn't deduce a genuine reason for It
as upon running the program again no such fault appears.
24. KINDLY close all the running files using ctrl+c and run above said procedure
    twice or thrice if you encounter Segmentation fault or any other stoppage issues
25. LOG Data is printed on Terminals where they are run .