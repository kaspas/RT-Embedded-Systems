#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc,char **argv)
{
  if(argc !=3)
  {
	printf("Error Missing parameters! \n"
			"The number of secconds: Secconds\n"
			"The number of samples: Samples \n");
    exit(1);
  }
  FILE *file;
  file= fopen("result.txt","w");
  if (file == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  int sec = atoi(argv[1]);
  int samples =atoi(argv[2]);
  struct timeval startwtime,endwtime;
  int i=0;
  /* Create A delay based on the number of samples and 
   * Number of secconds that user wants to run the program
   * t =secconds / samples 
   * fract = decimal part of the t multiplied by 10^9 because of timespec structure
   * realprt = secconds of delay time */
  double t = (double) sec/samples;
  double t2;
  int realprt= (int) t;
  double fract;
  fract= t - realprt;
  fract=fract*pow(10,9);
  fprintf(file,"%f %f %d\n",t,fract,realprt);
  
  struct timespec delay={realprt,fract};
  double *times=(double *)malloc(samples*sizeof(double));
  /* Initial Calibration of sample time*/
  fprintf(file,"Initial Caliration it won't take long...\n");
  double mean=0;
  t2=t;
  realprt=(int) t2;
  fract=(t2-realprt)*pow(10,9);
  delay.tv_sec=realprt;
  delay.tv_nsec=fract;
  /* begin sampling*/
  t2=t;
  fprintf(file,"\nExecution for %d secconds:\n",sec);
  gettimeofday(&startwtime,NULL);
  while(i<samples)
  {
    clock_nanosleep(CLOCK_REALTIME,0,&delay,NULL);
    gettimeofday(&endwtime,NULL);
    times[i]= (double)((endwtime.tv_usec - startwtime.tv_usec)
				/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);		
	
	/* time sampling calibration*/
	mean=t-times[i];
	t2=t2+mean;
	realprt=(int)t2;
	fract=(t2-realprt)*1000000000;
	delay.tv_sec=realprt;
	delay.tv_nsec=fract;
	i++;
	startwtime=endwtime;
  }
  double sum=0;
  for(i=0;i<samples;i++)
  {
	fprintf(file,"%fs\n",times[i]);  
	sum+=times[i];
  }
  fprintf(file,"Precision: %fs\n",(sum-sec));
}
