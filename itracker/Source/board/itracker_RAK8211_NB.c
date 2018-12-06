#include <stdbool.h>
#include "nrf_assert.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_platform.h"
#include <stddef.h>
#include <string.h>
#include "app_error.h"
#include "nrf_soc.h"
#include "itracker.h"
#include "bme280.h"
#include "bc95-g.h"
#include "hal_uart.h"


itracker_function_stru itracker_function;
extern GSM_RECIEVE_TYPE g_type;

uint32_t get_bme280_temp_bus(double *temp)
{
	  uint32_t ret = 1;
	  if(temp == NULL)
		{
			return 1;
		}
		ret = get_bme280_temp(temp);
	  return ret;
}

uint32_t get_bme280_humidity_bus(double *humidity)
{
	  uint32_t ret = 1;
	  if(humidity == NULL)
		{
			return 1;
		}
		ret = get_bme280_humidity(humidity);
	  return ret;
}

uint32_t get_bme280_pressure_bus(double *pressure)
{
	  uint32_t ret = 1;
	  if(pressure == NULL)
		{
			return 1;
		}
		ret = get_bme280_pressure(pressure);
	  return ret;
}

uint32_t get_lis3dh_data_bus(int *x, int *y, int *z)
{
	  uint32_t ret = 0;
	  if(x == NULL || y == NULL || z == NULL)
		{
			return 1;
		}
		ret = lis3dh_twi_init();
		if(ret != NRF_SUCCESS) 
		{
				DPRINTF(LOG_ERROR, "lis3dh_twi_init fail %d\r\n", ret);
		} 
		get_lis3dh_data(x,y,z);
	  return ret;
}

uint32_t get_lis2mdl_data_bus(float *magnetic_x, float *magnetic_y, float *magnetic_z)
{
	  uint32_t ret = 0;
	  if(magnetic_x == NULL || magnetic_y == NULL || magnetic_z == NULL)
		{
			return 1;
		}
		ret = lis2mdl_twi_init();
		if(ret != NRF_SUCCESS) 
		{
				DPRINTF(LOG_ERROR, "lis2mdl_twi_init fail %d\r\n", ret);
		}
		get_lis2mdl_data(magnetic_x,magnetic_y,magnetic_z);
	  return ret;
}

uint32_t get_opt3001_data_bus(float *light_data)
{
	  uint32_t ret = 0;
	  if(light_data == NULL)
		{
			return 1;
		}
		ret = opt3001_twi_init();
		if(ret != NRF_SUCCESS) 
		{
				DPRINTF(LOG_ERROR, "opt3001_twi_init fail %d\r\n", ret);
		}
		get_opt3001_data(light_data);
		
	  return ret;
}

uint32_t gps_data_get_bus(uint8_t *data, uint32_t len)
{
	  uint32_t ret = 0;
	  if(data == NULL || len < 0)
		{
			return 1;
		}
	  rak_uart_init(GPS_USE_UART,GPS_RXD_PIN,GPS_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud9600);
    delay_ms(2000);
		gps_data_get(data,len);
	  rak_uart_init(GSM_USE_UART,GSM_RXD_PIN,GSM_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud9600);
	  return ret;
}

void Gsm_wait_response(uint8_t *rsp, uint32_t len, uint32_t timeout,GSM_RECIEVE_TYPE type)
{
	  if(rsp == NULL || len < 0)
		{
			return;
		}
		g_type = type;
		Gsm_WaitRspOK(rsp, timeout, true);
}
void itracker_function_init()
{
	  memset(&itracker_function,0,sizeof(itracker_function));
#ifdef BEM280_TEST
	  itracker_function.temperature_get = get_bme280_temp_bus;
	  itracker_function.humidity_get = get_bme280_humidity_bus;	  
	  itracker_function.pressure_get = get_bme280_pressure_bus;
#endif
#ifdef LIS3DH_TEST
	  itracker_function.acceleration_get = get_lis3dh_data_bus;
#endif
#ifdef LIS2MDL_TEST	
	  itracker_function.magnetic_get = get_lis2mdl_data_bus;
#endif
#ifdef OPT3001_TEST
	  itracker_function.light_strength_get = get_opt3001_data_bus;
#endif
	  itracker_function.gps_get = gps_data_get_bus;
	  itracker_function.communicate_send = Gsm_print;
	  itracker_function.communicate_response = Gsm_wait_response;
}