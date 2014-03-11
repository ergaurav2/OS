#include<pthread.h>
#include<stdio.h>
#include<iostream>
#include<semaphore.h>
#include<stdlib.h>
using namespace std;
void* GeekArrives(void *);
void* NonGeekArrives(void *);
void* SingerArrives(void*);
void* CreateGeeks(void*);
void* CreateNonGeeks(void*);
void* CreateSingers(void*);
void CrossBrdige();
sem_t sem_addperson;
sem_t sem_addgeek;
sem_t sem_addnongeek;
sem_t sem_addsinger;
pthread_t threadid_geek[10000];
pthread_t threadid_nongeek[10000];
pthread_t threadid_singer[10000];
pthread_t threadid[4];
int noofgeek=0;
int noofnongeek=0;
int noofsinger=0;
int boardgeek=0;
int boardnongeek=0;
int boardsinger=0;
int geektotal=0;
int nongeektotal=0;
int singertotal=0;
int geek=0,singer=0,nongeek=0;
	int totalperson=noofgeek+noofnongeek+noofsinger;
	int personrequest=4;
int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Usage ./a.out <#of geeks> <#of non-geeks> <#of singers>");	
		exit(0);
	}
	 noofgeek=atoi(argv[1]);
 	noofnongeek=atoi(argv[2]);
 	noofsinger=atoi(argv[3]);
	 totalperson=noofgeek+noofnongeek+noofsinger;
	sem_init(&sem_addperson,0,4);
	sem_init(&sem_addsinger,0,1);
	sem_init(&sem_addnongeek,0,1);
	sem_init(&sem_addgeek,0,1);
	pthread_create(&threadid[1],NULL,&CreateGeeks,NULL);
	pthread_create(&threadid[2],NULL,&CreateNonGeeks,NULL);
	pthread_create(&threadid[3],NULL,&CreateSingers,NULL);
	CrossBrdige();
	printf("Reamining Geek %d Non-Geek %d Singers %d\n",noofgeek-boardgeek,noofnongeek-boardnongeek,noofsinger-boardsinger);
				fflush(stdout);
return 0;
}

void* GeekArrives(void*) {
	sem_wait(&sem_addperson);
	sem_wait(&sem_addgeek);
	geek++;
	geektotal++;
	sem_post(&sem_addgeek);	
}

void* NonGeekArrives(void*) {
	sem_wait(&sem_addperson);
	sem_wait(&sem_addnongeek);
	nongeek++;
	nongeektotal++;
	sem_post(&sem_addnongeek);	
}

void* SingerArrives(void*) {
	sem_wait(&sem_addperson);
	sem_wait(&sem_addsinger);
	singer++;
	singertotal++;
	sem_post(&sem_addsinger);	
}
void* CreateGeeks(void*) {
	for(int i=1;i<=noofgeek;i++) {
		pthread_create(&threadid_geek[i],NULL,&GeekArrives,NULL);
		}
}

void* CreateNonGeeks(void*) {
	for(int i=1;i<=noofnongeek;i++) {
		pthread_create(&threadid_nongeek[i],NULL,&NonGeekArrives,NULL);
		}
}

void* CreateSingers(void*) {
	for(int i=1;i<=noofsinger;i++) {
		pthread_create(&threadid_singer[i],NULL,&SingerArrives,NULL);
		}
}

void CrossBrdige() {
	while(1) {
		sleep(1);
	if(singer>=1&&geek+nongeek==3) {
		printf("Geek %d Non-Geek %d Singer %d\n",geek,nongeek,1);
		fflush(stdout);
		boardsinger++;
		boardgeek=boardgeek+geek;
		boardnongeek=boardnongeek+nongeek;
		geek=0;
		nongeek=0;
		singer=singer-1;
		} else if(geek==2&&nongeek==2) {
		printf("Geek %d Non-Geek %d\n",geek,nongeek);
		sleep(1);
			fflush(stdout);	
			boardgeek=boardgeek+geek;
		boardnongeek=boardnongeek+nongeek;		
			geek=0;
			nongeek=0;
			
		} else if (geek==4) {
			printf("Geek %d\n",geek);
			sleep(1);
			fflush(stdout);
			boardgeek=boardgeek+geek;
			geek=0;
		} else if (nongeek==4) {
			printf("Non-Geek %d\n",nongeek);
			sleep(1);
			boardnongeek=boardnongeek+nongeek;
			fflush(stdout);			
			nongeek=0;
			
		} else if (personrequest<=totalperson){
			personrequest++;
			fflush(stdout);			
			sem_post(&sem_addperson);
		} 
		if(personrequest>totalperson) {
			sleep(3);
			break;
		}
	}
}
