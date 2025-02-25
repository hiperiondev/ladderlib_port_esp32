/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>

#include "cmd_system.h"
#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ftpserver.h"
#include "hal_fs.h"
#include "hal_wifi.h"
#include "nvs_flash.h"

#include "ladder.h"
#include "ladder_print.h"
#include "port_esp32.h"

static const char* TAG = "main";

#define WIFI_SSID "HPRN-03"
#define WIFI_PASS "wifihiperion3773"

#define CONSOLE_MAX_COMMAND_LINE_LENGTH 1024
#define HISTORY_PATH                    "/littlefs/history.txt"

// registers quantity
#define QTY_M  8
#define QTY_I  8
#define QTY_Q  8
#define QTY_IW 8
#define QTY_QW 8
#define QTY_C  8
#define QTY_T  8
#define QTY_D  8
#define QTY_R  8

ladder_ctx_t ladder_ctx;
TaskHandle_t laddertsk_handle;

static bool load_demo(ladder_ctx_t* ladder_ctx);

static int ladder_status(int argc, char** argv) {
    printf("[scan time: %llu ms]\n", ladder_ctx.scan_internals.actual_scan_time);
    printf("-----------------------\n");

    printf("I0-I1-I2-I3-I4-I5-I6-I7\n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\n", ladder_ctx.memory.I[0], ladder_ctx.memory.I[1], ladder_ctx.memory.I[2], ladder_ctx.memory.I[3],
           ladder_ctx.memory.I[4], ladder_ctx.memory.I[5], ladder_ctx.memory.I[6], ladder_ctx.memory.I[7]);
    printf("\n");

    printf("M0-M1-M2-M3-M4-M5-M6-M7\n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\n", ladder_ctx.memory.M[0], ladder_ctx.memory.M[1], ladder_ctx.memory.M[2], ladder_ctx.memory.M[3],
           ladder_ctx.memory.M[4], ladder_ctx.memory.M[5], ladder_ctx.memory.M[6], ladder_ctx.memory.M[7]);
    printf("\n");

    printf("Q0-Q1-Q2-Q3-Q4-Q5-Q6-Q7\n");
    printf("%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\n", ladder_ctx.memory.Q[0], ladder_ctx.memory.Q[1], ladder_ctx.memory.Q[2], ladder_ctx.memory.Q[3],
           ladder_ctx.memory.Q[4], ladder_ctx.memory.Q[5], ladder_ctx.memory.Q[6], ladder_ctx.memory.Q[7]);
    printf("\n");

    printf("-----------------------                                                     \n");
    printf("        +----+----+-------+-----------------+\n");
    printf("        | Td | Tr |  acc  |   time_stamp    |\n");
    printf("        +----+----+-------+------- ---------+\n");
    printf("Timer 0 | %d  | %d  | %05u| %15llu |\n", ladder_ctx.memory.Td[0], ladder_ctx.memory.Tr[0], (unsigned int)ladder_ctx.timers[0].acc,
           ladder_ctx.timers[0].time_stamp);
    printf("Timer 1 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[1], ladder_ctx.memory.Tr[1], (unsigned int)ladder_ctx.timers[1].acc,
           ladder_ctx.timers[1].time_stamp);
    printf("Timer 2 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[2], ladder_ctx.memory.Tr[2], (unsigned int)ladder_ctx.timers[2].acc,
           ladder_ctx.timers[2].time_stamp);
    printf("Timer 3 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[3], ladder_ctx.memory.Tr[3], (unsigned int)ladder_ctx.timers[3].acc,
           ladder_ctx.timers[3].time_stamp);
    printf("Timer 4 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[4], ladder_ctx.memory.Tr[4], (unsigned int)ladder_ctx.timers[4].acc,
           ladder_ctx.timers[4].time_stamp);
    printf("Timer 5 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[5], ladder_ctx.memory.Tr[5], (unsigned int)ladder_ctx.timers[5].acc,
           ladder_ctx.timers[5].time_stamp);
    printf("Timer 6 | %d  | %d  | %05u | %15llu |\n", ladder_ctx.memory.Td[6], ladder_ctx.memory.Tr[6], (unsigned int)ladder_ctx.timers[6].acc,
           ladder_ctx.timers[6].time_stamp);
    printf("        +----+----+-------+-----------------+\n");

    printf("          +----+----+-------+\n");
    printf("          | Cd | Cr |   C   |\n");
    printf("          +----+----+-------+\n");
    printf("Counter 0 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[0], ladder_ctx.memory.Cr[0], (unsigned int)ladder_ctx.registers.C[0]);
    printf("Counter 1 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[1], ladder_ctx.memory.Cr[1], (unsigned int)ladder_ctx.registers.C[1]);
    printf("Counter 2 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[2], ladder_ctx.memory.Cr[2], (unsigned int)ladder_ctx.registers.C[2]);
    printf("Counter 3 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[3], ladder_ctx.memory.Cr[3], (unsigned int)ladder_ctx.registers.C[3]);
    printf("Counter 4 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[4], ladder_ctx.memory.Cr[4], (unsigned int)ladder_ctx.registers.C[4]);
    printf("Counter 5 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[5], ladder_ctx.memory.Cr[5], (unsigned int)ladder_ctx.registers.C[5]);
    printf("Counter 6 | %d  | %d  | %05u |\n", ladder_ctx.memory.Cd[6], ladder_ctx.memory.Cr[6], (unsigned int)ladder_ctx.registers.C[6]);
    printf("          +----+----+-------+\n");

    return 0;
}

static void register_ladder_status(void) {
    const esp_console_cmd_t cmd = {
        .command = "status",
        .help = "Get the current ladder status",
        .hint = NULL,
        .func = &ladder_status,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int fn_fs_ls(int argc, char** argv) {
    littlefs_ls();

    return 0;
}

static void register_fs_ls(void) {
    const esp_console_cmd_t cmd = {
        .command = "ls",
        .help = "List files",
        .hint = NULL,
        .func = &fn_fs_ls,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int ladder_dump(int argc, char** argv) {
    if (argc < 2) {
        printf(" Error: No file name\n");
        return 1;
    }

    size_t num_written = 0;

    printf("Dump to: %s\n", argv[1]);

    FILE* file = fs_open(argv[1], "wb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    num_written = fwrite(&ladder_ctx.ladder.quantity.networks, sizeof(uint32_t), 1, file);
    if (num_written != 1) {
        perror("Error writing to file");
        fclose(file);
        return 1;
    }

    num_written = fwrite(&ladder_ctx.ladder.quantity.net_rows, sizeof(uint8_t), 1, file);
    if (num_written != 1) {
        perror("Error writing to file");
        fclose(file);
        return 1;
    }

    num_written = fwrite(&ladder_ctx.ladder.quantity.net_columns, sizeof(uint8_t), 1, file);
    if (num_written != 1) {
        perror("Error writing to file");
        fclose(file);
        return 1;
    }

    for (uint32_t n = 0; n < ladder_ctx.ladder.quantity.networks; n++) {
        num_written = fwrite(&ladder_ctx.network[n].enable, sizeof(bool), 1, file);
        if (num_written != 1) {
            perror("Error writing to file");
            fclose(file);
            return 1;
        }

        for (uint32_t b = 0; b < ladder_ctx.ladder.quantity.net_columns; b++) {
            num_written = fwrite(&(ladder_ctx.network[n].bars[b]), sizeof(uint32_t), 1, file);
            if (num_written != 1) {
                perror("Error writing to file");
                fclose(file);
                return 1;
            }
        }

        for (uint32_t c = 0; c < ladder_ctx.ladder.quantity.net_columns; c++) {
            for (uint32_t r = 0; r < ladder_ctx.ladder.quantity.net_rows; r++) {
                num_written = 0;
                num_written += fwrite(&(ladder_ctx.network[n].cells[r][c].code), sizeof(uint8_t), 1, file);
                num_written += fwrite(&(ladder_ctx.network[n].cells[r][c].data_qty), sizeof(uint8_t), 1, file);
                for (uint32_t d = 0; d < ladder_ctx.network[n].cells[r][c].data_qty; d++) {
                    num_written += fwrite(&(ladder_ctx.network[n].cells[r][c].data[d]), sizeof(ladder_value_t), 1, file);
                }
            }
        }
    }

    fclose(file);

    printf("Dump OK\n");
    return 0;
}

static void register_ladder_dump(void) {
    const esp_console_cmd_t cmd = {
        .command = "dump",
        .help = "Dump networks to file",
        .hint = NULL,
        .func = &ladder_dump,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int ladder_load(int argc, char** argv) {
    if (argc < 2) {
        printf(" Error: No file name\n");
        return 1;
    }

    if (strcmp(argv[1], "demo") == 0) {
        ESP_LOGI(TAG, "Load demo program");

        if (ladder_ctx.network != NULL)
            ladder_ctx_deinit(&ladder_ctx);

        if (!ladder_ctx_init(&ladder_ctx, 6, 7, 3, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
            ESP_LOGI(TAG, "ERROR Initializing\n");
            return 1;
        }
        ladder_clear_program(&ladder_ctx);

        load_demo(&ladder_ctx);
        return 0;
    }

    size_t rn = 0;
    uint32_t nets = 0;
    uint8_t rows = 0, cols = 0;

    printf("Load from: %s\n", argv[1]);

    FILE* file = fs_open(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    rn = fread(&nets, sizeof(uint32_t), 1, file);
    if (rn != 1) {
        perror("Error reading to file");
        fclose(file);
        return 1;
    }

    rn = fread(&rows, sizeof(uint8_t), 1, file);
    if (rn != 1) {
        perror("Error reading to file");
        fclose(file);
        return 1;
    }

    rn = fread(&cols, sizeof(uint8_t), 1, file);
    if (rn != 1) {
        perror("Error reading to file");
        fclose(file);
        return 1;
    }

    printf("Networks: %d, Rows: %d, Columns: %d", (int)nets, (int)rows, (int)cols);

    if (ladder_ctx.network != NULL)
        ladder_ctx_deinit(&ladder_ctx);
    if (!ladder_ctx_init(&ladder_ctx, cols, rows, nets, QTY_M, QTY_I, QTY_Q, QTY_IW, QTY_QW, QTY_C, QTY_T, QTY_D, QTY_R)) {
        printf("ERROR Initializing\n");
        return 1;
    }

    ladder_clear_program(&ladder_ctx);

    for (uint32_t n = 0; n < ladder_ctx.ladder.quantity.networks; n++) {
        rn = fread(&ladder_ctx.network[n].enable, sizeof(bool), 1, file);
        for (uint32_t b = 0; b < ladder_ctx.ladder.quantity.net_columns; b++) {
            rn = fread(&(ladder_ctx.network[n].bars[b]), sizeof(uint32_t), 1, file);
        }

        for (uint32_t c = 0; c < ladder_ctx.ladder.quantity.net_columns; c++) {
            for (uint32_t r = 0; r < ladder_ctx.ladder.quantity.net_rows; r++) {
                rn = 0;
                uint8_t code = 0;
                rn += fread(&code, sizeof(uint8_t), 1, file);
                ladder_fn_cell(&ladder_ctx, n, r, c, code, 0);
                rn += fread(&(ladder_ctx.network[n].cells[r][c].data_qty), sizeof(uint8_t), 1, file);
                for (uint32_t d = 0; d < ladder_ctx.network[n].cells[r][c].data_qty; d++) {
                    rn += fread(&(ladder_ctx.network[n].cells[r][c].data[d]), sizeof(ladder_value_t), 1, file);
                }
            }
        }
    }

    return 0;
}

static void register_ladder_load(void) {
    const esp_console_cmd_t cmd = {
        .command = "load",
        .help = "Load networks from file",
        .hint = NULL,
        .func = &ladder_load,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int ladder_start(int argc, char** argv) {
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    // assign port functions
    ladder_ctx.hw.io.read_inputs_local = esp32_read_inputs_local;
    ladder_ctx.hw.io.write_outputs_local = esp32_write_outputs_local;
    // ladder_ctx.hw.io.read_inputs_remote = esp32_read_inputs_remote;
    // ladder_ctx.hw.io.write_outputs_remote = esp32_write_outputs_remote;
    // ladder_ctx.on.scan_end = esp32_on_scan_end;
    // ladder_ctx.on.instruction = esp32_on_instruction;
    // ladder_ctx.on.task_before = esp32_on_task_before;
    ladder_ctx.on.task_after = esp32_on_task_after;
    ladder_ctx.on.panic = esp32_on_panic;
    ladder_ctx.on.end_task = esp32_on_end_task;
    ladder_ctx.hw.time.millis = esp32_millis;
    ladder_ctx.hw.time.delay = esp32_delay;

    ESP_LOGI(TAG, "Start Task Ladder");
    if (xTaskCreatePinnedToCore(ladder_task, "ladder", 15000, (void*)&ladder_ctx, 10, &laddertsk_handle, 1) != pdPASS)
        ESP_LOGI(TAG, "ERROR: start task ladder");

    return 0;
}

static void register_ladder_start(void) {
    const esp_console_cmd_t cmd = {
        .command = "start",
        .help = "Start ladder logic",
        .hint = NULL,
        .func = &ladder_start,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int ladder_stop(int argc, char** argv) {
    ladder_ctx.ladder.state = LADDER_ST_RUNNING;

    return 0;
}

static void register_ladder_stop(void) {
    const esp_console_cmd_t cmd = {
        .command = "stop",
        .help = "Stop ladder logic",
        .hint = NULL,
        .func = &ladder_stop,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int ladder_net_print(int argc, char** argv) {
    if (ladder_ctx.network == NULL) {
        ESP_LOGI(TAG, "No program!");
        return 1;
    }

    ladder_print(ladder_ctx);

    return 0;
}

static void register_ladder_print(void) {
    const esp_console_cmd_t cmd = {
        .command = "print",
        .help = "Print ladder logic",
        .hint = NULL,
        .func = &ladder_net_print,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

//////////////////////////////////////////////////////////

static bool load_demo(ladder_ctx_t* ladder_ctx) {
    bool ret = true;

    ESP_LOGI(TAG, "blink (3 network, 7 rows, 6 columns)");

    // Network 0
    (*ladder_ctx).network[0].enable = true;

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 0, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 0, 0, LADDER_TYPE_M, 3)

        ret &= ladder_fn_cell(ladder_ctx, 0, 0, 1, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 1, 0, LADDER_TYPE_M, 2);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 2, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 2, 0, LADDER_TYPE_T, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 2, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 0, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 0, 5, 0, LADDER_TYPE_M, 1);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 0, 0, LADDER_TYPE_M, 1);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 1, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 1, 0, LADDER_TYPE_T, 1);
    ladder_cell_data(ladder_ctx, 0, 2, 1, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 2, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 2, 5, 0, LADDER_TYPE_M, 2);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 4, 0, 0, LADDER_TYPE_M, 1);

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 4, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 4, 5, 0, LADDER_TYPE_Q, 0);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 0, 0, LADDER_TYPE_Q, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 1, LADDER_INS_CTU, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 1, 0, LADDER_TYPE_C, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 1, 1, LADDER_TYPE_NONE, 5);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 0, 5, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 0, 5, 5, 0, LADDER_TYPE_M, 3);
    //------------------------------------------------------------//

    // Network 1
    (*ladder_ctx).network[1].enable = true;

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 0, 0, LADDER_TYPE_M, 3)

        ret &= ladder_fn_cell(ladder_ctx, 1, 0, 1, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 1, 0, LADDER_TYPE_M, 4);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 2, LADDER_INS_NC, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 2, 0, LADDER_TYPE_M, 5)

        ret &= ladder_fn_cell(ladder_ctx, 1, 0, 3, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 3, 0, LADDER_TYPE_T, 2);
    ladder_cell_data(ladder_ctx, 1, 0, 3, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 0, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 0, 5, 0, LADDER_TYPE_M, 6);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 0, 0, LADDER_TYPE_M, 6);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 1, LADDER_INS_TON, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 1, 0, LADDER_TYPE_T, 3);
    ladder_cell_data(ladder_ctx, 1, 2, 1, 1, LADDER_BASETIME_SEC, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 2, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 2, 5, 0, LADDER_TYPE_M, 5);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 4, 0, 0, LADDER_TYPE_M, 6);

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 4, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 4, 5, 0, LADDER_TYPE_Q, 1);

    // ----------------------------- //

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 0, 0, LADDER_TYPE_Q, 1);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 1, LADDER_INS_CTU, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 1, 0, LADDER_TYPE_C, 1);
    ladder_cell_data(ladder_ctx, 1, 5, 1, 1, LADDER_TYPE_NONE, 5);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 2, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 3, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 1, 5, 5, LADDER_INS_COIL, 0);
    ladder_cell_data(ladder_ctx, 1, 5, 5, 0, LADDER_TYPE_M, 4);

    //------------------------------------------------------------//

    // Network 2
    (*ladder_ctx).network[2].enable = true;

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 0, LADDER_INS_NO, 0);
    ladder_cell_data(ladder_ctx, 2, 0, 0, 0, LADDER_TYPE_I, 7);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 1, LADDER_INS_CONN, 0);
    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 2, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 3, LADDER_INS_CONN, 0);
    // ladder_fn_cell(ladder_ctx, 2, 0, 3, LADDER_INS_FOREIGN, 0);
    // ladder_cell_data(ladder_ctx, 2, 0, 3, 0, LADDER_TYPE_NONE, 0);
    // ladder_cell_data_cstr(ladder_ctx, 2, 0, 3, 1, "0 0 0 31 6 *");
    // ladder_cell_data(ladder_ctx, 2, 0, 3, 2, LADDER_TYPE_REAL, -45.3);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 4, LADDER_INS_CONN, 0);

    ret &= ladder_fn_cell(ladder_ctx, 2, 0, 5, LADDER_INS_COIL, 7);
    ladder_cell_data(ladder_ctx, 2, 0, 5, 0, LADDER_TYPE_Q, 4);

    return ret;
}

void app_main(void) {
    nvs_flash_init();
    fs_init();
    ESP_LOGI(TAG, "Connect WIFI\n");
    wifi_connect_sta(WIFI_SSID, WIFI_PASS);

    ESP_LOGI(TAG, "Create FTP server task");
    ftpserver_start("test", "test", "/littlefs");

    ///////////////////////////////////////////////////////

    ESP_LOGI(TAG, "Start console");

    esp_console_repl_t* repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = "ladderlib >";
    repl_config.max_cmdline_length = CONSOLE_MAX_COMMAND_LINE_LENGTH;

    repl_config.history_save_path = HISTORY_PATH;
    ESP_LOGI(TAG, "Command history enabled");

    /* Register commands */
    esp_console_register_help_command();
    register_system_common();
    register_ladder_status();
    register_fs_ls();
    register_ladder_dump();
    register_ladder_load();
    register_ladder_start();
    register_ladder_stop();
    register_ladder_print();

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ///////////////////////////////////////////////////////

    if (!ladder_ctx_init(&ladder_ctx, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)) {
        ESP_LOGI(TAG, "ERROR first initializing\n");
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}
