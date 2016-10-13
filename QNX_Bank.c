#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define QUEUE_LENGTH (10)
#define BILLION  1000000000L;

typedef struct{
	int random_enter_queue;
	float time_teller_start;
	float time_teller_stop;
	int random_trans_length;
} customer;

int time_up = 0;

//func prototypes
customer create_cust(void);
void teller_1(void);
int get_cust_arrival_time(void);
int get_cust_transaction_time(void);
struct timespec get_time_spec(int time);

int main(int argc, char *argv[]) {
	customer cust;
	customer cust_line[QUEUE_LENGTH];
	int x = 0;
	int cust_arrival_time = 0;
	int cust_transaction_time = 0;
	struct timespec timer, timer2;
	clock_t start_time;
	clock_t end_time;
	srand(time(NULL));
	struct timespec start, stop;
	double accum;

	// save time into start struct
	clock_gettime( CLOCK_REALTIME, &start);

	timer.tv_sec = 8;
	timer.tv_nsec = 5*100000000L;
	nanosleep(&timer, NULL);

	// save time into end struct
	clock_gettime( CLOCK_REALTIME, &stop);

	accum = ( stop.tv_sec - start.tv_sec )
			 + (double)( stop.tv_nsec - start.tv_nsec )
			   / (double)BILLION;
	printf( "%lf\n", accum );

	//  42s of abs clock time = 7hrs of simulated time
	// since, we scale 60s to 0.1s, 1hr would therefore be 6s
	// and 7hrs would then be 42s
	// 1. start teller threads
	// 2. get in loop that runs until day is over
	// 3. wait between 1-4 min - then create a new cust
	// 4. when a cust is a front of line, they release a mutex
	// 5. teller threds then race to get that mutex
	// 6. teller thread that successfully gets mutex sleeps for 30s to 6m

	/*while(clock() < end_time)
	cust_arrival_time = get_cust_arrival_time();
	timer = get_time_spec(cust_arrival_time);
	nanosleep(timer);
	cust_line[x] =

	cust_arrival_time = get_cust_arrival_time();*/

	/*while(x < QUEUE_LENGTH)
	{
		printf("arrival is: %d\n", cust_line[x].random_enter_queue);
		printf("arrival is: %d\n", cust_line[x].random_trans_length);
		x++;
	}*/



	return EXIT_SUCCESS;
}

int get_cust_arrival_time()
{
	return (rand() % 3000)+1000;
}

int get_cust_transaction_time()
{
	return (rand() % 5500)+500;
}

struct timespec get_time_spec(int time)
{
	struct timespec tim;
	long temp;
	tim.tv_sec = time/1000;
	temp = time % 1000;
	tim.tv_nsec = temp * BILLION;
	return tim;
}

customer create_cust()
{
	customer new_cust;
	new_cust.random_enter_queue = get_cust_arrival_time();
	new_cust.time_teller_start = 0.0;
	new_cust.time_teller_stop = 0.0;
	new_cust.random_trans_length = get_cust_transaction_time();

	return new_cust;
}
