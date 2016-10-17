#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define QUEUE_LENGTH (20)
#define BILLION  1000000000L;
#define WORK_DAY_LENGTH (42)

typedef struct{
	struct timespec time_entered_line;
	int id;
} Customer;

typedef struct{
	int break_duration;
	int next_break_time;
	unsigned int rand_seed;
} Teller;

// global variables
int back_of_line_index = 0;
Customer bank_line[QUEUE_LENGTH];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_customer_serviced = 0;
double total_cust_queue_time = 0;

int test = 0;

// function prototypes
Customer create_customer(void);
Teller create_teller(void);
void* teller(void* arg);
int get_cust_arrival_time(void);
int get_cust_transaction_time(void);
void set_timespec(int time, struct timespec* period);
double get_period(struct timespec* tim1, struct timespec* tim2);

void* teller(void* arg)
{
	Teller *teller = (Teller *)arg;
	srand(teller->rand_seed);
	struct timespec random_process_time, now;
	int front_of_line = 0;

	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(num_customer_serviced < back_of_line_index) {

			front_of_line = num_customer_serviced%QUEUE_LENGTH;
			clock_gettime(CLOCK_MONOTONIC, &now);

			total_cust_queue_time += get_period(&bank_line[front_of_line].time_entered_line, &now);
			//printf("%d teller -- %ld\n", bank_line[front_of_line].id, bank_line[front_of_line].time_entered_line.tv_sec);
			num_customer_serviced++;
			pthread_mutex_unlock(&mutex);

			// "process" the customer, i.e. take time
			set_timespec(get_cust_transaction_time(), &random_process_time);
			nanosleep(&random_process_time, NULL);
		}
		else {
			pthread_mutex_unlock(&mutex);
		}

	}
}

int main(int argc, char *argv[]) {
	struct timespec start_time, curr_time, random_arrival_time;
	Teller teller1, teller2, teller3;
	double accum_work_time;

	// we seed the random num generator
	srand(time(NULL));

	// save time into start struct
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	clock_gettime(CLOCK_MONOTONIC, &curr_time);
	accum_work_time = get_period(&start_time, &curr_time);

	teller1 = create_teller();
	pthread_create(NULL, NULL, &teller, (void *)&teller1);
	teller2 = create_teller();
	pthread_create(NULL, NULL, &teller, (void *)&teller2);
	teller3 = create_teller();
	pthread_create(NULL, NULL, &teller, (void *)&teller3);

	while(accum_work_time < WORK_DAY_LENGTH)
	{
		set_timespec(get_cust_arrival_time(), &random_arrival_time);
		nanosleep(&random_arrival_time, NULL);
		bank_line[back_of_line_index % QUEUE_LENGTH] = create_customer();
		back_of_line_index++;
		clock_gettime(CLOCK_MONOTONIC, &curr_time);
		accum_work_time = get_period(&start_time, &curr_time);
	}

	printf("total customer queue time: %lf\n", total_cust_queue_time);
	printf("Average time each customer is in line:\t\t%lf\n", total_cust_queue_time/(float)num_customer_serviced);
	printf("number of customers serviced:\t\t%d\n", num_customer_serviced);
	printf("length of day: \t\t%lf\n", accum_work_time);
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
	double period = 0;

	period = ( tim2->tv_sec - tim1->tv_sec )
		+ (double)( tim2->tv_nsec - tim1->tv_nsec )
		/ (double)BILLION;
	return period;
}

Customer create_customer(void)
{
	Customer new_cust;
	clock_gettime(CLOCK_MONOTONIC, &new_cust.time_entered_line);
	//printf("%d -- %ld\n", test, new_cust.time_entered_line.tv_sec);
	new_cust.id = test++;

	return new_cust;
}

Teller create_teller(void)
{
	Teller new_teller;
	new_teller.rand_seed = rand();
	new_teller.break_duration = 0;
	new_teller.next_break_time = 0;

	return new_teller;
}
