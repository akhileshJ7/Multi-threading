#ifndef BURGERPLACE_H
#define BURGERPLACE_H

typedef struct cooks{
	int numCooks;
	int time;
	int numBurgers;
} cooks;

typedef struct friers{
	int numFriers;
	int time;
	int numFries;
} friers;

typedef struct customer{
	int numBurgers;
	int numFries;
	int wait;
	int ordersFilled;
} customer;

#endif
