#include<pthread.h>
#include<stdio.h>
#include<iostream>
#include<semaphore.h>
#include<stdlib.h>
#include<unistd.h>
using namespace std;
void* site(void * in);
void* hyd(void * in);
void* oxy(void * in);
sem_t sem_mutex;
sem_t sem_thres;
sem_t sem_sites[100];
sem_t sem_hyd[10000];
sem_t sem_oxy[10000];
sem_t sem_hyds;
sem_t sem_oxys;
sem_t sem_react;
int siteids[100];
int hydids[10000];
int oxyids[10000];
int hydcount=0;
int oxycount=0;
int energy[100];
int state[100];
	int noofhyd;
	int noofoxy;
	int sites;
	int thres;
	pthread_t threadid_hyd[10000];
	pthread_t threadid_oxy[10000];
int main(int argc, char **argv) {
	if (argc!=5) {
	printf("Usage : ./a.out <No of H atoms> <No of O atoms> <No of sites> <Threshold Energy(inUnits)> \n");
	}
		noofhyd = atoi(argv[1]);
		noofoxy	= atoi(argv[2]);
		sites = atoi(argv[3]);
		thres = atoi(argv[4]);
	pthread_t threadid_sites[100];
	for(int i=1;i<=sites;i++) {
		sem_init(&sem_sites[i],0,1);
	}
	for(int i=1;i<=noofhyd;i++) {
		sem_init(&sem_hyd[i],0,0);
	}
	for(int i=1;i<=noofoxy;i++) {
		sem_init(&sem_oxy[i],0,0);
	}
	sem_init(&sem_mutex,0,1);
	sem_init(&sem_react,0,1); 
	sem_init(&sem_thres,0,thres); 
	sem_init(&sem_hyds,0,0);
	sem_init(&sem_oxys,0,0); 
	for(int i=1;i<=sites;i++) {
		siteids[i] = i;
		energy[i]=0;
	}
	for(int i=1;i<=sites;i++) {
		state[i]=0;
	}
	for(int i=1;i<=noofhyd;i++) {
		hydids[i] = i;
	}
	for(int i=1;i<=noofoxy;i++) {
		oxyids[i] = i;
	}
	for(int i=1;i<=noofhyd;i++) {
		pthread_create(&threadid_hyd[i],NULL,&hyd,&hydids[i]);
	}
	for(int j=1;j<=noofoxy;j++) {
		pthread_create(&threadid_oxy[j],NULL,&oxy,&oxyids[j]);
	}
	for(int k=1;k<=sites;k++) {
		pthread_create(&threadid_sites[k],NULL,&site,&siteids[k]);
	}
	
	for(int i=sites;i>=1;i--) {
		  pthread_join(threadid_sites[i],NULL);
	}
	printf("Not Reacted -> H %d O %d\n",noofhyd-hydcount,noofoxy-oxycount);
	fflush(stdout);
	for(int i=1;i<=sites;i++) {
		printf("Total Energy at site %d was %d E Mj\n",i,energy[i]);
	}
return 0;
}
void* site(void * in) {
	while(1) {
	// Checking for the consecutive sites, taking lock on them
	int siteid = *(int *)in;
	sem_wait(&sem_mutex);
	 siteid = *(int *)in;
	int leftsite = (siteid-1);
	int rightsite = (siteid+1);	
	if(leftsite==0 || (leftsite!=0&& state[leftsite]==0)) {
		if((rightsite==sites+1)|| (rightsite!=sites+1&&state[rightsite]==0) ) {
			state[siteid]=1;
		}
		else{
			sem_post(&sem_mutex);
			continue;
		}
	}
	else{
		sem_post(&sem_mutex);
		continue;
	}
	sem_post(&sem_mutex);
	// Checking for the threshold
	sem_wait(&sem_thres);
	sem_wait(&sem_react);
	if(hydcount+2<=noofhyd&&oxycount+1<=noofoxy) {
		sem_post(&sem_hyd[hydcount+1]);
		sem_post(&sem_hyd[hydcount+2]);
		sem_post(&sem_oxy[oxycount+1]);
		pthread_join(threadid_hyd[hydcount+1],NULL);
		pthread_join(threadid_hyd[hydcount+2],NULL);
		pthread_join(threadid_oxy[oxycount+1],NULL);
		hydcount=hydcount+2;
		oxycount=oxycount+1;
		printf("Water created at site %d releasing energy 1 E Mj (Remaining -> H %d O %d)\n",siteid,noofhyd-hydcount,noofoxy-oxycount);
		fflush(stdout);
		energy[siteid]++;
		sem_post(&sem_react);
	} else {
		sem_post(&sem_react);
		sem_post(&sem_thres);
		state[siteid]=0;
		
		pthread_exit(0);		
	}
	sleep(3);
	sem_post(&sem_thres);
	//Releasing the consecutive sites
	siteid = *(int *)in;
	state[siteid]=0;
	usleep(1000);
	}
}
void* hyd(void * in) {
	int hydid = *(int *)in;
	sem_wait(&sem_hyd[hydid]);
	
}
void* oxy(void * in) {
	int oxyid = *(int *)in;
	sem_wait(&sem_oxy[oxyid]);
}
