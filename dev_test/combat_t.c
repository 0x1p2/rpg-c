#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>		// Threads. Requires the -lpthread for compiling with gcc.
#include <unistd.h>		// For sleep.
#include <string.h>

#define THREADS 2

void *thread_task(void *arg);


pthread_t	tid[THREADS];
pthread_mutex_t	lock;

typedef struct soul
{
	char name[16];
	int hp;		int hp_c;
	int dmg;	int def;
	int id;		int speed;

	struct soul *opp;

} soul_t;


int main(void)
{
	void *status;
	long i;

	pthread_attr_t attr;

	struct soul *p = malloc(sizeof(soul_t));
	struct soul *n = malloc(sizeof(soul_t));
	p->hp    = 50;	n->hp    = 40;
	p->dmg   =  5;	n->dmg   =  4;
	p->def   =  3;	n->def   =  3;
	p->id    =  1;	n->id    =  2;
	p->speed =  2;	n->speed = 3;

	p->hp_c = p->hp;	n->opp = p;
	n->hp_c = n->hp;	p->opp = n;


	strncpy(p->name, "Dev", sizeof(p->name));
	strncpy(n->name, "NPC", sizeof(n->name));


	pthread_mutex_init(&lock, NULL);

	/*  Create the threads. */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&tid[0], &attr, thread_task, (void *) p);
	pthread_create(&tid[1], &attr, thread_task, (void *) n);

	/*  Join the threads. */
	pthread_attr_destroy(&attr);
	
	for(i = 0; i < THREADS; i++)
	{
		pthread_join(tid[i], &status);
	}

	pthread_mutex_destroy(&lock);
	free(n);
	free(p);
	pthread_exit(NULL);
}


void *thread_task(void *arg)
{
	struct soul *l = (struct soul *) arg;
	printf("%s initialized; Fighting: %s. \n", l->name, l->opp->name);

	while(l->hp_c > 0 && l->opp->hp_c > 0)
	{

		//pthread_mutex_lock(&lock);	/* Initiate a lock. */
		l->hp_c -= l->opp->dmg;
		//pthread_mutex_unlock(&lock);	/* Unlock the code block. */
		printf("%s took %d. Hp is: %d, Speed: %d \n", l->name, l->opp->dmg, l->hp_c, l->speed);
		sleep(l->speed);
	}

	pthread_exit((void *) 0);	/* Exit the thread. */
}