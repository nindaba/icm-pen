//
// Created by Arthur Nindaba on 23/03/2024.
//

#ifndef PEN_MOTION_PEN_I2C_H
#define PEN_MOTION_PEN_I2C_H

#include <stdio.h>
#include "driver/i2c_master.h"

#define ICM_ADDRESS            0x68
#define I2C_FREQ               400000
#define I2C_TIMEOUT_VALUE_MS   50
#define I2C_MASTER_SCL         19
#define I2C_MASTER_SDA         18

uint8_t i2c_init_with_default(void);

uint8_t i2c_init(void);

uint8_t i2c_read(uint8_t reg, uint8_t *data, uint8_t len);

uint8_t i2c_write(uint8_t reg, uint8_t *data, uint8_t len);

void register_i2c_cmds(void);

#endif //PEN_MOTION_PEN_I2C_H
