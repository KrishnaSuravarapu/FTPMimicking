----------------------------------MULTICHANNEL STOP AND WAIT PROTOCOL -------------------------------------------

1. In Order to excute it is required to run server first otherwise connection cannot be accepted.
2. To RUN SERVER type gcc server.c and ./a.out 
3. Server automatically creates a file called output.txt and writes to it
4. Now we can start client to start transfer
5. TO RUN CLIENT type gcc server.c and ./a.out filename.txt {filename.txt as arg which is a file to copy}
6. In this question i tried to solve the problem of timeout using select system call
7. 2 sockets are created on same port which serve the purpose of two channels
8. These 2 Channels are then added to select which act as FILE DESCRIPTORS for select call
9. timeout for select call is set to be 2 seconds
10. Packets will be sent from 2 channels simultaneously , if packet is dropped at one channel ,other channel continuous to send the Packets
11. Packets retransmission will happen only when BOTH the channels drop packets.
12. Now retransmission takes place and two channels continuous to send the packets until one is dropped
13. Drop is implemented by using totalpacket%(100/PDR) . Although this might not randomly drop packets , it drops statistically almost correctly.
14. Here totalpacket is not sequence number wherelse it is total packets.
15. On Server side when one channel fails to send packet or drop takes place , other will send the packets.
16. These packets will be buffered if they are not inorder as lost packet might have less seq.no;
and when we get the lost packet all this buffer is written to file.
17. Buffer Size is kept to be [100/PDR] where PDR is PACKET DROP RATE
18. This comes from the fact that PDR is fixed and other channel sends atmost [100/PDR] packets before getting dropped.
19. Also here PAYLOAD size is assumed to same as of DATA size in the packet.
20. Client Trace and Server Trace are printed on the terminals on which they are run.
