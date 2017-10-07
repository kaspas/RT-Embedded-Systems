#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/utils.h"
/*
 * TO DO LIST
 * small memory leak every 2-3mins  
 * must reduce memory usage
 * somewhere i have a pointer or smth that i haven't free 
 * change getline with fgets for sorter buffer size
 */



#define _GNU_SOURCE
#define MAXARG 50 //number of different ssid that we can save (if we need more we will alocate the size )

#define DELIM1 "\n"
#define DELIM2 "\""
#define DELIM "\n\""


//char **get_name(char *line);
char* concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if(!result)
   {
    printf("Error allocate memory!\n");
    exit(1);
   }  
    strcpy(result, s1);
    strcat(result, s2);
   
    return result;
}
char ** read_ssid(){
  FILE *fp;
  char path[1035];
  char **line;
  line=(char**)malloc(MAXARG*sizeof(char*));
  int i;
  for(i=0;i<MAXARG;i++)line[i]=(char*)malloc(1035*sizeof(char ));
  /* Open the command for reading. */
  fp = popen("/bin/sh searchWifi.sh", "r");
 // fflush(fp); 
  while (fp == NULL) {
    printf("Failed to run command : %s\n",strerror(errno) );
    for(i=0;i<MAXARG;i++){
	  free(line[i]);
	}
	free(line);
    return NULL;
   // exit(1);
  }
  /* Read the output a line at a time - output it. */
  i=0;
  while (fgets(path, sizeof(path), fp) != NULL) {
    //printf("%s", path);
    char *ptr;
    char *token;
    token=strtok_r(path,DELIM2,&ptr);
    token=strtok_r(NULL,DELIM2,&ptr);
    strcpy(line[i],token);
   // if(token!=NULL) free(path);
    i++;
  }

  int j;
  for(j=i;j<MAXARG;j++){
    free(line[j]);
  }
  line[i]=NULL;
 
  /* close */
  pclose(fp);

  return line ;
}

int get_in_file(char **ssid,struct tm tm,struct timeval startwtime){
  FILE *file;
  struct timeval endwtime;
  file=fopen("../data/my_wifi","a+");
  if(file==NULL){
    printf("Failed to open my_wifi appended mode : %s\n",strerror(errno) );
    return 1;
  }
  fseek(file,0,SEEK_END);
  int size=ftell(file);
  if(ssid==NULL) {
	fclose(file);
    return 0;
  }
  if (size==0){

	int i=0;
    while(ssid[i]!=NULL){
	  fprintf(file,"\"%s\" ",ssid[i]);
	  fprintf(file,"%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      free(ssid[i]);
      ssid[i]=NULL;
      i++;
	  gettimeofday(&endwtime,NULL);
		if(save_difftime(startwtime,endwtime)!=0){
		  printf("ERROR");
          fclose(file);
		  return 6;
		}
	  if(i>MAXARG) break;
	}
    fclose(file);
  }
  else{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fclose(file);

    file=fopen("../data/my_wifi","r");
    if(file==NULL){
      printf("Failed to open my_wifi read mode : %s\n",strerror(errno) );
	  return 1;
	}
    //fflush(file); 
    int i;
    while ((read= getline(&line, &len, file)) != -1) {
      char *token;
      char *ptr;
      char *ssid_and_stambss;
      token=strtok_r(line,DELIM,&ptr);//token = ssid name clear without " "
      ssid_and_stambss=strtok_r(NULL,DELIM,&ptr);// the timestambs
      char *temp;
      temp=concat("\"",token);

      char *temp2=concat(temp,"\"");
      char *ssid_and_stambs=concat(temp2,ssid_and_stambss);// whole line without \n delimiter
      int flag=0;
      free(temp);
      free(temp2);
      temp=NULL;
      temp2=NULL;
	  i=0;
	  int count;
	  while(ssid[i]!=NULL){
	    i++;
       	if(i>MAXARG) break;
	  }
	  count=i-1;//ssid expect NULL pointer
	  i=0;
      while(ssid[i]!=NULL){
	    if(strcmp(ssid[i],token)==0){
		  char temp[25];

		  sprintf(temp," %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		  char *ssid_and_stamb=concat(ssid_and_stambs,temp);
		  flag=1;
		  if(save_to_new_file(ssid_and_stamb)!=0){
		    printf("ERROR");
		    return 2;
		  }
		gettimeofday(&endwtime,NULL);
		if(save_difftime(startwtime,endwtime)!=0){
		  printf("ERROR");
		  return 6;
		}
		free(ssid_and_stamb);
		ssid_and_stamb=NULL;
		char tmp[1035];
		strcpy(tmp,ssid[count]);
		strcpy(ssid[i],tmp);
		free(ssid[count]);
		ssid[count]=NULL;
		count--;
		if(count<0) count=0;
		i=0;
		break;
		}
		else{
          i++;
     	} 
     	if(i>MAXARG) break;
      }

      if(flag==0){
		char *ssid_and_stamb=concat(ssid_and_stambs,"\n");
	    if(save_to_new_file(ssid_and_stamb)!=0){
	      printf("ERROR");
	      return 3;
	    }
	    free(ssid_and_stamb);
	    ssid_and_stamb=NULL;
	    gettimeofday(&endwtime,NULL);
		if(save_difftime(startwtime,endwtime)!=0){
		  printf("ERROR");
		  return 6;
		}
	  }
      free(token);
      token=NULL;
	  free(ssid_and_stambs);
	  ssid_and_stambs=NULL;
	}	
    free(line);
    line=NULL;
    i=0;
	while(ssid[i]!=NULL){
      char *ssid_and_stambs;
      char *temp,tmp[25];
      temp=concat("\"",ssid[i]);
      char *temp2=concat(temp,"\"");
	  fprintf(file,"\"%s\" ",ssid[i]);
	  free(ssid[i]);
	  ssid[i]=NULL;
	  sprintf(tmp," %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
   	  ssid_and_stambs=concat(temp2,tmp);
	  free(temp);
	  free(temp2);
	  temp=NULL;
	  temp2=NULL;
      if(save_to_new_file(ssid_and_stambs)!=0){
	    printf("ERROR");
	    return 2;
	  }
      free(ssid_and_stambs);
      ssid_and_stambs=NULL;
      i++;
	  gettimeofday(&endwtime,NULL);
	  if(save_difftime(startwtime,endwtime)!=0){
		printf("ERROR");
        fclose(file);
		return 6;
      }
      if(i>MAXARG) break;

	}
    fclose(file);
    if(remove("../data/my_wifi")!=0){
      printf("Error removing my_wifi");
      return 4; 
    }
    if(rename("../data/.my_new_wifi","../data/my_wifi")!=0){
      printf("Error renaming old file");
      return 5;
    }
  }
  rename_file();
  return 0;
}
int save_to_new_file(char *line){
  FILE *fp;
  fp=fopen("../data/.my_new_wifi","a+");
  if(fp==NULL) {
    printf("Failed to open .my_new_wifi appended mode : %s\n",strerror(errno) );
    return 1;
  }
  fprintf(fp,"%s",line);
  fclose(fp);
  return 0;
}
void rename_file()
{
  FILE *fp;
  fp=fopen("../data/my_wifi","r");
  if(fp==NULL){
    printf("Failed to open my_wifi appended mode : %s\n",strerror(errno) );
    return;
  }
  fseek(fp,0,SEEK_END);
  int size=ftell(fp);
  if(size>=80000){//80k bytes
    char buffer[100];
    time_t t; 
    struct tm tm;
    t= time(NULL);
    tm= *localtime(&t);
    sprintf(buffer,"../data/my_wifi.old-%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(fp);
    if(rename("../data/my_wifi",buffer)!=0){
	  printf("Error Change my_wifi name\n");
	}
  }
  else fclose(fp);
}

int save_difftime(struct timeval startwtime,struct timeval endwtime)
{
  FILE *fp;
  fp=fopen("difftimes","a+");
  if(fp==NULL)
  {
    printf("Failed to open difftimes appended mode : %s\n",strerror(errno) );
	return 1;
  }
  double time=(double)((endwtime.tv_usec - startwtime.tv_usec)
				/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);	
  fprintf(fp,"%fs\n",time);
  fclose(fp);
  return 0;
}

void cpu_usage(clock_t start_t,clock_t end_t)
{
  FILE *fp;
  fp=fopen("cpu_Usage","a+");
  if(fp==NULL){
    printf("Failed to open cpu_usage appended mode : %s\n",strerror(errno) );
  }
  double elapsed;
  elapsed=(double)(end_t-start_t)/CLOCKS_PER_SEC;
  fprintf(fp,"%fs\n",elapsed);
  fclose(fp);
}

