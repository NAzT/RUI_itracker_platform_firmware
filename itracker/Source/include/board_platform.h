#ifndef _BOARD_PLATFORM_H
#define _BOARD_PLATFORM_H


typedef enum GSM_RECIEVE_TYPE
{
	GSM_TYPE_CHAR,
	GSM_TYPE_FILE,
}GSM_RECIEVE_TYPE;



//here define the type of itracker and just retain one

//#define RAK8212
#define RAK8212_M
//#define RAK8211_G
//#define RAK8211_NB

#ifdef RAK8212
#include "board_RAK8212.h"
#endif

#ifdef RAK8212_M
#include "board_RAK8212_M.h"
#endif

#ifdef RAK8211_G
#include "board_RAK8211_G.h"
#endif

#ifdef RAK8211_NB
#include "board_RAK8211_NB.h"
#endif
#endif
