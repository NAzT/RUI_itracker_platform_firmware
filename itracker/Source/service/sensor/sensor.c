#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "app_timer.h"
#include "fds.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_platform.h"
#include "nrf_power.h"
#include "hal_uart.h"


void sensors_init()
{
		int ret;
#ifdef GSM_TEST
		// init gsm
    Gsm_Init();
#endif
#ifdef GPS_TEST
	  // init gps
	  Gps_Init();
#endif

#ifdef BEM280_TEST
		ret = bme280_spi_init();
		if(ret != NRF_SUCCESS) {
				DPRINTF(LOG_ERROR, "bme280_spi_init fail %d\r\n", ret);
		} else {
				ret = _bme280_init();
				if(ret < 0) {
						DPRINTF(LOG_ERROR, "lis3dh_init fail\r\n");
				}
		}
#endif
#ifdef LIS3DH_TEST
		ret = lis3dh_twi_init();
		if(ret != NRF_SUCCESS) {
				DPRINTF(LOG_ERROR, "lis3dh_twi_init fail %d\r\n", ret);
		} else {
				ret = lis3dh_init();
				if(ret < 0) {
						DPRINTF(LOG_ERROR, "lis3dh_init fail\r\n");
				}
		}
#endif
#ifdef LIS2MDL_TEST
		ret = lis2mdl_twi_init();
		if(ret < 0) {
				DPRINTF(LOG_ERROR, "lis2mdl_twi_init fail %d\r\n", ret);
		} else {
				ret = lis2mdl_init();
				if(ret < 0) {
						DPRINTF(LOG_ERROR, "lis2mdl_init fail\r\n");
				}
		}
#endif
#ifdef OPT3001_TEST
		ret = opt3001_twi_init();
		if(ret < 0) {
				DPRINTF(LOG_ERROR, "opt3001_twi_init fail %d\r\n", ret);
		} else {
				ret = opt3001_init();
				if(ret < 0) {
						DPRINTF(LOG_ERROR, "opt3001_init fail\r\n");
				}
		}
#endif
		
}
