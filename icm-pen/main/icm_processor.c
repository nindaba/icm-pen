//
// Created by Arthur Nindaba on 23/03/2024.
//

#include "icm_processor.h"
#include "icm20948.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

static void icm_init() {
    icm20948_init();
//    ak09916_init();
}

static int icm_init_cmd(int argc, char **argv) {
    icm_init();
    return 0;
}

static void register_icm_init_cmds(void) {
    const esp_console_cmd_t cmd = {
            .command = "icm_init",
            .help = "Initializes ICM",
            .hint = NULL,
            .func = &icm_init_cmd,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static void format_csv_line(char *line, axises *acc, axises *gyr, axises *mgn) {
    sprintf(line, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
            acc->x, acc->y, acc->z,
            gyr->x, gyr->y, gyr->z,
            mgn->x, mgn->y, mgn->z);
}

static struct {
    struct arg_int *number_of_reads;
    struct arg_end *end;
} i2c_args;

static int read_icm_cmd(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **) &i2c_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, i2c_args.end, argv[0]);
        return 1;
    }

    int repeats = i2c_args.number_of_reads->ival[0];

    if(repeats < 1) {
        printf("Number of values to be read must be greater than 0 falling back to DEFAULT_NUM_OF_READINGS\r\n");
        repeats = DEFAULT_NUM_OF_READINGS;
    }

    printf("initializing ICM\r\n");
    icm_init();
    printf("Done initializing ICM\r\n");
    printf("Starting Readings\r\n");

    axises gyr, acc;
    char line[CSV_LINE_LENGTH];

    axises mag = {
            .x = 0,
            .y = 0,
            .z = 0
    };

    for (int i = 0; i < repeats; ++i) {
        icm20948_gyro_read_dps(&gyr);
        icm20948_accel_read_g(&acc);
//        ak09916_mag_read_uT(&mag);

        format_csv_line(line, &acc, &gyr, &mag);
        printf("%s\r", line);
    }
    return 0;
}

static void register_icm_read_cmds(void) {
    i2c_args.number_of_reads = arg_int0("r", "reads-count", "<repeats>", "Number of readings to take");
    i2c_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
            .command = "icm_read",
            .help = "Number of cycles to read from ICM",
            .hint = NULL,
            .func = &read_icm_cmd,
            .argtable = &i2c_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int send_values_to_wifi_cmd(int argc, char **argv) {
    send_values_to_wifi();
    return 0;
}

void send_values_to_wifi() {
    printf("Starting Readings\r\n");
    axises gyr, acc, mag;
    char line[CSV_LINE_LENGTH];

    while (1) {
        icm20948_gyro_read_dps(&gyr);
        icm20948_accel_read_g(&acc);
        ak09916_mag_read_uT(&mag);

        format_csv_line(line, &acc, &gyr, &mag);
        //send to wifi
        printf("%s\r", line);
    }

}

void register_icm_cmds(void) {
    register_icm_init_cmds();
    register_icm_read_cmds();
}
