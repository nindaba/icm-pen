#include <stdio.h>
#include "pen_console.h"
#include "pen_i2c.h"
#include "icm_processor.h"

static void register_all_cmds(void) {
    register_icm_cmds();
    register_i2c_cmds();
}
void app_main(void) {
    init_console(&register_all_cmds);
}
