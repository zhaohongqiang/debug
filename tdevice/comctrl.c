//#include "comctrl.h"
#include "tdevice.h"

#include     <stdio.h>      
#include     <stdlib.h>     
#include     <unistd.h>    
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      
#define      TRUE  1
#define      FALSE 0

/**
*@brief  ���ô���ͨ������
*@param  fd     ���� int  �򿪴��ڵ��ļ����
*@param  speed  ���� int  �����ٶ�
*@return  void
*/
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
		B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
			19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{
	int   i; 
	int   status; 
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
		if  (speed == name_arr[i]) {     
			tcflush(fd, TCIOFLUSH);     
			cfsetispeed(&Opt, speed_arr[i]);  
			cfsetospeed(&Opt, speed_arr[i]);   
			status = tcsetattr(fd, TCSANOW, &Opt);  
			if  (status != 0) {        
				return;     
			}    
			tcflush(fd,TCIOFLUSH);   
		}  
	}
}
 


/**
*@brief   ���ô�������λ��ֹͣλ��Ч��λ
*@param  fd     ����  int  �򿪵Ĵ����ļ����
*@param  databits ����  int ����λ   ȡֵ Ϊ 7 ����8
*@param  stopbits ����  int ֹͣλ   ȡֵΪ 1 ����2
*@param  parity  ����  int  Ч������ ȡֵΪN,E,O,,S
*/

int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if(tcgetattr(fd, &options) != 0)
	{ 
		return(FALSE);  
	}
	
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag  &= ~OPOST;   /*Output*/
	
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*��������λ��*/
	{   
	case 7:		
		options.c_cflag |= CS7; 
		break;
	case 8:     
		options.c_cflag |= CS8;
		break;   
	default:    
		fprintf(stderr,"Unsupported data sizen"); return (FALSE);  
	}
  switch (parity) 
  {   
	 case 'n':
	 case 'N':    
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
	case 'o':   
	case 'O':     
		options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
	case 'e':  
	case 'E':   
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S': 
	case 's':  /*as no parity*/   
	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;break;  
	default:   
		fprintf(stderr,"Unsupported parityn");    
		return (FALSE);  
	}  
/* ����ֹͣλ*/  
 switch (stopbits)
 {   
	case 1:    
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		options.c_cflag |= CSTOPB;  
	   break;
	default:    
		 fprintf(stderr,"Unsupported stop bitsn");  
		 return (FALSE); 
 } 
/* Set input parity option */ 
if (parity != 'n')   
	options.c_iflag |= INPCK; 
  tcflush(fd,TCIFLUSH);
  options.c_cc[VTIME] = 150; /* ���ó�ʱ15 seconds*/   
  options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
if(tcsetattr(fd,TCSANOW,&options) != 0)    
{ 
	return (FALSE);  
 } 
 return (TRUE);  
}


int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY  ); 
        //| O_NOCTTY | O_NDELAY	
	
	if (-1 == fd)	
	{ 			
		
		return -1;		
	}	
		
	
	return fd;
}
