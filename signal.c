#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/signal.h>  
#include <fcntl.h>  
#include <termios.h>  
#include <errno.h>  
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h> 
#include <netdb.h>
  
#define FALSE -1  
#define TRUE 0  
#define flag 1  
#define noflag 0  
  
int wait_flag = noflag;  
int STOP = 0;  
int res;  

struct recv_parm
{
  int fd;
  int ffd;
  FILE *file;
};
int fd1 = 0;

int speed_arr[] =  
  { B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600,  
B4800, B2400, B1200, B300, };  
int name_arr[] =  
  { 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400,  
1200, 300, };  

int  set_speed (int fd, int speed)  
{  
  printf("%s\n", __FUNCTION__);
  int i;  
  int status;  
  struct termios Opt;  
  tcgetattr (fd, &Opt);  
  for (i = 0; i < sizeof (speed_arr) / sizeof (int); i++)  
    {  
      if (speed == name_arr[i])  
    {  
      tcflush (fd, TCIOFLUSH);  
      cfsetispeed (&Opt, speed_arr[i]);  
      cfsetospeed (&Opt, speed_arr[i]);  
      status = tcsetattr (fd, TCSANOW, &Opt);  
      if (status != 0)  
        {  
          perror ("tcsetattr fd1");  
          return;  
        }  
      tcflush (fd, TCIOFLUSH);  
    }  
    }  
    return fd;
}  
  
int  set_Parity (int fd, int databits, int stopbits, int parity)  
{  
  printf("%s\n", __FUNCTION__);
  struct termios options;  
  if (tcgetattr (fd, &options) != 0)  
    {  
      perror ("SetupSerial 1");  
      return (FALSE);  
    }  
  options.c_cflag &= ~CSIZE;  
  switch (databits)  
    {  
    case 7:  
      options.c_cflag |= CS7;  
      break;  
    case 8:  
      options.c_cflag |= CS8;  
      break;  
    default:  
      fprintf (stderr, "Unsupported data size\n");  
      return (FALSE);  
    }  
  switch (parity)  
    {  
    case 'n':  
    case 'N':  
      options.c_cflag &= ~PARENB;   /* Clear parity enable */  
      options.c_iflag &= ~INPCK;    /* Enable parity checking */  
      break;  
    case 'o':  
    case 'O':  
      options.c_cflag |= (PARODD | PARENB);  
      options.c_iflag |= INPCK; /* Disnable parity checking */  
      break;  
    case 'e':  
    case 'E':  
      options.c_cflag |= PARENB;    /* Enable parity */  
      options.c_cflag &= ~PARODD;  
      options.c_iflag |= INPCK; /* Disnable parity checking */  
      break;  
    case 'S':  
    case 's':           /*as no parity */  
      options.c_cflag &= ~PARENB;  
      options.c_cflag &= ~CSTOPB;  
      break;  
    default:  
      fprintf (stderr, "Unsupported parity\n");  
      return (FALSE);  
    }  
  
  switch (stopbits)  
    {  
    case 1:  
      options.c_cflag &= ~CSTOPB;  
      break;  
    case 2:  
      options.c_cflag |= CSTOPB;  
      break;  
    default:  
      fprintf (stderr, "Unsupported stop bits\n");  
      return (FALSE);  
    }  
  /* Set input parity option */  
  if (parity != 'n')  
    options.c_iflag |= INPCK;  
  tcflush (fd, TCIFLUSH);  
  options.c_cc[VTIME] = 150;  
  options.c_cc[VMIN] = 0;   /* Update the options and do it NOW */  
  if (tcsetattr (fd, TCSANOW, &options) != 0)  
    {  
      perror ("SetupSerial 3");  
      return (FALSE);  
    }  
  return (TRUE);  
}  

int find_port(char *port_phefix)
{
  printf("%s\n", __FUNCTION__);
  int USBNum = 0;
  //int USBNum1 = 1;
  int fid = 0;
  char dir[50];
  while(1)
  {
    if(USBNum < 10)
    {
      sprintf(dir,"%s%d",port_phefix,USBNum);
      //printf("dir = %s\n", dir);
      //sprintf(dir1,"%s%d",port_phefix,USBNum1);
      fid = open (dir, O_RDWR);
      //fid = open(dir1,O_RDWR);
      if(fid < 0)
      {
        USBNum++;
        //USBNum1++;
      }
      else
      {
      	USBNum++;
      	sprintf(dir,"%s%d",port_phefix,USBNum);
      	fd1 = open(dir,O_RDWR);
        //printf("open%s SUCCESS!",dir);
        //printf ("%s", ttyname (fid));       
        break;
      }
    }
    else
    {
      return -1;
    }
  }
   return fid;
}

void build_datafile(char *pathname)
{
  printf("%s\n", __FUNCTION__);
  time_t now;
  struct tm *timenow;
  time(&now);
  timenow = localtime(&now);
  char *CharTime = NULL;
  CharTime = asctime(timenow);

  int k;
  for(k=0;CharTime[k]!='\0';k++)
  {
    pathname[k]=CharTime[k];
  }
  pathname[k+1]='.';
  pathname[k+2]='t';
  pathname[k+3]='x';
  pathname[k+4]='t';
  pathname[k+5]='\0'; 
  //printf("pathname=%s",pathname);
}

void send_data(int fd)
{
  char buff[] = "#01\n";
  int nwrite = 0;
  while(1)
  {
    if(nwrite = write(fd,buff,5)>0)
    {
      printf("\nsend success!%s == %d\n",buff, nwrite);
    } 
    else
    {
      printf("\nsend error\n");
      return;
    }
    sleep(30);
  }
}

void recv_data(void *data)
{
  struct recv_parm *tt = (struct recv_parm*)data ;
 // tt = (struct recv_parm*)malloc(sizeof(struct recv_parm));
  int fid = 0;
  int fd_D = 0;
  FILE *file1 = NULL;
 // tt->file = (FILE*)malloc(sizeof(FILE));
  fid = tt->fd;
  fd_D = tt->ffd;
  file1 = tt->file;
  //printf("%s, fd = %d\n", __FUNCTION__, fd);
  char buf[255] = ""; 
  char dtu_buf[50] = "";
  char *dtu_data1 = NULL;
  char *dtu_data2 = NULL;
  char *p = NULL;
  char *data_buff[9] = {0};
  char CharTime[20] = "";
  char colon[2] = ":";
  char comma[2] = ",";
  char head[10] = "#RTU";
  char enter[5] = "\n";

  time_t now;
  struct tm *timenow;

  while(1)
  {         
    int nread = 0; 
    int nwrt = 0; 
    int length = 0;
    float data_ph = 0.0;
    float data_tem = 0.0;
    while((nread = read(fid, buf, sizeof(buf))) > 0)
    {
      time(&now);
      timenow = localtime(&now);
      sprintf(CharTime,"%d%s%d%s%d",timenow->tm_hour,colon,timenow->tm_min,colon,timenow->tm_sec); 
      //printf("%s\n",CharTime); 

      printf("nread = %d,%s",nread, buf);
      p = strtok(buf,"+-");
      int i = 0;
      while(p)
      {
        data_buff[i] = p;
        p = strtok(NULL, "+-");
        i++;
      }            
      for(i=0; i<9; i++)
      {
        //printf("data[%d]%s\t",i,data_buff[i]); 
        switch(i)
        {
        	case 1:
        	dtu_data1 = data_buff[i];
        	case 2:
        	dtu_data2 = data_buff[i];
        }
        if (i==0)
        {
          fwrite(CharTime, sizeof(char),strlen(CharTime),file1);           /* code */
        } 
        else
        {
          fwrite(data_buff[i], sizeof(char),strlen(data_buff[i]),file1);
        }       
        fwrite("\t", sizeof(char),1,file1);           
      }
      data_ph = atof(dtu_data1) * 0.9458 - 3.784;
      data_tem = atof(dtu_data2) * 6.301 - 75.52;

      // sprintf(dtu_buf,"%s%s%s%s%s%s",head,comma,dtu_data1,comma,dtu_data2,enter);
      // printf("data_ph = %f\n",data_ph);
      // printf("data_tem = %f\n",data_tem);
      sprintf(dtu_buf,"%s%s%f%s%f%s",head,comma,data_ph,comma,data_tem,enter);
      length = strlen(dtu_buf);
      //printf("dtu_buf = %s\n",dtu_buf);
      if(nwrt = write(fd_D,dtu_buf,length)>0)
      {
      	printf("nwrt = %d",nwrt);
      }

      fwrite("\n",sizeof(char),1,file1);
      fflush(file1);
      //fclose(file1);    
       memset(buf, 0 , sizeof(buf));      
       memset(data_buff,0,sizeof(data_buff)); 
       memset(dtu_data1,0,sizeof(dtu_data1));
       memset(dtu_data2,0,sizeof(dtu_data2)); 
       memset(dtu_buf,0,sizeof(dtu_buf));                                                                                                                             
    }
  }
}

void recv_data1(int fd,FILE *file)
{
  char buf[255] = ""; 
  char *p = NULL;
  char *data_buff[9] = {0};
  while(1)
  {             

    int nread = 0;  
    while((nread = read(fd, buf, sizeof(buf))) > 0)
    {
      printf("nread = %d,%s",nread, buf);
      p = strtok(buf,"+-");
      int i = 0;
      while(p)
      {
        data_buff[i] = p;
        p = strtok(NULL, "+-");
        i++;
      }            
      for(i=0; i<9; i++)
      {
        printf("data[%d]%s\t",i,data_buff[i]);  
        fwrite(data_buff[i], sizeof(char),strlen(data_buff[i]),file);
        fwrite("\t", sizeof(char),1,file);    
        // fflush(file);
      }
      fclose(file);    
      memset(buf, 0 , sizeof(buf));                                                                                                                                     
    }
  }
}


  
int  main (int argc, char *argv[])  
{  
  printf ("This program updates last time at %s   %s\n", __TIME__, __DATE__);  
  printf ("STDIO COM1\n"); 
  int fd = 0 ;
  int fd_DAM = 0;
  int fd_DTU = 0;
  char root[20] = "/dev/ttyUSB"; 

  //打开两个串口设备
  fd = find_port(root);
  //fd = open("/dev/ttyUSB1",O_RDWR);
  //fd1 = open("/dev/ttyUSB2",O_RDWR);
  printf("LINE = %d, fd = %d,fd1 = %d\n", __LINE__, fd,fd1);
 /* fd = open(argv[1], O_RDWR );
  if (fd < 0)
  {
  	perror("open error:");
  }*/
  set_speed (fd, 9600); 
  set_speed (fd1, 38400);
  if (set_Parity (fd, 8, 1, 'N') == FALSE)  
  {  
    printf ("Set Parity Error\n");  
    exit (-1);  
  }  

  if (set_Parity (fd1, 8, 1, 'N') == FALSE)  
  {  
    printf ("Set Parity Error\n");  
    exit (-1);  
  }  

  char *filename;
  filename = (char *)malloc(100);
  build_datafile(filename);
  FILE *file;
  if ((file = fopen(filename, "w+")) == NULL) 
  { 
      printf("failed to open test.txt\n"); 

      exit(-1); 
  } 
  
  printf("fd1=%d\n",fd );

  //识别DAM与DTU设备串口
  char test_buf[] = "#01\n"; 
  char recv_buf[255] = "";
  int n_read = 0;
  write(fd,test_buf,5);
  write(fd1,test_buf,5);
  while(1)
  {
  	if((n_read = read(fd, recv_buf, sizeof(recv_buf))) > 0)
  	{
  		fd_DAM = fd;
  		fd_DTU = fd1;
  		printf("fd_DAM = %d,fd_DTU = %d\n",fd_DAM,fd_DTU);
  		break;
  	}
  	else if ((n_read = read(fd1, recv_buf, sizeof(recv_buf))) > 0)
  	{
  		fd_DAM = fd1;
  		fd_DTU = fd;
  		printf("fd_DAM = %d,fd_DTU = %d\n",fd_DAM,fd_DTU);
  		break;
  	}
  	break;
  }




  struct recv_parm *test;
  test = (struct recv_parm *)malloc(sizeof(struct recv_parm));

  test->fd = fd_DAM;
  test->ffd = fd_DTU;
  test->file = file;

  //printf("fd2=%d\n",test->fd);
  // int test_wrt = 0;
  // int test_resd = 0;
  // char test_send[] = "#RTU,10,10\n";
  // printf("LINE = %d\n", __LINE__);
  // write(fd_DTU,test_send,sizeof(test_send));

  pthread_t send,recv;
  pthread_create(&send,NULL,(void*)send_data,(void *)fd_DAM);
  pthread_create(&recv,NULL,(void*)recv_data,(void *)test);
  pthread_join(send,NULL);
  pthread_join(recv,NULL);

  //send_data(fd);
  //recv_data1(fd,file);

  //recv_data((void*)test);

  // char buf[255] = ""; 
  // char *p = NULL,*p1 = NULL;
  // char *data_buff[9] = {0};

  // while(1)
  // {
  //   int nread = 0;  
  //   while((nread = read(fd, buf, sizeof(buf))) > 0)
  //   {
  //     printf("nread = %d,%s",nread, buf);
  //     p = strtok(buf,"+-");
  //     int i = 0;
  //     while(p)
  //     {
  //       data_buff[i] = p;
  //       p = strtok(NULL, "+-");
  //       i++;
  //     }            
  //     for(i=0; i<9; i++)
  //     {
  //       printf("data[%d]%s\t",i,data_buff[i]);  
  //       fwrite(data_buff[i], sizeof(char),strlen(data_buff[i]),file);
  //       fwrite("\t", sizeof(char),1,file);    
  //       // fflush(file);
  //     }
  //     fclose(file);    
  //     memset(buf, 0 , sizeof(buf));                                                                                                                                     

  //   }
  // } 
  close (fd);  
  return 0;  
}  