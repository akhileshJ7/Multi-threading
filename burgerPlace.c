/*
 ============================================================================
 Name        : burgerPlace.c
 Author      : Janaswamy
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "llist.h"
#include "burgerPlace.h"

void* cookRoutine(void* args);
void* frierRoutine(void* args);
void* customerRoutine(void* args);

static list burgerWarmer;
static list fryWarmer;

pthread_mutex_t burgerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fryMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t orderingMutex = PTHREAD_MUTEX_INITIALIZER;

sem_t burgerCount;
sem_t fryCount;

volatile int start = 0;

int maxBurgers;
int maxFries;

struct cooks* c;
struct friers* f;

int main(int argc, char* argv[]) {

	// End program if there isn't a file
	if(argc != 2){
		printf("Invalid number of arguments\n");
		exit(EXIT_FAILURE);
	}

	// Open matrix file
	FILE* f1 = fopen(argv[1], "r");

	// Allocate memory for cooks
	c = (struct cooks*) malloc(sizeof(struct cooks));

	// Get values for cooks
	fscanf(f1, "%d", &c->numCooks);
	fscanf(f1, "%d", &c->time);
	fscanf(f1, "%d", &c->numBurgers);

	// Allocate memory for friers
	f = (struct friers*) malloc(sizeof(struct friers));

	// Get values for friers
	fscanf(f1, "%d", &f->numFriers);
	fscanf(f1, "%d", &f->time);
	fscanf(f1, "%d", &f->numFries);

	// Create variable to track number of customers
	int numCustomers;
	fscanf(f1, "%d", &numCustomers);

	struct customer* cust[numCustomers];
	for(int i = 0; i < numCustomers; i++){
		// Allocate memory for the customer
		cust[i] = (struct customer*) malloc(sizeof(struct customer));

		// Get values for customer
		fscanf(f1, "%d", &cust[i]->numBurgers);
		fscanf(f1, "%d", &cust[i]->numFries);
		fscanf(f1, "%d", &cust[i]->wait);
		cust[i]->ordersFilled = 0;
	}
	fclose(f1);

	printf("The Burger Place\n");
	printf("Burger cooks - Number:%d Time: %d Total: %d\n", c->numCooks, c->time, c->numBurgers);
	printf("Fry cooks - Number: %d Time:%d Total:%d\n", f->numFriers, f->time, f->numFries);
	for(int i = 0; i < numCustomers; i++){
		printf("Customer %d - Burgers:%d Fries:%d Wait:%d\n", i, cust[i]->numBurgers, cust[i]->numFries, cust[i]->wait);
	}

	pthread_t brg_ids[c->numCooks];

	pthread_t fry_ids[f->numFriers];

	pthread_t cst_ids[numCustomers];

	llInit(&burgerWarmer);
	llInit(&fryWarmer);

	sem_init(&burgerCount, 0, 0);
	sem_init(&fryCount, 0, 0);

	for(int i = 0; i < c->numCooks; i++){
		if(pthread_create(&brg_ids[i], NULL, cookRoutine, NULL) == -1){
			printf("Could not create thread\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < f->numFriers; i++){
		if(pthread_create(&fry_ids[i], NULL, frierRoutine, NULL) == -1){
			printf("Could not create thread\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < numCustomers; i++){
		if(pthread_create(&cst_ids[i], NULL, customerRoutine, cust[i]) == -1){
			printf("Could not create thread\n");
			exit(EXIT_FAILURE);
		}
	}

	start = 1;

	for(int i = 0; i < c->numCooks; i++){
		pthread_join(brg_ids[i], NULL);
	}

	for(int i = 0; i < f->numFriers; i++){
		pthread_join(fry_ids[i], NULL);
	}

	sleep(5);

	//printf("There are %d items in the burger tray\n", llSize(&burgerWarmer));
	//printf("and %d items in the fry tray\n", llSize(&fryWarmer));
	printf("The max burgers in tray: %d\n", maxBurgers);
	printf("The max fries in the tray: %d\n", maxFries);

	for(int i = 0; i < numCustomers; i++){
		printf("Customer %d had their order filled %d times\n", i, cust[i]->ordersFilled);

	}
	llClear(&burgerWarmer);
	llClear(&fryWarmer);

	sem_destroy(&burgerCount);
	sem_destroy(&fryCount);

	pthread_mutex_destroy(&burgerMutex);
	pthread_mutex_destroy(&fryMutex);
	pthread_mutex_destroy(&orderingMutex);

	free(c);
	free(f);
	for(int i = 0; i < numCustomers; i++){
		free(cust[i]);
	}
}



void* cookRoutine(void* args){
	while (start == 0);

	int num;

	// Cooks will make burgers until they reach their total
	for(int i = 0; i < c->numBurgers; i++){
		// Burger Warmer must be locked before adding to it
		pthread_mutex_lock(&burgerMutex);

		// Burger is pushed to list
		llPushFront(&burgerWarmer, "BURGER");

		//printf("Burger added to the warmer\n");

		// Increase count of burgers
		sem_post(&burgerCount);

		// Set highest value of burgers, if applicable
		sem_getvalue(&burgerCount, &num);

		if(maxBurgers < num){
			maxBurgers = num;
		}

		// Burger warmer is unlocked
		pthread_mutex_unlock(&burgerMutex);

		// Cook waits until they can make another burger
		usleep(c->time);
	}

	return NULL;
}



void* frierRoutine(void* args){
	while (start == 0);

	// Friers will make fries until they reach their total
	for(int i = 0; i < f->numFries; i++){
		// Frier Warmer must be locked before adding to it
		pthread_mutex_lock(&fryMutex);

		// Fry is pushed to list
		llPushFront(&fryWarmer, "FRIES");

		//printf("Fry added to the warmer\n");

		// Increase count of fries
		sem_post(&fryCount);

		// Set highest value of burgers, if applicable
		int num;
		sem_getvalue(&fryCount, &num);

		if(maxFries < num){
			maxFries = num;
		}

		// Fry warmer is unlocked
		pthread_mutex_unlock(&fryMutex);

		// Frier waits until they can make more fries
		usleep(f->time);
	}

	return NULL;
}



void* customerRoutine(void* args){
	while(start == 0);

	struct customer* c = (struct customer*) args;

	//printf("Customer wants %d burgers and %d fries\n", c->numBurgers, c->numFries);

	for(int i = 0; i < 1000; i++){
		pthread_mutex_lock(&orderingMutex);

		for(int i = 0; i < c->numBurgers; i++){
			sem_wait(&burgerCount);
			pthread_mutex_lock(&burgerMutex);

			if(llPopBack(&burgerWarmer) == 0){
				printf("Burger removed from empty tray \n");
			}

			//printf("Burger removed from warmer\n");

			pthread_mutex_unlock(&burgerMutex);
		}


		for(int i = 0; i < c->numFries; i++){

			sem_wait(&fryCount);

			pthread_mutex_lock(&fryMutex);

			if(llPopBack(&fryWarmer) == 0){
				printf("Fry removed from empty tray\n");
			}
			//printf("Fry removed from warmer\n");

			pthread_mutex_unlock(&fryMutex);
		}

		pthread_mutex_unlock(&orderingMutex);
		c->ordersFilled++;
		usleep(c->wait);
	}

	return NULL;
}









