


#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<stdlib.h>
#include<unistd.h>
using namespace std;

sem_t empty_customer_chef;
sem_t full_customer_chef;


sem_t empty_chef_masterchef;
sem_t full_chef_masterchef;

sem_t empty_masterchef_waiter;
sem_t full_masterchef_waiter;


pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;

queue<int> customer_chef,chef_masterchef,masterchef_waiter;
int cnt = 0;

void init_semaphore()
{
	sem_init(&empty_customer_chef,0,5);
	sem_init(&full_customer_chef,0,0);

	sem_init(&empty_chef_masterchef,0,1);
	sem_init(&full_chef_masterchef,0,0);

	sem_init(&empty_masterchef_waiter,0,1);
	sem_init(&full_masterchef_waiter,0,0);



	pthread_mutex_init(&lock1,0);
	pthread_mutex_init(&lock2,0);
	pthread_mutex_init(&lock3,0);
}


void * waiter(void * arg){
   printf("I am Just an Waiter.\n");

	while(true)
	{

        sem_wait(&full_masterchef_waiter); // 0

		pthread_mutex_lock(&lock3);

		sleep(1);
		int item = masterchef_waiter.front();
		masterchef_waiter.pop();
		printf("\nWaiter has delivered order, Item %d\n", item);


		pthread_mutex_unlock(&lock3);


        sem_post(&empty_masterchef_waiter);
	}




}






void * masterchef(void * arg){
   printf("I am Master-chef.\n");

	while(true)
	{

        sem_wait(&full_chef_masterchef); // 0
        sem_wait(&empty_masterchef_waiter);

		pthread_mutex_lock(&lock2);
		pthread_mutex_lock(&lock3);

		sleep(1);
		int item = chef_masterchef.front();
		chef_masterchef.pop();
		masterchef_waiter.push(item);
		printf("\nMaster-Chef processed order, Item no: %d\n", item);


		pthread_mutex_unlock(&lock2);
		pthread_mutex_unlock(&lock3);


        sem_post(&empty_chef_masterchef);
        sem_post(&full_masterchef_waiter);
	}




}

void * customer(void * arg)
{
    int* producer_id = (int*)arg;
	printf("I am customer %d\n",(int*)producer_id);
	int i;
	while(true)
	{
		sem_wait(&empty_customer_chef);

		pthread_mutex_lock(&lock1);

		sleep(1);
		customer_chef.push(cnt);
		printf("\nCustomer(%d) made request for item %d\n", producer_id, cnt);
		cnt++;
		pthread_mutex_unlock(&lock1);

		sem_post(&full_customer_chef);
	}
}








void * chef(void * arg)
{
     int* chef_id = (int*)arg;

    printf("I am chef %c\n", (char*) chef_id+64);
	int i;
	while(true)
	{
        sem_wait(&full_customer_chef);
        sem_wait(&empty_chef_masterchef); // 0

		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lock2);

		sleep(1);
		int item = customer_chef.front();
		customer_chef.pop();
		chef_masterchef.push(item);

		printf("\nChef %c has processed order,Item no: %d\n", (char*) chef_id+64, item);


		pthread_mutex_unlock(&lock1);
		pthread_mutex_unlock(&lock2);

        sem_post(&empty_customer_chef);
        sem_post(&full_chef_masterchef);
	}
}






int main(void)
{
	pthread_t thread_customer[10];
	pthread_t thread_chef[3];
	pthread_t thread_masterchef;
	pthread_t thread_waiter;

    int x[10];

    // holding the id in an array
    for(int i = 0; i < 10; i++){
        x[i] = i+1;
    }

	init_semaphore();



	for(int i=0;i<3;i++){
        pthread_create(&thread_chef[i], NULL, chef, (void*) *(x+i));
	}

	for (int i = 0; i < 10; i++){
       pthread_create(&thread_customer[i], NULL, customer, (void*) *(x+i));
	}
	pthread_create(&thread_masterchef, NULL, masterchef, (void*) *(x+0));
    pthread_create(&thread_waiter,NULL,waiter,(void*) *(x+0));


	while(1);
	return 0;
}
