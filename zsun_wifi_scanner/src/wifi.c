#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "../include/utils.h"
#define MAX 50 //number of different ssid that we can save (if we need more we will alocate the size )
typedef struct{

	time_t t; 
    struct tm tm;
	char **ssid;
	struct timeval startwtime;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
	} ssid_info;

void delete_ssid_info(ssid_info *wifi_info);
ssid_info * ssid_info_init();
void *wifi_writer(void* arg);
void *wifi_scanner(void* arg);


pthread_cond_t *wakeupA,*wakeupB;
pthread_mutex_t *threadA,*threadB;


int main(int argc,char *argv[])
{ 
  if(argc!=2){
  printf("Missing number of seconds ! \n");
  exit(1);
  } 
  threadA=(pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (threadA, NULL);
  threadB=(pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (threadB, NULL);
  wakeupA=(pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init(wakeupA,NULL);
  wakeupB=(pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init(wakeupB,NULL);
  int sleepTime=atoi(argv[1]);
  ssid_info *wifi_info;
  wifi_info=ssid_info_init();
  if (ssid_info_init()==NULL){
    fprintf (stderr, "main: Wifi Info Init failed.\n");
    exit (1);  
  }
  pthread_t scanner, writer;
  int flag;
  flag=pthread_create (&scanner, NULL, wifi_scanner, wifi_info);
  if(flag){//elegxos an egine create
			  printf("Error: pthread_create returned code: %d\n", flag);
			  exit(1);
  }
  flag=pthread_create (&writer, NULL, wifi_writer, wifi_info);
  if(flag){//elegxos an egine create
			  printf("Error: pthread_create returned code: %d\n", flag);
			  exit(1);
  }
  
	/*
	 * SCAN TIMER
	 * it makes the threads to wait for the 
	 * scan time we've set
	 */

  struct timeval startwtime,endwtime;
  clock_t start_t,end_t;
  /*time to sleep*/
  double t2,time;
  double mean=0;
  int realprt= (int) sleepTime;
  double fract;
  fract= sleepTime - realprt;
  fract=fract*pow(10,9);  
  struct timespec delay={realprt,fract};
  /* Initial Calibration of sample time*/
  delay.tv_sec=realprt;
  delay.tv_nsec=fract;
  /* begin sampling*/
  t2=sleepTime;
  gettimeofday(&startwtime,NULL);
  for(;;){
  start_t = clock();
  clock_nanosleep(CLOCK_REALTIME,0,&delay,NULL);
  gettimeofday(&endwtime,NULL);
  time=(double)((endwtime.tv_usec - startwtime.tv_usec)
				/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);		
  /* time sampling calibration*/
	mean=sleepTime-time;
	t2=t2+mean;
	realprt=(int)t2;
	fract=(t2-realprt)*1000000000;
	delay.tv_sec=realprt;
	delay.tv_nsec=fract;
    pthread_cond_signal(wakeupA);
	pthread_cond_signal(wakeupB);
	end_t=clock();
	cpu_usage(start_t,end_t);
	startwtime=endwtime;
  }
  pthread_join (scanner, NULL);
  pthread_join (writer, NULL);
  //delete_ssid_info(wifi_info);

}


void *wifi_writer(void* q){
  ssid_info *wifi_info=(ssid_info *) q;
  pthread_mutex_lock(threadA);
  int i;
  for(;;){
  pthread_mutex_lock (wifi_info->mut);
  if(wifi_info->ssid == NULL){
    pthread_cond_wait (wifi_info->notEmpty, wifi_info->mut);
  }

    struct tm t;
    t=wifi_info->tm;
    int fl=get_in_file(wifi_info->ssid,t,wifi_info->startwtime);
    if(fl!=0) printf("Error! %d\n",fl);
    wifi_info->ssid=NULL;
    pthread_mutex_unlock (wifi_info->mut);
    pthread_cond_signal (wifi_info->notFull);

  pthread_cond_wait (wakeupA,threadA);
  }
}
void *wifi_scanner(void* q){
  ssid_info *wifi_info;
  wifi_info=(ssid_info *) q;
  pthread_mutex_lock(threadB);

  for(;;){
  pthread_mutex_lock (wifi_info->mut);
  if( wifi_info->ssid !=NULL){
    pthread_cond_wait (wifi_info->notFull, wifi_info->mut);
  }
  wifi_info->t= time(NULL);
  
  wifi_info->ssid=read_ssid();
  wifi_info->tm= *localtime(&wifi_info->t);
  gettimeofday(&wifi_info->startwtime,NULL);
  pthread_mutex_unlock (wifi_info->mut);
  pthread_cond_signal (wifi_info->notEmpty);
  pthread_cond_wait (wakeupB,threadB);
  }
}

void delete_ssid_info(ssid_info *wifi_info){
  free(wifi_info->ssid);

  pthread_mutex_destroy(wifi_info->mut);
  free(wifi_info->mut);
  pthread_cond_destroy(wifi_info->notFull);
  free(wifi_info->notFull);
  pthread_cond_destroy(wifi_info->notEmpty);
  free(wifi_info->notEmpty);
  free(wifi_info);
}
ssid_info * ssid_info_init(){
  ssid_info *wifi_info;
  wifi_info=(ssid_info*)malloc(sizeof(ssid_info));
  if (wifi_info==NULL) return NULL;
  wifi_info->ssid=NULL;
  wifi_info->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (wifi_info->mut, NULL);
  wifi_info->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (wifi_info->notFull, NULL);
  wifi_info->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (wifi_info->notEmpty, NULL);
  return (wifi_info);
}
