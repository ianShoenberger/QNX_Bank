#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define QUEUE_LENGTH (10)

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
	struct timespec timer;
	clock_t start_time;
	long end_time;
	srand(time(NULL));

	start_time = clock();
	end_time = (start_time +(CLOCKS_PER_SEC * 42)+1000000);
	printf("starttime is: %ld\n", start_time/CLOCKS_PER_SEC);
	printf("endtime is: %ld\n", end_time/CLOCKS_PER_SEC);

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
	return (rand() % 300)+100;
}

int get_cust_transaction_time()
{
	return (rand() % 550)+50;
}

struct timespec get_time_spec(int time)
{
	struct timespec tim;
	long temp;
	tim.tv_sec = time/100;
	temp = time % 100;
	// this is 10 to 10th -- because our random # is 10^-2
	tim.tv_nsec = temp * 10000000000;
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
