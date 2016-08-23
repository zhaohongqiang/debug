#ifndef		TDM_SI_INCLUDED_COMCTRL_H
#define		TDM_SI_INCLUDED_COMCTRL_H
#include "tdevice.h"

#ifdef		TDM_SI_CPLUSPLUS
extern	"C"{
#endif		/*TDM_SI_CPLUSPLUS*/

//TDM_TDEVICEAPI
// TDVOID	TDDebugFunction(char * file,int  line,char * message);
TDM_TDEVICEAPI int OpenDev(char *Dev);
TDM_TDEVICEAPI int set_Parity(int fd,int databits,int stopbits,int parity);
TDM_TDEVICEAPI void set_speed(int fd, int speed);

#ifdef		TDM_SI_CPLUSPLUS
	}
#endif		/*TDM_SI_CPLUSPLUS*/
#endif
