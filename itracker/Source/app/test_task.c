#include "board_platform.h"
#include "nrf52.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_rtc.h"
#include "nrf_rtc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "sensor.h"
#include "itracker.h"
#include "nrf_log.h"


void test_task(void * pvParameter)
{
	 uint8_t gps_rsp[128] = {0};
	 uint8_t gsm_rsp[128] = {0};	 
   double temp = 0;
   double humidity = 0;
   double pressure = 0;
   int x = 0;
   int y = 0;
   int z = 0;
   float magnetic_x = 0;
   float magnetic_y = 0;
   float magnetic_z = 0;
   float light = 0;
	 char output[50] = {0};
	 while(1)
	 {
#ifdef BEM280_TEST
			itracker_function.temperature_get(&temp);
		  memset(output,0,50);
		  sprintf(output,"%.2f",temp);
		  DPRINTF(LOG_INFO, "++++++++++++++++test begin++++++++++++++++\r\n");
		  DPRINTF(LOG_INFO, "temperature = %s\r\n",output);
		  itracker_function.humidity_get(&humidity);
		 	memset(output,0,50);
		  sprintf(output,"%.2f",humidity);
		  DPRINTF(LOG_INFO, "humidity = %s\r\n",output);	
		  itracker_function.pressure_get(&pressure);
		 	memset(output,0,50);
		  sprintf(output,"%.2f",pressure);
		  DPRINTF(LOG_INFO, "pressure = %s\r\n",output);	
#endif
#ifdef LIS3DH_TEST		 
		  itracker_function.acceleration_get(&x,&y,&z);
		 	memset(output,0,50);
		  sprintf(output,"%d,%d,%d",x,y,z);
		  DPRINTF(LOG_INFO, "acceleration x,y,z = %s\r\n",output);
#endif
#ifdef LIS2MDL_TEST	
  		itracker_function.magnetic_get(&magnetic_x,&magnetic_y,&magnetic_z);
		 	memset(output,0,50);
		  sprintf(output,"%.2f,%.2f,%.2f",magnetic_x,magnetic_y,magnetic_z);		 
		  DPRINTF(LOG_INFO, "magnetic x,y,z = %s\r\n",output);
#endif
#ifdef OPT3001_TEST
		  itracker_function.light_strength_get(&light);
		 	memset(output,0,50);
		  sprintf(output,"%.2f",light);			
 		  DPRINTF(LOG_INFO, "light strength = %s\r\n",output);	
#endif
			DPRINTF(LOG_INFO, "gsm version info = "); 
		  itracker_function.communicate_send("ATI");
		  memset(gsm_rsp,0,128);
		  itracker_function.communicate_response(gsm_rsp,128,500,GSM_TYPE_CHAR);				
		  memset(gps_rsp,0,128);
		  itracker_function.gps_get(gps_rsp,128);
			DPRINTF(LOG_INFO, "gps info = %s\r\n",gps_rsp);	
		  DPRINTF(LOG_INFO, "++++++++++++++++test end++++++++++++++++\r\n");			
		  vTaskDelay(10000);
	 }
}