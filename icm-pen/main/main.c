#include <stdio.h>
#include "pen_console.h"
#include "pen_i2c.h"
#include "icm_processor.h"
//#include "wifi_smartconfig.h"

static void register_all_cmds(void) {
    register_icm_cmds();
    register_i2c_cmds();
}

static void init_connections(void) {
    i2c_init();
//    wifi_init();
}

void app_main(void) {
    init_connections();
    init_console(&register_all_cmds);
}
