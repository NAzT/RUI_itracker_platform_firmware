/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/** @file
 *
 * @defgroup bootloader_secure_ble main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file for secure DFU.
 *
 */

#include <stdint.h>
#include "boards.h"
#include "nrf_mbr.h"
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_dfu.h"
#include "nrf_dfu_settings.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "app_error_weak.h"
#include "nrf_bootloader_info.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_dfu_utils.h"
#include "crc32.h"

extern uint8_t m_dfu_settings_buffer[BOOTLOADER_SETTINGS_PAGE_SIZE];
static void on_error(void)
{
    NRF_LOG_FINAL_FLUSH();

#if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
    // To allow the buffer to be flushed by the host.
    nrf_delay_ms(100);
#endif
#ifdef NRF_DFU_DEBUG_VERSION
    NRF_BREAKPOINT_COND;
#endif
    NVIC_SystemReset();
}

extern nrf_dfu_settings_t s_dfu_settings;

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    NRF_LOG_ERROR("%s:%d", p_file_name, line_num);
    on_error();
}


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    NRF_LOG_ERROR("Received a fault! id: 0x%08x, pc: 0x%08x, info: 0x%08x", id, pc, info);
    on_error();
}


void app_error_handler_bare(uint32_t error_code)
{
    NRF_LOG_ERROR("Received an error: 0x%08x!", error_code);
    on_error();
}

/**
 * @brief Function notifies certain events in DFU process.
 */
static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type)
    {
        case NRF_DFU_EVT_DFU_FAILED:
        case NRF_DFU_EVT_DFU_ABORTED:
        case NRF_DFU_EVT_DFU_INITIALIZED:
            bsp_board_init(BSP_INIT_LEDS);
            bsp_board_led_on(BSP_BOARD_LED_0);
            bsp_board_led_on(BSP_BOARD_LED_1);
            bsp_board_led_off(BSP_BOARD_LED_2);
            break;
        case NRF_DFU_EVT_TRANSPORT_ACTIVATED:
            bsp_board_led_off(BSP_BOARD_LED_1);
            bsp_board_led_on(BSP_BOARD_LED_2);
            break;
        case NRF_DFU_EVT_DFU_STARTED:
            break;
        default:
            break;
    }
}

void dfu_settings_init(void)
{
    memset(&s_dfu_settings, 0x00, sizeof(nrf_dfu_settings_t));
	  // Copy the DFU settings out of flash and into a buffer in RAM.
    memcpy((void*)&s_dfu_settings, m_dfu_settings_buffer, sizeof(nrf_dfu_settings_t));
	  s_dfu_settings.crc  = 0xFFFFFFF;
	  s_dfu_settings.settings_version = 0x01;
	  s_dfu_settings.bootloader_version = 0x01;
	  s_dfu_settings.app_version = 0x01;
	  s_dfu_settings.bank_layout = 0;
	  s_dfu_settings.bank_1.image_size = 120*1024;
	  s_dfu_settings.progress.update_start_address = 0x0004B800;
	  s_dfu_settings.write_offset = 0;
} 

void dfu_settings_deinit(void)
{
    memset(&s_dfu_settings, 0x00, sizeof(nrf_dfu_settings_t));
} 

uint32_t firmware_copy(uint32_t dst_addr,
                           uint32_t src_addr,
                           uint32_t size,
                           uint32_t progress_update_step)
{
    if (src_addr == dst_addr)
    {
        NRF_LOG_DEBUG("No copy needed src_addr: 0x%x, dst_addr: 0x%x", src_addr, dst_addr);
        return NRF_SUCCESS;
    }

    ASSERT(src_addr >= dst_addr);
    ASSERT(progress_update_step > 0);
    ASSERT((dst_addr % CODE_PAGE_SIZE) == 0);

    uint32_t max_safe_progress_upd_step = (src_addr - dst_addr)/CODE_PAGE_SIZE;  // 37 yu 2
    ASSERT(max_safe_progress_upd_step > 0);

    uint32_t ret_val = NRF_SUCCESS;
    uint32_t pages_left = CEIL_DIV(size, CODE_PAGE_SIZE); //38 yu 1

    progress_update_step = MIN(progress_update_step, max_safe_progress_upd_step);
		
		//nrf_dfu_flash_erase(dst_addr, 30*CODE_PAGE_SIZE, NULL);
    //nrf_dfu_flash_store(dst_addr,(uint32_t *)src_addr,ALIGN_NUM(sizeof(uint32_t), 30*CODE_PAGE_SIZE),NULL);
    while (size > 0)
    {
        uint32_t pages;
        uint32_t bytes;
        if (pages_left <= progress_update_step)
        {
            pages = pages_left;
            bytes = size;
        }
        else
        {
            pages = progress_update_step;
            bytes = progress_update_step * CODE_PAGE_SIZE;
        }
        // Erase the target pages
        ret_val = nrf_dfu_flash_erase(dst_addr, pages, NULL);
        if (ret_val != NRF_SUCCESS)
        {
            return ret_val;
        }

        // Flash one page
        NRF_LOG_DEBUG("Copying 0x%x to 0x%x, size: 0x%x", src_addr, dst_addr, bytes);
        ret_val = nrf_dfu_flash_store(dst_addr,
                                      (uint32_t *)src_addr,
                                      ALIGN_NUM(sizeof(uint32_t), bytes),
                                      NULL);
        if (ret_val != NRF_SUCCESS)
        {
            return ret_val;
        }

        pages_left  -= pages;
        size        -= bytes;
        dst_addr    += bytes;
        src_addr    += bytes;
        s_dfu_settings.write_offset += bytes;
    }

    return ret_val;
}


void firmware_activate(void)
{
    // This function is only in use when new app is present in Bank 1
    uint32_t const image_size  = s_dfu_settings.bank_1.image_size;

    uint32_t src_addr    = s_dfu_settings.progress.update_start_address;
    uint32_t ret_val     = NRF_SUCCESS;
    uint32_t target_addr = nrf_dfu_bank0_start_addr() + s_dfu_settings.write_offset;
    uint32_t length_left = (image_size - s_dfu_settings.write_offset);
    uint32_t crc;

    NRF_LOG_DEBUG("Enter nrf_dfu_app_continue");

    src_addr += s_dfu_settings.write_offset;

    if (src_addr == target_addr)
    {
        length_left = 0;
    }

    ret_val = firmware_copy(target_addr, src_addr, length_left, NRF_BL_FW_COPY_PROGRESS_STORE_STEP);
    if (ret_val != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed to copy firmware.");
        return;
    }

    return;
}



/**@brief Function for application main entry. */
int main(void)
{
    uint32_t ret_val;

    // Protect MBR and bootloader code from being overwritten.
    //ret_val = nrf_bootloader_flash_protect(0, MBR_SIZE, false);
    //APP_ERROR_CHECK(ret_val);
    //ret_val = nrf_bootloader_flash_protect(BOOTLOADER_START_ADDR, BOOTLOADER_SIZE, false);
    //APP_ERROR_CHECK(ret_val);

    (void) NRF_LOG_INIT(app_timer_cnt_get);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Inside main");

    //ret_val = nrf_bootloader_init(NULL);
    //APP_ERROR_CHECK(ret_val);
	
	  nrf_dfu_flash_init(false);
	  //1* init settings
    dfu_settings_init();

	  if(s_dfu_settings.bank_1.bank_code == NRF_DFU_BANK_VALID_APP)
		{
			//2* copy firmware
	    firmware_activate();
		}

    s_dfu_settings.bank_1.bank_code = NRF_DFU_BANK_INVALID;

    nrf_dfu_settings_write(NULL);
    // Boot the main application.
    nrf_bootloader_app_start();

    // Should never be reached.
    NRF_LOG_INFO("After main");
}

/**
 * @}
 */
