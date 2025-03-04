/*
 * Copyright 2025 Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/ladderlib *
 *
 * This is based on other projects:
 *    PLsi (https://github.com/ElPercha/PLsi)
 *
 *    please contact their authors for more information.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <string.h>

#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_fs.h"
#include "nvs_flash.h"

#include "cmd_ladderlib.h"
#include "cmd_system.h"
#include "ladder.h"

static const char *TAG = "main";

#define CONSOLE_MAX_COMMAND_LINE_LENGTH 1024
#define HISTORY_PATH                    "/littlefs/history.txt"

ladder_ctx_t ladder_ctx;
TaskHandle_t laddertsk_handle;

//////////////////////////////////////////////////////////

void app_main(void) {
    nvs_flash_init();
    fs_init();
    ///////////////////////////////////////////////////////

    ESP_LOGI(TAG, "Start console");

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = "ladderlib >";
    repl_config.max_cmdline_length = CONSOLE_MAX_COMMAND_LINE_LENGTH;

    repl_config.history_save_path = HISTORY_PATH;
    ESP_LOGI(TAG, " (Command history enabled)");

    // register commands
    esp_console_register_help_command();

    register_system_common();
    register_ladder_status();
    register_fs_ls();
    register_fs_rm();
    register_ladder_dump();
    register_ladder_load();
    register_ladder_start();
    register_ladder_stop();
    register_connect_wifi();
    /////////////////////////

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ///////////////////////////////////////////////////////

    if (!ladder_ctx_init(&ladder_ctx, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)) {
        ESP_LOGI(TAG, "ERROR first initializing\n");
    }
}
