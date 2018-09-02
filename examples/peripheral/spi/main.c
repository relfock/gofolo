/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
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

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**< SPI instance index. */
#define SPI_INSTANCE  1

/**< SPI instance. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

/* LCD resolution */
#define LCD_XRES 128
#define LCD_YRES 128
#define LCD_BYTES_LINE LCD_XRES / 8
#define LCD_BUF_SIZE LCD_YRES * LCD_BYTES_LINE

static uint8_t m_tx_buf[LCD_YRES * LCD_BYTES_LINE];
static uint8_t *ptr;

#include "lcd.c"

void clear_lcd()
{
    uint8_t data[2];
    // Clear LCD screen
    data[0] = 4;
    data[1] = 0;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, data, 2, NULL, 0));

    return;
}

void toggle_vcom()
{
    int i;
    uint8_t vcom[2];

    vcom[0] = 0x00;
    vcom[1] = 0x00;

    while(1) {
        vcom[0] = ((1 & (i / 2)) << 1);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, vcom, 2, NULL, 0));
        nrf_delay_ms(5000);
        i++;
    }
}

void write_frame(uint8_t *frame)
{
    int i;
    uint8_t tx_data[LCD_BYTES_LINE + 4];

    memset(tx_data, 0, sizeof(tx_data));
    for(i = 1; i <= LCD_YRES; ++i) {
        tx_data[0] = 1 | ((1 & (i / 2)) << 1);
        tx_data[1] = i;
        memcpy(tx_data + 2, frame + (i - 1) * LCD_BYTES_LINE, LCD_BYTES_LINE);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, LCD_BYTES_LINE + 4, NULL, 0));
    }

    return;
}

int main(void)
{
    bsp_board_leds_init();

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    int pin_number = 20;

    //Configuring a pin as output: 
    nrf_gpio_cfg_output(pin_number);

    // Turn on the LCD Display
    nrf_gpio_pin_set(pin_number); 

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
    spi_config.ss_pin = SPI_SS_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin = SPI_SCK_PIN;

    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL));

    clear_lcd();

    nrf_delay_ms(1000);

    memset(m_tx_buf, 0x00, sizeof(m_tx_buf));

    ptr = graphics;

    write_frame(ptr);

    toggle_vcom();

    return 0;
}
