#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define QUEUE_LENGTH (20)
#define BILLION  1000000000L;
#define WORK_DAY_LENGTH (42)

typedef struct{
	int random_enter_queue;
	float time_teller_start;
	float time_teller_stop;
	int random_trans_length;
} Customer;

// global variables
int front_of_line_index = 0;
int back_of_line_index = 0;
Customer bank_line[QUEUE_LENGTH];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_customer_serviced = 0;

// function prototypes
Customer create_customer(void);
void* teller(void* arg);
int get_cust_arrival_time(void);
int get_cust_transaction_time(void);
void set_timespec(int time, struct timespec* period);
double get_period(struct timespec* tim1, struct timespec* tim2);

void* teller(void* rand_seed)
{
	srand(rand_seed);
	struct timespec random_process_time;
	while(1)
	{
		if(front_of_line_index != back_of_line_index) {
			pthread_mutex_lock(&mutex);
			front_of_line_index = (front_of_line_index + 1)%QUEUE_LENGTH;
			num_customer_serviced++;
			pthread_mutex_unlock(&mutex);

			// "process" the customer, i.e. take time
			set_timespec(get_cust_transaction_time(), &random_process_time);
			nanosleep(&random_process_time, NULL);
		}

	}
}

int main(int argc, char *argv[]) {
	struct timespec start_time, curr_time, random_arrival_time;
	double accum_work_time;
	int temp_rand = 0;

	// we seed the random num generator
	srand(time(NULL));

	// save time into start struct
	clock_gettime(CLOCK_REALTIME, &start_time);
	clock_gettime(CLOCK_REALTIME, &curr_time);
	accum_work_time = get_period(&start_time, &curr_time);

	temp_rand = rand();
	pthread_create(NULL, NULL, &teller, &temp_rand);
	temp_rand = rand();
	pthread_create(NULL, NULL, &teller, &temp_rand);
	temp_rand = rand();
	pthread_create(NULL, NULL, &teller, &temp_rand);

	while(accum_work_time < WORK_DAY_LENGTH)
	{
		set_timespec(get_cust_arrival_time(), &random_arrival_time);
		nanosleep(&random_arrival_time, NULL);
		bank_line[front_of_line_index % QUEUE_LENGTH] = create_customer();
		back_of_line_index = (back_of_line_index + 1)%QUEUE_LENGTH;
		clock_gettime(CLOCK_REALTIME, &curr_time);
		accum_work_time = get_period(&start_time, &curr_time);
	}

	printf("length of work-day:\t\t%lf\n", accum_work_time);
	printf("number of customers serviced:\t\t%d\n", num_customer_serviced);
	return EXIT_SUCCESS;
}

int get_cust_arrival_time(void)
{
	return (rand() % 300)+100;
}

int get_cust_transaction_time(void)
{
	return (rand() % 550)+50;
}

void set_timespec(int time, struct timespec* period)
{
	// since 1 sec of real time is equal to 10 min
	// and we have no need to set anything greater than 6 min, we are
	// only setting the nsec property
	period->tv_sec = 0;
	period->tv_nsec = time * 1000000;
}

double get_period(struct timespec* tim1, struct timespec* tim2) {
	double period;
	period = ( tim2->tv_sec - tim1->tv_sec )
		+ (double)( tim2->tv_nsec - tim1->tv_nsec )
		/ (double)BILLION;
	return period;
}

Customer create_customer(void)
{
	Customer new_cust;
	new_cust.random_enter_queue = get_cust_arrival_time();
	new_cust.time_teller_start = 0.0;
	new_cust.time_teller_stop = 0.0;
	new_cust.random_trans_length = get_cust_transaction_time();

	return new_cust;
}
