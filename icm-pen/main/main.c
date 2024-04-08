#include <stdio.h>
#include "pen_console.h"
#include "pen_i2c.h"
#include "icm_processor.h"
#include "wifi_smartconfig.h"
#include "udp_server.h"

static void register_all_cmds(void) {
    register_icm_cmds();
    register_i2c_cmds();
}

static void init_connections(void) {
    i2c_init_with_default();
    icm_init();
    wifi_init();
    udp_server_init(&read_all_sensor_values);
}

void app_main(void) {
    init_connections();
    init_console(&register_all_cmds);
}
