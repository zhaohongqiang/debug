#ifndef TDM_SI_INCLUDED_TDVEVICE_H
#define TDM_SI_INCLUDED_TDVEVICE_H

#define UNIX

/*define dll interface macro*/


#define  TDM_TDEVICEAPI



#ifdef		TDM_SI_CPLUSPLUS
extern	"C"{
#endif		/*TDM_SI_CPLUSPLUS*/


//#include "../windows/th_define.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "../comserv/comserv.h"
#include "../platabs/platabs.h"
#include "../js/tdmalloc.h"
#include "../platabs/tcomdef.h"

#include "file.h"
#include "whxfile.h"
#include "whxsock.h"
//#include "tdimage.h"

#ifdef		TDM_SI_CPLUSPLUS
}
#endif		/*TDM_SI_CPLUSPLUS*/

#endif		/*TDM_SI_INCLUDED_COMSERV_H*/
