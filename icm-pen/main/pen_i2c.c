//
// Created by Arthur Nindaba on 23/03/2024.
//

#include <stdio.h>
#include "driver/i2c_master.h"
#include "esp_console.h"
#include "esp_log.h"
#include "pen_i2c.h"
#include "pen_console.h"
#include "argtable3/argtable3.h"


static const char *TAG = "pen_i2c";

static i2c_master_bus_config_t i2c_mst_config;
static i2c_device_config_t dev_cfg;
static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

static int i2c_init_cmd(int argc, char **argv) {
    return i2c_init_with_default();
}

static void register_i2c_init_cmd(void) {
    const esp_console_cmd_t cmd = {
            .command = "i2c_init",
            .help = "Initializes I2C",
            .hint = NULL,
            .func = &i2c_init_cmd,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


static struct {
    struct arg_int *icm_addr;
    struct arg_int *freq;
    struct arg_end *end;
} i2c_args;

static int i2c_config_cmd(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **) &i2c_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, i2c_args.end, argv[0]);
        return 1;
    }

    dev_cfg.device_address = i2c_args.icm_addr->ival[0];
    dev_cfg.scl_speed_hz = i2c_args.freq->ival[0];

    return i2c_init();
}

static void register_i2c_config_cmd(void) {
    i2c_args.icm_addr = arg_int0("a", "icm_addr", "<addr>", "ICM device address");
    i2c_args.freq = arg_int0("f", "freq", "<freq>", "I2C frequency");
    i2c_args.end = arg_end(2);
    const esp_console_cmd_t cmd = {
            .command = "i2c_config",
            .help = "Configures I2C",
            .hint = NULL,
            .func = &i2c_config_cmd,
            .argtable = &i2c_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *reg;
    struct arg_int *len;
    struct arg_end *end;
} i2c_read_args;

static int i2c_read_cmd(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **) &i2c_read_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, i2c_read_args.end, argv[0]);
        return 1;
    }

    uint8_t reg = i2c_read_args.reg->ival[0];
    uint8_t len = i2c_read_args.len->ival[0];
    uint8_t *data = malloc(len);

    if (i2c_read(reg, data, len) != 0) {
        free(data);
        return 1;
    }

    for (int i = 0; i < len; i++) {
        printf("0x%02x ", data[i]);
    }

    printf("\n");
    free(data);
    return 0;
}

static void register_i2c_read_cmd(void) {
    i2c_read_args.reg = arg_int0("r", "reg", "<reg>", "Register to read from");
    i2c_read_args.len = arg_int0("l", "len", "<len>", "Number of bytes to read");
    i2c_read_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
            .command = "i2c_read",
            .help = "Reads from I2C",
            .hint = NULL,
            .func = &i2c_read_cmd,
            .argtable = &i2c_read_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *reg;
    struct arg_int *data;
    struct arg_int *len;
    struct arg_end *end;
} i2c_write_args;

static int i2c_write_cmd(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **) &i2c_write_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, i2c_write_args.end, argv[0]);
        return 1;
    }

    uint8_t reg = i2c_write_args.reg->ival[0];
    int *data_arg = i2c_write_args.data->ival;
    uint8_t len = i2c_write_args.len->ival[0];
    uint8_t *data = malloc(len + 1);

    for (int i = 0; i < len; i++) {
        data[i] = data_arg[i];
    }

    if (i2c_write(reg, data, len) != 0) {
        free(data);
        return 1;
    }

    free(data);
    return 0;
}

static void register_i2c_write_cmd(void) {
    i2c_write_args.reg = arg_int0("r", "reg", "<reg>", "Register to write to");
    i2c_write_args.data = arg_intn("d", "data", "<data>", 0, 16, "Data to write");
    i2c_write_args.len = arg_int0("l", "len", "<len>", "Number of bytes to write");
    i2c_write_args.end = arg_end(2);
    const esp_console_cmd_t cmd = {
            .command = "i2c_write",
            .help = "Writes to I2C",
            .hint = NULL,
            .func = &i2c_write_cmd,
            .argtable = &i2c_write_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

uint8_t i2c_init_with_default() {
    i2c_master_bus_config_t i2c_mst_config_tm = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = I2C_NUM_0,
            .scl_io_num = I2C_MASTER_SCL,
            .sda_io_num = I2C_MASTER_SDA,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
    };

    i2c_device_config_t dev_cfg_tm = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = ICM_ADDRESS,
            .scl_speed_hz = I2C_FREQ,
    };

    i2c_mst_config = i2c_mst_config_tm;
    dev_cfg = dev_cfg_tm;
    return i2c_init();
}

uint8_t i2c_init() {
    if (dev_handle != NULL) {
        i2c_master_bus_rm_device(dev_handle);
    }

    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C initialization failed: %s", esp_err_to_name(ret));
        return 1;
    }

    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C device add failed: %s", esp_err_to_name(ret));
        return 1;
    }
    return 0;
}


uint8_t i2c_read(uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret = i2c_master_transmit_receive(dev_handle,(uint8_t*)&reg, 1, data, len, I2C_TIMEOUT_VALUE_MS);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed: %s", esp_err_to_name(ret));
        return 1;
    }

    return 0;
}

uint8_t i2c_write(uint8_t reg, uint8_t *data, uint8_t len) {
    uint8_t *buf = malloc(len + 1);
    buf[0] = reg;
    for (int i = 0; i < len; i++) {
        buf[i + 1] = data[i];
    }
    esp_err_t ret = i2c_master_transmit(dev_handle, buf, len + 1, I2C_TIMEOUT_VALUE_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(ret));
        return 1;
    }

    free(buf);
    return ret;
}


void register_i2c_cmds(void) {
    register_i2c_init_cmd();
    register_i2c_config_cmd();
    register_i2c_read_cmd();
    register_i2c_write_cmd();
}