#include <stdlib.h>
#include <stdio.h>

#define QUEUE_LENGTH (10)

typedef struct{
	float time_queue_start;
	float time_teller_start;
	float time_teller_stop;
} customer;
customer cust_line[QUEUE_LENGTH];

int time_up = 0;

//func prototypes
customer create_cust(void);
void teller_1(void);

int main(int argc, char *argv[]) {
	customer cust;
	int x = 0;

	while(x < QUEUE_LENGTH)
	{
		cust_line[x] = create_cust();
		x++;
	}
	//s_rand()

	while(!time_up)
	{
		sleep(rand());
	}

	pthread_create(NULL, NULL, &teller_1, NULL);

	return EXIT_SUCCESS;
}

customer create_cust()
{
	customer new_cust;
	new_cust.time_queue_start = 0.0;
	new_cust.time_teller_start = 0.0;
	new_cust.time_teller_stop = 0.0;

	return new_cust;
}
