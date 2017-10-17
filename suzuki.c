//********************************************************************
//
// Rahul Bethi
// Advanced Operating Systems
// Project #3: Distribued Mutual Exclusion using
//				Suzuki and Kasamiâ€™s Broadast Algorithm
// November 17, 2016
// Instructor: Dr. Ajay K. Katangur
//
//********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h> // to set srand

int main(int argc, char *argv[])
{
	int rank, size, root = 0;
	MPI_Datatype tokenType;
	MPI_Status status;
	
	int i, j, probe_output, t, t1;
	int reqTag = 0, tokenTag = 1;
	int sim_totalTime, sim_endTime, wait_maxTime, wait_endTime, cs_totalTime = 2, cs_endTime, tTime;
	float inc_endTime, inc_totalTime = 0.065;
	enum pStates_enum { RELEASED, REQUESTED, GRANTED } pState;
	enum boolean { false = 0, true = 1 } hasToken, running, waiting, found;
	pState = RELEASED;
	
	MPI_Init(&argc, &argv); // Initializing MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // to know the rank of each process
	MPI_Comm_size(MPI_COMM_WORLD, &size); // to know the total number of processes
	MPI_Type_contiguous(size+100+1, MPI_INT, &tokenType);
	MPI_Type_commit(&tokenType);
	
	int RN[size], RN1[size], SN;
	struct token_struct { int LN[size]; int Q[100]; int Qsize; } token;
	token.Qsize = 0;
	
	if(size < 2)
	{
		printf("Error: Mnimum 2 processes are needed to run\n");
		MPI_Finalize();
		return;
	}
	
	// Initiating LN, RN
	for(i=0; i<size; i++)
	{
		token.LN[i] = 0;
		RN[i] = 0;
	}
	
	// SRand
	srand(time(NULL) + rank);
	
	// Taking inputs
	sscanf(argv[1], "%d", &sim_totalTime);
	sscanf(argv[2], "%d", &wait_maxTime);
	
	// Determining who should have the token first and initiating token
	if(rank == root)
	{
		hasToken = false;
		t = rand()%size;
		for(i=0;i<size;i++)
		{
			if(i != root)
			{
				if(i == t)	t1 = true;
				else			t1 = false;
				MPI_Send(&t1, 1, MPI_INT, i,tokenTag, MPI_COMM_WORLD);
			}
			else if(t == root)
			{
				hasToken = true;
				/* */printf("Process %d: has the token at first\n", rank);
			}
		}
	}
	else
	{
		MPI_Recv(&hasToken, 1, MPI_INT, root, tokenTag, MPI_COMM_WORLD, &status);
		/* */if(hasToken) // process has token if hasToken is true
			/* */printf("Process %d: has the token at first\n", rank);
	}
	
	//Setting Simulation time
	running = true;
	sim_endTime = MPI_Wtime() + sim_totalTime;
	
	// MPI Barrier ------------
	MPI_Barrier(MPI_COMM_WORLD);
	
	// Simulation starts here
	while(running)
	{
		if(pState == RELEASED) // RELEASED state, as soon as the process comes out of Critical section
		{
			inc_endTime = 0;
			waiting = true;
			tTime = rand() % (wait_maxTime+1);
			wait_endTime = MPI_Wtime() + tTime;// random waiting time generated between 0 to input given (5 secs)
			while(waiting) // waiting true until current time > waiting end time
			{
				// Iprobe for requests
				if( MPI_Wtime() >= inc_endTime )
				{
					MPI_Iprobe( MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &probe_output, &status); // check if message is received
					if(probe_output) // probe output true if message is ready
					{
						// update RN
						MPI_Recv(&SN, 1, MPI_INT, MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &status); // message received, SN
						if(SN > RN[status.MPI_SOURCE])
							RN[status.MPI_SOURCE] = SN; // check if SN is the latest and update RN
						// check for token
						printf("Rank %d received critical section request from rank %d\n", rank, status.MPI_SOURCE);
						if(hasToken && RN[status.MPI_SOURCE] == token.LN[status.MPI_SOURCE]+1)// if token is with self, and if RN is latest and genuine, then send token
						{
							// send token
							printf("Rank %d is sending the token to rank %d\n", rank, status.MPI_SOURCE);
							MPI_Send(&token, 1, tokenType, status.MPI_SOURCE, tokenTag, MPI_COMM_WORLD);
							hasToken = false; // token no longer available
						}
					}
					inc_endTime = MPI_Wtime() + inc_totalTime;
				}
				if( MPI_Wtime() >= sim_endTime || MPI_Wtime() >= wait_endTime )
					waiting = false; // stop waiting loop if waiting or simulation time expires
			}
			// check for simTime
			if( MPI_Wtime() < sim_endTime )
			{
				// check for token
				if(hasToken)
					pState = GRANTED; // change state to GRANTED
				else
				{
					// send requests to all processes
					RN[rank]++;
					printf("Process with rank %d and sequence number %d is requesting critical section\nBroadcast message (%d:%d)\n", rank, RN[rank], rank, RN[rank]);
					for(i=0; i<size; i++)
						if(i != rank)
							MPI_Send(&RN[rank], 1, MPI_INT, i, reqTag, MPI_COMM_WORLD);
					// change state to REQUESTED
					pState = REQUESTED;
					inc_endTime = 0;
				}
			}
			else
				running = false; // stop simulation loop if simulation time expires
		}
		if(pState == REQUESTED) // REQUESTED state
		{
			if( MPI_Wtime() >= inc_endTime )
			{
				// Iprobe for requests *** (same as in RELEASED state)
				MPI_Iprobe( MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &probe_output, &status);
				if(probe_output)
				{
					MPI_Recv(&SN, 1, MPI_INT, MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &status);
					if(SN > RN[status.MPI_SOURCE])
						RN[status.MPI_SOURCE] = SN;
					printf("Rank %d received critical section request from rank %d\n", rank, status.MPI_SOURCE);
				}
				// Iprobe for token
				MPI_Iprobe( MPI_ANY_SOURCE, tokenTag, MPI_COMM_WORLD, &probe_output, &status);
				if(probe_output)
				{
					MPI_Recv(&token, 1, tokenType, MPI_ANY_SOURCE, tokenTag, MPI_COMM_WORLD, &status); // Receive Token
					printf("Rank %d has received the token from Rank %d and entering into critical section\n", rank, status.MPI_SOURCE);
					pState = GRANTED;// goto critical section
					hasToken = true; // token available
				}
				inc_endTime = MPI_Wtime() + inc_totalTime;
			}
		}
		if(pState == GRANTED) // GRANTED state - critical section
		{
			inc_endTime = 0;
			cs_endTime = MPI_Wtime() + cs_totalTime; // calculate critical section end time, current time + 2 seconds
			while( MPI_Wtime() < cs_endTime ) // loop untill critical section time expires
			{
				if( MPI_Wtime() >= inc_endTime )
				{
					// Iprobe for requests *** (same as in RELEASED state)
					MPI_Iprobe( MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &probe_output, &status);
					if(probe_output)
					{
						MPI_Recv(&SN, 1, MPI_INT, MPI_ANY_SOURCE, reqTag, MPI_COMM_WORLD, &status);
						if(SN > RN[status.MPI_SOURCE])
							RN[status.MPI_SOURCE] = SN;
						printf("Rank %d received critical section request from rank %d\n", rank, status.MPI_SOURCE);
					}
					inc_endTime = MPI_Wtime() + inc_totalTime;
				}
			}
			token.LN[rank] = RN[rank]; // update LN with its SN
			for(i=0; i<size; i++) // check LN, RN and add processes to Q
			{
				if( i != rank && RN[i] == token.LN[i]+1 ) // check if RN is 1 more than LN
				{
					found = false; // if true, check if that process is already in Q
					for(j=0; j<token.Qsize; j++)
					{
						if(token.Q[j] == i)
						{
							found = true;
							j = token.Qsize;
						}
					}
					if(!found) // if not found, add that process to Q
					{
						token.Q[token.Qsize] = i;
						token.Qsize++;
					}
				}
			}
			printf("Rank %d has exited critical section\n", rank);
			if(token.Qsize > 0)
			{
				t = token.Q[0];
				token.Qsize--;
				for(i=0; i<token.Qsize; i++)
					token.Q[i] = token.Q[i+1];
				// send token to Q[0]
				printf("Rank %d is sending the token to rank %d\n", rank, t);
				MPI_Send(&token, 1, tokenType, t, tokenTag, MPI_COMM_WORLD);
				hasToken = false;
			}
			// change state to RELEASED
			pState = RELEASED;
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	/* */printf("Process %d: Exit\n", rank);
	MPI_Finalize();
	return;
}