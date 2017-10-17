//********************************************************************
//
//  Rahul Bethi
//  Advanced Operating Systems
//  Programming Project #3: Distribued Mutual Exclusion using
//			    Suzuki and Kasami’s Broadast Algorithm
//  November 20, 2016
//  Instructor: Dr. Ajay K. Katangur
//
//  Read me file
//
//********************************************************************

This project is to implement Suzuki and Kasami's Broadcast Algorithm in Message passing interface (MPI) system. This is a simulation. Input is given with the simulation time and alsot he wiating time, which is the time a process has to wait to ask for a crtiical section request after coming out of a critical section. In any state, either the REQUESTED, RELEASED or GRANTED state, they have to process the requests coming to them. Each process has RN vector and there is token which moves around. the process which has the token can goto critical section or GRANTED state. Here the token is a structure which has LN vector, Q list and length of Q.
First, the root process (which is set to 0) generates a random number from 0 to the size of processes to decide which process has tohave the token at first. It sends the message to all processes about if they have the token or not. After this, the simulation starts with all process at once using a barrier. Simulation end time is set by adding the simulation duration to the presetn time.
All processes are set to be in RELEASED state. As soon as they enter release state, they generate a random number between 0 and the maximum waiting time they can wait. This end time of the wiating time is set by adding the duration of the waiting time to the present time. a flag is set to run a while loop untill this time is exceeded to process the Critical section requests from all other processes. This processing of requests is done in any state. Iprobe function is used to tell if a messae has been received. If that probe flag is set to 1, then the messge is recieved which has a sequence number of the requesting process. Iprobe function is called 15 times in a second, as its expensive for jumpshot. Maximum value of the sequence number and the RN value of that process, which this has is stored in RN. If the process has the toke, then If this RN value of the requesting process is  1 more than the LN value of that requesting rpocess, in the token, then the token is given to that process. If the process doesn not have the token, then it only updates RN. if the wiating time exceeds, If this process has the token and also wants to goto critical secttion, it just goes to the critical sectiom, as it already has the token. If the process doesn't have th token, it will send the critical section requests to all other processes by incrementing its sequence value by 1.= and send it. It then goes to REQUESTED state, where it waits for the token and also processes other critical section requests.
In the REQUESTED, along with processing for requests, it also processes messages for token. Similar to critical section requests, Iprobe is used with a different tag to check for token. If the token is received, then it goes to GRANTED state, which it waits in critical section for 2 seconds.
In the GRANTED state, it processes critical section requsts, just like RELEASED state and waiting. After the critical section times out, then it comes out the checks its RN for each process with LN+1 and checks if that rank id is already in Q list. If it is not found, then this rank is added to Q list. If the Q list is not empty after this point, then it sends the token to the first process in Q. It removes the first value in Q and adjusts it, before sending the token. It then finally goes to the RELEASED state.
This while processes is looped untill the simulation time is finished.
After the simulation time exceeds, no more new requests are made, but the processes finish only after all requested processes execute critical section.


//********************************************************************
//  FILE NAMES
//********************************************************************
Readme.txt
Makefile
suzuki.c
suzuki.pdf

//********************************************************************
//  EXAMPLE INPUT / OUTPUT
//********************************************************************

[...]$ make
mpecc -mpilog suzuki.c -o suzuki
[...]$ mpirun -np 4 suzuki 30 5
Process 2: has the token at first
Process with rank 3 and sequence number 1 is requesting critical section
Broadcast message (3:1)
Rank 0 received critical section request from rank 3
Rank 1 received critical section request from rank 3
Rank 2 received critical section request from rank 3
Rank 2 is sending the token to rank 3
Rank 3 has received the token from Rank 2 and entering into critical section
Rank 3 has exited critical section
Process with rank 0 and sequence number 1 is requesting critical section
Broadcast message (0:1)
Rank 2 received critical section request from rank 0
Rank 3 received critical section request from rank 0
Rank 3 is sending the token to rank 0
Rank 1 received critical section request from rank 0
Rank 0 has received the token from Rank 3 and entering into critical section
Process with rank 2 and sequence number 1 is requesting critical section
Broadcast message (2:1)
Process with rank 3 and sequence number 2 is requesting critical section
Broadcast message (3:2)
Rank 2 received critical section request from rank 3
Rank 1 received critical section request from rank 3
Rank 0 received critical section request from rank 3
Rank 3 received critical section request from rank 2
Rank 1 received critical section request from rank 2
Rank 0 received critical section request from rank 2
Process with rank 1 and sequence number 1 is requesting critical section
Broadcast message (1:1)
Rank 0 has exited critical section
Rank 0 is sending the token to rank 2
Rank 2 has received the token from Rank 0 and entering into critical section
Rank 2 received critical section request from rank 1
Rank 0 received critical section request from rank 1
Rank 3 received critical section request from rank 1
Rank 2 has exited critical section
Rank 2 is sending the token to rank 3
Process with rank 2 and sequence number 2 is requesting critical section
Broadcast message (2:2)
Rank 3 has received the token from Rank 2 and entering into critical section
Rank 3 received critical section request from rank 2
Rank 0 received critical section request from rank 2
Rank 1 received critical section request from rank 2
Rank 3 has exited critical section
Rank 3 is sending the token to rank 1
Rank 1 has received the token from Rank 3 and entering into critical section
Process with rank 0 and sequence number 2 is requesting critical section
Broadcast message (0:2)
Rank 2 received critical section request from rank 0
Rank 3 received critical section request from rank 0
Rank 1 received critical section request from rank 0
Process with rank 3 and sequence number 3 is requesting critical section
Broadcast message (3:3)
Rank 1 has exited critical section
Rank 1 is sending the token to rank 2
Rank 2 has received the token from Rank 1 and entering into critical section
Rank 2 received critical section request from rank 3
Rank 1 received critical section request from rank 3
Rank 0 received critical section request from rank 3
Process with rank 1 and sequence number 2 is requesting critical section
Broadcast message (1:2)
Rank 2 has exited critical section
Rank 2 is sending the token to rank 0
Rank 0 has received the token from Rank 2 and entering into critical section
Rank 0 received critical section request from rank 1
Rank 2 received critical section request from rank 1
Rank 3 received critical section request from rank 1
Rank 0 has exited critical section
Rank 0 is sending the token to rank 3
Process with rank 2 and sequence number 3 is requesting critical section
Broadcast message (2:3)
Rank 3 has received the token from Rank 0 and entering into critical section
Rank 3 received critical section request from rank 2
Rank 0 received critical section request from rank 2
Rank 1 received critical section request from rank 2
Process with rank 0 and sequence number 3 is requesting critical section
Broadcast message (0:3)
Rank 2 received critical section request from rank 0
Rank 1 received critical section request from rank 0
Rank 3 received critical section request from rank 0
Rank 3 has exited critical section
Rank 3 is sending the token to rank 1
Rank 1 has received the token from Rank 3 and entering into critical section
Rank 1 has exited critical section
Rank 1 is sending the token to rank 0
Rank 0 has received the token from Rank 1 and entering into critical section
Process with rank 1 and sequence number 3 is requesting critical section
Broadcast message (1:3)
Process with rank 3 and sequence number 4 is requesting critical section
Broadcast message (3:4)
Rank 0 has exited critical section
Rank 0 is sending the token to rank 2
Rank 3 received critical section request from rank 1
Rank 2 has received the token from Rank 0 and entering into critical section
Rank 2 received critical section request from rank 3
Rank 1 received critical section request from rank 3
Rank 0 received critical section request from rank 1
Rank 2 received critical section request from rank 1
Rank 0 received critical section request from rank 3
Rank 2 has exited critical section
Rank 2 is sending the token to rank 1
Process with rank 2 and sequence number 4 is requesting critical section
Broadcast message (2:4)
Rank 1 has received the token from Rank 2 and entering into critical section
Rank 1 received critical section request from rank 2
Rank 3 received critical section request from rank 2
Rank 0 received critical section request from rank 2
Process with rank 0 and sequence number 4 is requesting critical section
Broadcast message (0:4)
Rank 2 received critical section request from rank 0
Rank 1 received critical section request from rank 0
Rank 3 received critical section request from rank 0
Rank 1 has exited critical section
Rank 1 is sending the token to rank 3
Rank 3 has received the token from Rank 1 and entering into critical section
Rank 3 has exited critical section
Rank 3 is sending the token to rank 0
Rank 0 has received the token from Rank 3 and entering into critical section
Rank 0 has exited critical section
Rank 0 is sending the token to rank 2
Rank 2 has received the token from Rank 0 and entering into critical section
Process 3: Exit
Process 0: Exit
Process 1: Exit
Rank 2 has exited critical section
Process 2: Exit
Writing logfile....
Enabling the Default clock synchronization...
Finished writing logfile suzuki.clog2.