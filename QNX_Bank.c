#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define QUEUE_LENGTH (10)
#define BILLION  (1000000000L)
#define THOUSAND (1000)      //need to convert the units from the random generation into simulation time
#define WORK_DAY_LENGTH (42)

typedef struct{
	struct timespec time_entered_line;
} Customer;

typedef struct{
	struct timespec next_break_length;
	struct timespec next_break_time;
	double total_transaction_time;
	double max_transaction_time;
	double total_wait_time;
	double max_wait_time;
	unsigned int rand_seed;
} Teller;

// global variables
int back_of_line_index = 0;
Customer bank_line[QUEUE_LENGTH];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_customer_serviced = 0;
int max_line_depth = 0;
double total_cust_queue_time = 0.0;
double max_cust_queue_time = 0.0;

// function prototypes
Customer create_customer(void);
Teller create_teller(void);
void* teller(void* arg);
int get_cust_arrival_time(void);
int get_cust_transaction_time(void);
void set_timespec(int time, struct timespec* period);
double get_period(struct timespec* tim1, struct timespec* tim2);
double get_max(double val1, double val2);
double convert_to_simulation_time(double val);
void set_next_teller_break(struct timespec* break_time, struct timespec* break_time_length);

void* teller(void* arg)
{
	Teller *teller = (Teller *)arg;
	srand(teller->rand_seed);
	struct timespec teller_waiting, random_process_time, now;
	int front_of_line = 0;
	double temp_wait_time, temp_transaction_time, temp_cust_wait_time, temp_break_check = 0.0;

	// we need to set the random teller break periods in the startup routine so that the results
	// are unique each time
	set_next_teller_break(&teller->next_break_time, &teller->next_break_length);

	clock_gettime(CLOCK_MONOTONIC, &teller_waiting);

	while(1)
	{
		clock_gettime(CLOCK_MONOTONIC, &now);
		temp_break_check = get_period(&teller->next_break_time, &now);

		if(temp_break_check > 0)
		{
			nanosleep(&teller->next_break_length, NULL);
			set_next_teller_break(&teller->next_break_time, &teller->next_break_length);
		}

		pthread_mutex_lock(&mutex);
		if(num_customer_serviced < back_of_line_index) {

			// figure out if we have a new maximum depth of the queue
			if((back_of_line_index - num_customer_serviced) > max_line_depth)
				max_line_depth = back_of_line_index - num_customer_serviced;

			front_of_line = num_customer_serviced%QUEUE_LENGTH;
			clock_gettime(CLOCK_MONOTONIC, &now);

			// determine how long a customer had been waiting to be served
			temp_cust_wait_time = get_period(&bank_line[front_of_line].time_entered_line, &now);
			total_cust_queue_time += temp_cust_wait_time;
			max_cust_queue_time = get_max(max_cust_queue_time, temp_cust_wait_time);

			num_customer_serviced++;
			pthread_mutex_unlock(&mutex);

			// determine how long teller has been waiting since serving a customer
			temp_wait_time = get_period(&teller_waiting, &now);
			teller->total_wait_time += temp_wait_time;
			teller->max_wait_time = get_max(teller->max_wait_time, temp_wait_time);

			// add up all transaction times to get stats for customers time with tellers
			temp_transaction_time = get_cust_transaction_time();
			teller->total_transaction_time += temp_transaction_time;
			//find max transaction time
			teller->max_transaction_time = get_max(teller->max_transaction_time, temp_transaction_time);

			// "process" the customer, i.e. take time
			set_timespec(temp_transaction_time, &random_process_time);
			nanosleep(&random_process_time, NULL);

			// done with customer
			clock_gettime(CLOCK_MONOTONIC, &teller_waiting);
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
	double max_wait_time_tellers, avg_wait_time_tellers, max_trans_time_customers, avg_trans_time_customers = 0.0;

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

	// STATS
	// total num customers serviced
	printf("The total number of customers serviced during the day:\t%d\n", num_customer_serviced);

	// avg time each customer waits in line
	printf("Average time each customer spends waiting in the line:\t%lf\n", convert_to_simulation_time(total_cust_queue_time)/(float)num_customer_serviced);

	// avg time customer spends with teller
	avg_trans_time_customers = (teller1.total_transaction_time + teller2.total_transaction_time + teller3.total_transaction_time)/(float)num_customer_serviced;
	printf("The average time each customer spends with the teller:\t%.2f\n", avg_trans_time_customers/100.0);

	// avg time tellers wait for cust
	avg_wait_time_tellers = (teller1.total_wait_time + teller2.total_wait_time + teller3.total_wait_time)/(float)num_customer_serviced;
	printf("The average time tellers wait for customers:\t\t%.2f\n", convert_to_simulation_time(avg_wait_time_tellers));

	// max cust wait time in line
	printf("The maximum customer wait time in the line:\t\t%.2f\n", convert_to_simulation_time(max_cust_queue_time));
	// max wait time for tellers
	max_wait_time_tellers = get_max(get_max(teller1.max_wait_time, teller2.max_wait_time), teller3.max_wait_time);
	printf("The maximum wait time for tellers:\t\t\t%.2f\n", convert_to_simulation_time(max_wait_time_tellers));
	// max transaction time for tellers
	max_trans_time_customers = get_max(get_max(teller1.max_transaction_time, teller2.max_transaction_time), teller3.max_transaction_time);
	printf("The maximum transaction time for the tellers:\t\t%.2f\n", max_trans_time_customers/100.0);
	// max depth of line
	printf("The maximum depth of the queue:\t\t\t\t%d\n", max_line_depth);

	//printf("length of day: \t\t%lf\n", accum_work_time);
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

void set_next_teller_break(struct timespec* break_time, struct timespec* break_time_length)
{
	int temp_rand = 0;

	temp_rand = (rand() % 3000) + 3000;
	clock_gettime(CLOCK_MONOTONIC, break_time);
	break_time->tv_sec += temp_rand/1000;
	break_time->tv_nsec += (temp_rand%1000)*1000000;

	temp_rand = (rand() % 300) + 100;
	break_time_length->tv_sec = temp_rand/1000;
	break_time_length->tv_nsec = (temp_rand%1000)*1000000;

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

double get_max(double val1, double val2)
{
	double max = 0;
	if(val1 > val2)
		max = val1;
	else
		max = val2;

	return max;
}

double convert_to_simulation_time(double val)
{
	double time = 0;
	time = val * 10;
	return time;
}

Customer create_customer(void)
{
	Customer new_cust;
	clock_gettime(CLOCK_MONOTONIC, &new_cust.time_entered_line);
	return new_cust;
}

Teller create_teller(void)
{
	Teller new_teller;
	new_teller.rand_seed = rand();
	new_teller.next_break_length.tv_sec = 0;
	new_teller.next_break_length.tv_nsec = 0;
	new_teller.next_break_time.tv_sec = 0;
	new_teller.next_break_time.tv_nsec = 0;
	new_teller.max_transaction_time = 0.0;
	new_teller.max_wait_time = 0.0;
	new_teller.total_transaction_time = 0.0;
	new_teller.total_wait_time = 0.0;

	return new_teller;
}
