#include<pthread.h>
#include<stdio.h>
#include<iostream>
#include<semaphore.h>
#include<stdlib.h>
#include<fstream>
#include<stdio.h>
#include<string.h>
using namespace std;
void* student(void*);
pthread_t threadid_stud[10000];
int preferlist[1000][9];
int availcourse[1000][5];
int studids[10000];
int alloccount[1000];
int noofstud=0;
int noofcour=0;
sem_t sem_coursealloc[1000];
sem_t mutex;
int nonalloc=0;
int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage ./a.out <#of students> <#of courses>");
		exit(0);
		}
		 noofstud=atoi(argv[1]);
		 noofcour=atoi(argv[2]);
		int divide = noofstud/5; 
		for(int j=1;j<=noofcour;j++) {
			availcourse[j][1] = divide;
			availcourse[j][2] = divide;
			availcourse[j][3] = divide*2;
			availcourse[j][4] = divide;
		}
	 	for(int i=1;i<=noofstud;i++) {
		studids[i] = i;
		}
		for(int i=1;i<=noofcour;i++) {
		sem_init(&sem_coursealloc[i],0,1);
		}
		sem_init(&mutex,0,1);
		for(int i=1;i<=noofstud;i++) {
		pthread_create(&threadid_stud[i],NULL,&student,&studids[i]);
		}
		for(int i=noofstud;i>=1;i--) {
		  pthread_join(threadid_stud[i],NULL);
		}
		printf("Student not allocated %d\n",nonalloc);
		FILE* allocfs = fopen("available.txt","w");
		for(int i=1;i<noofcour;i++) {
			fprintf(allocfs,"course %d --> %d\n",i,alloccount[i]);
		}
return 0;
}
void* student(void* in) {
	int coursealloc[4]={0,0,0,0};
	int studid = *(int *)in;
	//printf("Entering student %d\n",studid);
	for(int j=0;j<=3;j++) {
		while(1) {
			int v1 = rand() % noofcour+ 1;

			if(v1%4==j) {
				preferlist[studid][j+1]=v1;
				break;
			}
		}
	}
	for(int j=5;j<=8;j++) {
			int v1 = rand() % noofcour+ 1;
			preferlist[studid][j]=v1;
		}
	sem_wait(&mutex);
	for(int k=1;k<=8;k++) {
	//	printf("%d ",preferlist[studid][k]);
	}
	int branch = rand() % 4+ 1;
	//printf("for student %d with branch %d\n",studid,branch);
	sem_post(&mutex);
	for(int k=1;k<=8;k++) {
		int courseid = preferlist[studid][k];
		int spect = preferlist[studid][k]%4;
		if(coursealloc[spect]==0) {
			sem_wait(&sem_coursealloc[courseid]);
			//printf("Avaliable course for %d in %d %d\n",courseid,branch,availcourse[courseid][branch]);
			if(availcourse[courseid][branch]>0) {
					availcourse[courseid][branch]--;
					alloccount[courseid]++;
					coursealloc[spect]=1;
				//	printf("Allocated course for %d in %d\n",courseid,branch);
				}
			sem_post(&sem_coursealloc[courseid]);
		}
	}
	sem_wait(&mutex);
	sem_post(&mutex);
	if(coursealloc[0]==1 && coursealloc[1]==1 && coursealloc[2]==1 && coursealloc[3]==1) {
	} else {
		printf("No course allocated to the student %d\n",studid);
		sem_wait(&mutex);
		nonalloc++;
		sem_post(&mutex);
	}
}

