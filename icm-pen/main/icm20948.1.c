
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>

/* bl808 c906 std driver */
#include <bl808_glb.h>
#include <bl808_i2c.h>
#include "cst816x_reg.h"

#include "icm20948.1.h"

//protected:


void ICM20948_set_clock_to_auto_select(ICM20948 *icm);

void ICM20948_correct_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val);

void ICM20948_correct_gyr_raw_values(ICM20948 *icm, XYZ_float *gyr_raw_val);

void ICM20948_switch_bank(ICM20948 *icm, uint8_t new_bank);

void ICM20948_write_register8(ICM20948 *icm, uint8_t bank, uint8_t reg, uint8_t val);

void ICM20948_write_register16(ICM20948 *icm, uint8_t bank, uint8_t reg, int16_t val);

uint8_t ICM20948_read_register8(ICM20948 *icm, uint8_t bank, uint8_t reg);

int16_t ICM20948_read_register16(ICM20948 *icm, uint8_t bank, uint8_t reg);

void ICM20948_read_all_data(ICM20948 *icm, uint8_t *data);

void ICM20948_read_XYZ_val_from_fifo(ICM20948 *icm, XYZ_float  *values);

void ICM20948_write_AK09916_register8(ICM20948 *icm, uint8_t reg, uint8_t val);

uint8_t ICM20948_read_AK09916_register8(ICM20948 *icm, uint8_t reg);

int16_t ICM20948_read_AK09916_register16(ICM20948 *icm, uint8_t reg);

void ICM20948_reset(ICM20948 *icm);

void ICM20948_enable_i2c_master(ICM20948 *icm);

void ICM20948_enable_mag_data_read(ICM20948 *icm, uint8_t reg, uint8_t bytes);

float sq(float x);
/************ Basic Settings ************/

bool ICM20948_init(ICM20948 *icm) {
    icm->current_bank = 0;

    ICM20948_reset(icm);

    if (ICM20948_who_am_i(icm) != ICM20948_WHO_AM_I_CONTENT) {
        return false;
    }

    icm->acc_offset_val.x = 0.0;
    icm->acc_offset_val.y = 0.0;
    icm->acc_offset_val.z = 0.0;
    icm->acc_corr_factor.x = 1.0;
    icm->acc_corr_factor.y = 1.0;
    icm->acc_corr_factor.z = 1.0;
    icm->acc_range_factor = 1.0;
    icm->gyr_offset_val.x = 0.0;
    icm->gyr_offset_val.y = 0.0;
    icm->gyr_offset_val.z = 0.0;
    icm->gyr_range_factor = 1.0;
    icm->fifo_type = ICM20948_FIFO_ACC;

    ICM20948_sleep(icm, false);
    ICM20948_write_register8(icm, 2, ICM20948_ODR_ALIGN_EN, 1); // aligns ODR

    return true;
}

void ICM20948_auto_offsets(ICM20948 *icm) {
    XYZ_float acc_raw_val, gyr_raw_val;
    uint8_t samples = 50;
    icm->acc_offset_val.x = 0.0;
    icm->acc_offset_val.y = 0.0;
    icm->acc_offset_val.z = 0.0;

    ICM20948_set_gyr_dlpf(icm, ICM20948_DLPF_6); // lowest noise
    ICM20948_set_gyr_range(icm, ICM20948_GYRO_RANGE_250); // highest resolution
    ICM20948_set_acc_range(icm, ICM20948_ACC_RANGE_2G);
    ICM20948_set_acc_dlpf(icm, ICM20948_DLPF_6);

    vTaskDelay(100);

    for (int i = 0; i < samples; i++) {
//        ICM20948_read_sensor(icm);
        ICM20948_get_acc_raw_values(icm,&acc_raw_val);

        icm->acc_offset_val.x += acc_raw_val.x;
        icm->acc_offset_val.y += acc_raw_val.y;
        icm->acc_offset_val.z += acc_raw_val.z;

        vTaskDelay(10);
    }

    icm->acc_offset_val.x /= samples;
    icm->acc_offset_val.y /= samples;
    icm->acc_offset_val.z /= samples;
    icm->acc_offset_val.z -= 16384.0;

    for (int i = 0; i < samples; i++) {
        ICM20948_read_sensor(icm);
        ICM20948_get_gyr_raw_values(icm,&gyr_raw_val);
        icm->gyr_offset_val.x += gyr_raw_val.x;
        icm->gyr_offset_val.y += gyr_raw_val.y;
        icm->gyr_offset_val.z += gyr_raw_val.z;

        vTaskDelay(100);
    }

    icm->gyr_offset_val.x /= samples;
    icm->gyr_offset_val.y /= samples;
    icm->gyr_offset_val.z /= samples;

}

void
ICM20948_set_acc_offsets(ICM20948 *icm, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {

    icm->acc_offset_val.x = (x_max + x_min) * 0.5;
    icm->acc_offset_val.y = (y_max + y_min) * 0.5;
    icm->acc_offset_val.z = (z_max + z_min) * 0.5;
    icm->acc_corr_factor.x = (x_max + fabsf(x_min)) / 32768.0;
    icm->acc_corr_factor.y = (y_max + fabsf(y_min)) / 32768.0;
    icm->acc_corr_factor.z = (z_max + fabsf(z_min)) / 32768.0;

}

void ICM20948_set_gyr_offsets(ICM20948 *icm, float x_offset, float y_offset, float z_offset) {
    icm->gyr_offset_val.x = x_offset;
    icm->gyr_offset_val.y = y_offset;
    icm->gyr_offset_val.z = z_offset;
}

uint8_t ICM20948_who_am_i(ICM20948 *icm) {
    return ICM20948_read_register8(icm, 0, ICM20948_WHO_AM_I);
}

void ICM20948_enable_acc(ICM20948 *icm, bool en_acc) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_PWR_MGMT_2);
    if (en_acc) {
        icm->reg_val &= ~ICM20948_ACC_EN;
    } else {
        icm->reg_val |= ICM20948_ACC_EN;
    }
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_2, icm->reg_val);
}


void ICM20948_set_acc_range(ICM20948 *icm, ICM20948_acc_range acc_range) {
    icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_ACCEL_CONFIG);
    icm->reg_val &= ~(0x06);
    icm->reg_val |= (acc_range << 1);
    ICM20948_write_register8(icm, 2, ICM20948_ACCEL_CONFIG, icm->reg_val);
    icm->acc_range_factor = 1 << acc_range;
}

void ICM20948_set_acc_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf) {
    icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_ACCEL_CONFIG);
    if (dlpf == ICM20948_DLPF_OFF) {
        icm->reg_val &= 0xFE;
        ICM20948_write_register8(icm, 2, ICM20948_ACCEL_CONFIG, icm->reg_val);
        return;
    } else {
        icm->reg_val |= 0x01;
        icm->reg_val &= 0xC7;
        icm->reg_val |= (dlpf << 3);
    }
    ICM20948_write_register8(icm,2, ICM20948_ACCEL_CONFIG, icm->reg_val);
}

void ICM20948_set_acc_sample_rate_divider(ICM20948 *icm, uint16_t acc_spl_rate_div) {
    ICM20948_write_register16(icm, 2, ICM20948_ACCEL_SMPLRT_DIV_1, acc_spl_rate_div);
}

void ICM20948_enable_gyr(ICM20948 *icm, bool en_gyr) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_PWR_MGMT_2);
    if (en_gyr) {
        icm->reg_val &= ~ICM20948_GYR_EN;
    } else {
        icm->reg_val |= ICM20948_GYR_EN;
    }
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_2, icm->reg_val);
}

void ICM20948_set_gyr_range(ICM20948 *icm, ICM20948_gyro_range gyro_range) {
    icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_GYRO_CONFIG_1);
    icm->reg_val &= ~(0x06);
    icm->reg_val |= (gyro_range << 1);
    ICM20948_write_register8(icm, 2, ICM20948_GYRO_CONFIG_1, icm->reg_val);
    icm->gyr_range_factor = (1 << gyro_range);
}

void ICM20948_set_gyr_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf) {
    icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_GYRO_CONFIG_1);
    if (dlpf == ICM20948_DLPF_OFF) {
        icm->reg_val &= 0xFE;
        ICM20948_write_register8(icm, 2, ICM20948_GYRO_CONFIG_1, icm->reg_val);
        return;
    } else {
        icm->reg_val |= 0x01;
        icm->reg_val &= 0xC7;
        icm->reg_val |= (dlpf << 3);
    }
    ICM20948_write_register8(icm, 2, ICM20948_GYRO_CONFIG_1, icm->reg_val);
}


void ICM20948_set_gyr_sample_rate_divider(ICM20948 *icm, uint8_t gyr_spl_rate_div) {
    ICM20948_write_register8(icm, 2, ICM20948_GYRO_SMPLRT_DIV, gyr_spl_rate_div);
}

void ICM20948_set_temp_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf) {
    ICM20948_write_register8(icm, 2, ICM20948_TEMP_CONFIG, dlpf);
}

void ICM20948_set_i2c_mst_sample_rate(ICM20948 *icm, uint8_t rate_exp) {
    if (rate_exp < 16) {
        ICM20948_write_register8(icm, 3, ICM20948_I2C_MST_ODR_CFG, rate_exp);
    }
}

/************* x,y,z results *************/

void ICM20948_read_sensor(ICM20948 *icm) {
    ICM20948_read_all_data(icm, icm->buffer);
}

void ICM20948_get_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val) {
    acc_raw_val->x = (((icm->buffer[0]) << 8) | icm->buffer[1]) * 1.0;
    acc_raw_val->y = (((icm->buffer[2]) << 8) | icm->buffer[3]) * 1.0;
    acc_raw_val->z = (((icm->buffer[4]) << 8) | icm->buffer[5]) * 1.0;

//    acc_raw_val->x = ICM20948_read_register16(icm,0,ICM20948_ACCEL_X_OUT) * 1.0;
//    acc_raw_val->y = ICM20948_read_register16(icm,0,ICM20948_ACCEL_Y_OUT) * 1.0;
//    acc_raw_val->z = ICM20948_read_register16(icm,0,ICM20948_ACCEL_Z_OUT) * 1.0;
}

void ICM20948_get_corrected_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val) {
    ICM20948_get_acc_raw_values(icm, acc_raw_val);
    ICM20948_correct_acc_raw_values(icm, acc_raw_val);
}

//todo check that constant

void ICM20948_get_g_values(ICM20948 *icm, XYZ_float *g_val) {
    XYZ_float acc_raw_val;
    ICM20948_get_corrected_acc_raw_values(icm, &acc_raw_val);

    g_val->x = acc_raw_val.x * icm->acc_range_factor / 16384.0;
    g_val->y = acc_raw_val.y * icm->acc_range_factor / 16384.0;
    g_val->z = acc_raw_val.z * icm->acc_range_factor / 16384.0;
}

void ICM20948_get_acc_raw_values_from_fifo(ICM20948 *icm,XYZ_float *acc_raw_val) {
    ICM20948_read_XYZ_val_from_fifo(icm,acc_raw_val);
}

void ICM20948_get_corrected_acc_raw_values_from_fifo(ICM20948 *icm,XYZ_float *acc_raw_val) {
    ICM20948_get_acc_raw_values_from_fifo(icm, acc_raw_val);
    ICM20948_correct_acc_raw_values(icm, acc_raw_val);
}

//todo check this constant

void ICM20948_get_g_values_from_fifo(ICM20948 *icm,XYZ_float *g_val) {
    XYZ_float acc_raw_val;
    ICM20948_get_corrected_acc_raw_values_from_fifo(icm,&acc_raw_val);

    g_val->x = acc_raw_val.x * icm->acc_range_factor / 16384.0;
    g_val->y = acc_raw_val.y * icm->acc_range_factor / 16384.0;
    g_val->z = acc_raw_val.z * icm->acc_range_factor / 16384.0;
}

float ICM20948_get_resultant_g(XYZ_float *g_val) {
    float resultant = 0.0;
    resultant = sqrt(sq(g_val->x) + sq(g_val->y) + sq(g_val->z));
    return resultant;
}

float ICM20948_get_temperature(ICM20948 *icm) {
    int16_t raw_temp = ((icm->buffer[12]) << 8) | icm->buffer[13];
    float tmp = (raw_temp * 1.0 - ICM20948_ROOM_TEMP_OFFSET) / ICM20948_T_SENSITIVITY + 21.0;
    return tmp;
}


void ICM20948_get_gyr_raw_values(ICM20948 *icm, XYZ_float *gyr_raw_val) {
    gyr_raw_val->x = (int16_t)(((icm->buffer[6]) << 8) | icm->buffer[7]) * 1.0;
    gyr_raw_val->y = (int16_t)(((icm->buffer[8]) << 8) | icm->buffer[9]) * 1.0;
    gyr_raw_val->z = (int16_t)(((icm->buffer[10]) << 8) | icm->buffer[11]) * 1.0;
}


void ICM20948_get_corrected_gyr_raw_values(ICM20948 *icm,XYZ_float *gyr_raw_val) {
    ICM20948_get_gyr_raw_values(icm,gyr_raw_val);
    ICM20948_correct_gyr_raw_values(icm, gyr_raw_val);
}

void ICM20948_get_gyr_values(ICM20948 *icm,XYZ_float *gyr_val) {
    ICM20948_get_corrected_gyr_raw_values(icm,gyr_val);
    gyr_val->x = gyr_val->x * icm->gyr_range_factor * 250.0 / 32768.0;
    gyr_val->y = gyr_val->y * icm->gyr_range_factor * 250.0 / 32768.0;
    gyr_val->z = gyr_val->z * icm->gyr_range_factor * 250.0 / 32768.0;
}

void ICM20948_get_gyr_values_from_fifo(ICM20948 *icm,XYZ_float *gyr_val) {
    XYZ_float gyr_raw_val;
    ICM20948_read_XYZ_val_from_fifo(icm,&gyr_raw_val);
    ICM20948_correct_gyr_raw_values(icm, &gyr_raw_val);
    gyr_val->x = gyr_raw_val.x * icm->gyr_range_factor * 250.0 / 32768.0;
    gyr_val->y = gyr_raw_val.y * icm->gyr_range_factor * 250.0 / 32768.0;
    gyr_val->z = gyr_raw_val.z * icm->gyr_range_factor * 250.0 / 32768.0;
}


void ICM20948_get_mag_values(ICM20948 *icm,XYZ_float *mag) {
    int16_t x, y, z;

    x = (icm->buffer[15] << 8) | icm->buffer[14];
    y = (icm->buffer[17] << 8) | icm->buffer[16];
    z = (icm->buffer[19] << 8) | icm->buffer[18];

    mag->x = x * AK09916_MAG_LSB;
    mag->y = y * AK09916_MAG_LSB;
    mag->z = z * AK09916_MAG_LSB;

}


/********* Power, Sleep, Standby *********/

void ICM20948_enable_cycle(ICM20948 *icm, ICM20948_cycle cycle) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_LP_CONFIG);
    icm->reg_val &= 0x0F;
    icm->reg_val |= cycle;

    ICM20948_write_register8(icm, 0, ICM20948_LP_CONFIG, icm->reg_val);
}

void ICM20948_enable_low_power(ICM20948 *icm, bool en_lp) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_PWR_MGMT_1);
    if (en_lp) {
        icm->reg_val |= ICM20948_LP_EN;
    } else {
        icm->reg_val &= ~ICM20948_LP_EN;
    }
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_1, icm->reg_val);
}

void ICM20948_set_gyr_average_in_cycle_mode(ICM20948 *icm, ICM20948_gyro_avg_low_power avg) {
    ICM20948_write_register8(icm, 2, ICM20948_GYRO_CONFIG_2, avg);
}

void ICM20948_set_acc_average_in_cycle_mode(ICM20948 *icm, ICM20948_acc_avg_low_power avg) {
    ICM20948_write_register8(icm, 2, ICM20948_ACCEL_CONFIG_2, avg);
}

void ICM20948_sleep(ICM20948 *icm, bool sleep) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_PWR_MGMT_1);
    if (sleep) {
        icm->reg_val |= ICM20948_SLEEP;
    } else {
        icm->reg_val &= ~ICM20948_SLEEP;
    }
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_1, icm->reg_val);
}


/******** Angles and Orientation *********/


void ICM20948_get_angles(ICM20948 *icm, XYZ_float *angle_val) {

    XYZ_float g_val;
    ICM20948_get_g_values(icm,&g_val);

    if (g_val.x > 1.0) {
        g_val.x = 1.0;
    } else if (g_val.x < -1.0) {
        g_val.x = -1.0;
    }
    angle_val->x = (asin(g_val.x)) * 57.296;

    if (g_val.y > 1.0) {
        g_val.y = 1.0;
    } else if (g_val.y < -1.0) {
        g_val.y = -1.0;
    }
    angle_val->y = (asin(g_val.y)) * 57.296;

    if (g_val.z > 1.0) {
        g_val.z = 1.0;
    } else if (g_val.z < -1.0) {
        g_val.z = -1.0;
    }
    angle_val->z = (asin(g_val.z)) * 57.296;
}

ICM20948_orientation ICM20948_get_orientation(ICM20948 *icm) {
    XYZ_float angle_val;
    ICM20948_get_angles(icm,&angle_val);

    ICM20948_orientation orientation = ICM20948_FLAT;

    if (fabs(angle_val.x) < 45) {      // |x| < 45
        if (fabs(angle_val.y) < 45) {      // |y| < 45
            if (angle_val.z > 0) {          //  z  > 0
                orientation = ICM20948_FLAT;
            } else {                        //  z  < 0
                orientation = ICM20948_FLAT_1;
            }
        } else {                         // |y| > 45
            if (angle_val.y > 0) {         //  y  > 0
                orientation = ICM20948_XY;
            } else {                       //  y  < 0
                orientation = ICM20948_XY_1;
            }
        }
    } else {                           // |x| >= 45
        if (angle_val.x > 0) {           //  x  >  0
            orientation = ICM20948_YX;
        } else {                       //  x  <  0
            orientation = ICM20948_YX_1;
        }
    }
    return orientation;
}


char *ICM20948_get_orientation_as_string(ICM20948 *icm) {
    ICM20948_orientation orientation = ICM20948_get_orientation(icm);
    char *orientation_as_string = "";
    switch (orientation) {
        case ICM20948_FLAT:
            orientation_as_string = "z up";
            break;
        case ICM20948_FLAT_1:
            orientation_as_string = "z down";
            break;
        case ICM20948_XY:
            orientation_as_string = "y up";
            break;
        case ICM20948_XY_1:
            orientation_as_string = "y down";
            break;
        case ICM20948_YX:
            orientation_as_string = "x up";
            break;
        case ICM20948_YX_1:
            orientation_as_string = "x down";
            break;
    }
    return orientation_as_string;
}


float ICM20948_get_pitch(ICM20948 *icm) {
    XYZ_float angleVal;
    ICM20948_get_angles(icm,&angleVal);
    float pitch = (atan2(-angleVal.x, sqrt(fabs((angleVal.y * angleVal.y + angleVal.z * angleVal.z)))) * 180.0) / M_PI;
    return pitch;
}

float ICM20948_get_roll(ICM20948 *icm) {
    XYZ_float angleVal;
    ICM20948_get_angles(icm,&angleVal);
    float roll = (atan2(angleVal.y, angleVal.z) * 180.0) / M_PI;
    return roll;
}


/************** Interrupts ***************/

void ICM20948_set_int_pin_polarity(ICM20948 *icm, ICM20948_int_pin_pol pol) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
    if (pol) {
        icm->reg_val |= ICM20948_INT1_ACTL;
    } else {
        icm->reg_val &= ~ICM20948_INT1_ACTL;
    }
    ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val);
}

void ICM20948_enable_int_latch(ICM20948 *icm, bool latch) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
    if (latch) {
        icm->reg_val |= ICM20948_INT_1_LATCH_EN;
    } else {
        icm->reg_val &= ~ICM20948_INT_1_LATCH_EN;
    }
    ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val);
}

void ICM20948_enable_clear_int_by_any_read(ICM20948 *icm, bool clear_by_any_read) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
    if (clear_by_any_read) {
        icm->reg_val |= ICM20948_INT_ANYRD_2CLEAR;
    } else {
        icm->reg_val &= ~ICM20948_INT_ANYRD_2CLEAR;
    }
    ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val);
}

void ICM20948_set_fsync_int_polarity(ICM20948 *icm, ICM20948_int_pin_pol pol) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
    if (pol) {
        icm->reg_val |= ICM20948_ACTL_FSYNC;
    } else {
        icm->reg_val &= ~ICM20948_ACTL_FSYNC;
    }
    ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val);
}

void ICM20948_enable_interrupt(ICM20948 *icm, ICM20948_int_type type) {
    switch (type) {
        case ICM20948_FSYNC_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
            icm->reg_val |= ICM20948_FSYNC_INT_MODE_EN;
            ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val); // enable FSYNC as interrupt pin
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val |= 0x80;
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val); // enable wake on FSYNC interrupt
            break;

        case ICM20948_WOM_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val |= 0x08;
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val);
            icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL);
            icm->reg_val |= 0x02;
            ICM20948_write_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL, icm->reg_val);
            break;

        case ICM20948_DMP_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val |= 0x02;
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val);
            break;

        case ICM20948_DATA_READY_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_1, 0x01);
            break;

        case ICM20948_FIFO_OVF_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_2, 0x01);
            break;

        case ICM20948_FIFO_WM_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_3, 0x01);
            break;
    }
}


void ICM20948_disable_interrupt(ICM20948 *icm, ICM20948_int_type type) {
    switch (type) {
        case ICM20948_FSYNC_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_PIN_CFG);
            icm->reg_val &= ~ICM20948_FSYNC_INT_MODE_EN;
            ICM20948_write_register8(icm, 0, ICM20948_INT_PIN_CFG, icm->reg_val);
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val &= ~(0x80);
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val);
            break;

        case ICM20948_WOM_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val &= ~(0x08);
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val);
            icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL);
            icm->reg_val &= ~(0x02);
            ICM20948_write_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL, icm->reg_val);
            break;

        case ICM20948_DMP_INT:
            icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_ENABLE);
            icm->reg_val &= ~(0x02);
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE, icm->reg_val);
            break;

        case ICM20948_DATA_READY_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_1, 0x00);
            break;

        case ICM20948_FIFO_OVF_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_2, 0x00);
            break;

        case ICM20948_FIFO_WM_INT:
            ICM20948_write_register8(icm, 0, ICM20948_INT_ENABLE_3, 0x00);
            break;
    }
}


uint8_t ICM20948_read_and_clear_interrupts(ICM20948 *icm) {
    uint8_t int_source = 0;
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_I2C_MST_STATUS);
    if (icm->reg_val & 0x80) {
        int_source |= 0x01;
    }
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_STATUS);
    if (icm->reg_val & 0x08) {
        int_source |= 0x02;
    }
    if (icm->reg_val & 0x02) {
        int_source |= 0x04;
    }
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_STATUS_1);
    if (icm->reg_val & 0x01) {
        int_source |= 0x08;
    }
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_STATUS_2);
    if (icm->reg_val & 0x01) {
        int_source |= 0x10;
    }
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_INT_STATUS_3);
    if (icm->reg_val & 0x01) {
        int_source |= 0x20;
    }
    return int_source;
}

bool ICM20948_check_interrupt(uint8_t source, ICM20948_int_type type) {
    source &= type;
    return source;
}

void ICM20948_set_wake_on_motion_threshold(ICM20948 *icm, uint8_t wom_thresh, ICM20948_wom_comp_en wom_comp_en) {
    icm->reg_val = ICM20948_read_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL);
    if (wom_comp_en) {
        icm->reg_val |= 0x01;
    } else {
        icm->reg_val &= ~(0x01);
    }
    ICM20948_write_register8(icm, 2, ICM20948_ACCEL_INTEL_CTRL, icm->reg_val);
    ICM20948_write_register8(icm, 2, ICM20948_ACCEL_WOM_THR, wom_thresh);
}


/***************** FIFO ******************/

void ICM20948_enable_fifo(ICM20948 *icm, bool fifo) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_USER_CTRL);
    if (fifo) {
        icm->reg_val |= ICM20948_FIFO_EN;
    } else {
        icm->reg_val &= ~ICM20948_FIFO_EN;
    }
    ICM20948_write_register8(icm, 0, ICM20948_USER_CTRL, icm->reg_val);
}

void ICM20948_set_fifo_mode(ICM20948 *icm, ICM20948_fifo_mode mode) {
    if (mode) {
        icm->reg_val = 0x01;
    } else {
        icm->reg_val = 0x00;
    }
    ICM20948_write_register8(icm, 0, ICM20948_FIFO_MODE, icm->reg_val);
}

void ICM20948_start_fifo(ICM20948 *icm, ICM20948_fifo_type fifo) {
    icm->fifo_type = fifo;
    ICM20948_write_register8(icm, 0, ICM20948_FIFO_EN_2, icm->fifo_type);
}

void ICM20948_stop_fifo(ICM20948 *icm) {
    ICM20948_write_register8(icm, 0, ICM20948_FIFO_EN_2, 0);
}

void ICM20948_reset_fifo(ICM20948 *icm) {
    ICM20948_write_register8(icm, 0, ICM20948_FIFO_RST, 0x01);
    ICM20948_write_register8(icm, 0, ICM20948_FIFO_RST, 0x00);
}

int16_t ICM20948_get_fifo_count(ICM20948 *icm) {
    int16_t reg_val_16 = ICM20948_read_register16(icm, 0, ICM20948_FIFO_COUNT);
    return reg_val_16;
}


int16_t ICM20948_get_number_of_fifo_data_sets(ICM20948 *icm) {
    int16_t number_of_sets = ICM20948_get_fifo_count(icm);

    if ((icm->fifo_type == ICM20948_FIFO_ACC) || (icm->fifo_type == ICM20948_FIFO_GYR)) {
        number_of_sets /= 6;
    } else if (icm->fifo_type == ICM20948_FIFO_ACC_GYR) {
        number_of_sets /= 12;
    }

    return number_of_sets;
}

void ICM20948_find_fifo_begin(ICM20948 *icm) {
    uint16_t count = ICM20948_get_fifo_count(icm);
    int16_t start = 0;

    if ((icm->fifo_type == ICM20948_FIFO_ACC) || (icm->fifo_type == ICM20948_FIFO_GYR)) {
        start = count % 6;
        for (int i = 0; i < start; i++) {
            ICM20948_read_register8(icm, 0, ICM20948_FIFO_R_W);
        }
    } else if (icm->fifo_type == ICM20948_FIFO_ACC_GYR) {
        start = count % 12;
        for (int i = 0; i < start; i++) {
            ICM20948_read_register8(icm, 0, ICM20948_FIFO_R_W);
        }
    }
}


/************** Magnetometer **************/

bool ICM20948_init_magnetometer(ICM20948 *icm) {
    ICM20948_enable_i2c_master(icm);
    ICM20948_reset_mag(icm);
    ICM20948_reset(icm);
    ICM20948_sleep(icm, false);
    ICM20948_write_register8(icm, 2, ICM20948_ODR_ALIGN_EN, 1); // aligns ODR
    vTaskDelay(10);
    ICM20948_enable_i2c_master(icm);
    vTaskDelay(10);

    int16_t who_am_i = ICM20948_who_am_i_mag(icm);
    if (!((who_am_i == AK09916_WHO_AM_I_1) || (who_am_i == AK09916_WHO_AM_I_2))) {
        return false;
    }
    ICM20948_set_mag_op_mode(icm, AK09916_CONT_MODE_100HZ);

    return true;
}

uint16_t ICM20948_who_am_i_mag(ICM20948 *icm) {
    return ICM20948_read_AK09916_register16(icm, AK09916_WIA_1);
}

void ICM20948_set_mag_op_mode(ICM20948 *icm, AK09916_op_mode op_mode) {
    ICM20948_write_AK09916_register8(icm, AK09916_CNTL_2, op_mode);
    vTaskDelay(10);

    if (op_mode != AK09916_PWR_DOWN) {
        ICM20948_enable_mag_data_read(icm, AK09916_HXL, 0x08);
    }
}

void ICM20948_reset_mag(ICM20948 *icm) {
    ICM20948_write_AK09916_register8(icm, AK09916_CNTL_3, 0x01);
    vTaskDelay(100);
}


/************************************************
     Private Functions
*************************************************/

void ICM20948_set_clock_to_auto_select(ICM20948 *icm) {
    icm->reg_val = ICM20948_read_register8(icm, 0, ICM20948_PWR_MGMT_1);
    icm->reg_val |= 0x01;
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_1, icm->reg_val);
    vTaskDelay(10);
}

void ICM20948_correct_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val) {
    acc_raw_val->x = (acc_raw_val->x - (icm->acc_offset_val.x / icm->acc_range_factor)) / icm->acc_corr_factor.x;
    acc_raw_val->y = (acc_raw_val->y - (icm->acc_offset_val.y / icm->acc_range_factor)) / icm->acc_corr_factor.y;
    acc_raw_val->z = (acc_raw_val->z - (icm->acc_offset_val.z / icm->acc_range_factor)) / icm->acc_corr_factor.z;
}

void ICM20948_correct_gyr_raw_values(ICM20948 *icm, XYZ_float *gyr_raw_val) {
    gyr_raw_val->x -= (icm->gyr_offset_val.x / icm->gyr_range_factor);
    gyr_raw_val->y -= (icm->gyr_offset_val.y / icm->gyr_range_factor);
    gyr_raw_val->z -= (icm->gyr_offset_val.z / icm->gyr_range_factor);
}

void ICM20948_switch_bank(ICM20948 *icm, uint8_t bank) {
    if (icm->current_bank != bank) {
        icm->current_bank = bank;
        uint8_t bank_array[1] = {(icm->current_bank << 4)};
        I2C_Transfer_Cfg cfg;

        cfg.slaveAddr10Bit = false;
        cfg.stopEveryByte = false;
        cfg.slaveAddr = icm->i2c_address;


        cfg.subAddr = ICM20948_REG_BANK_SEL;
        cfg.subAddrSize = 1;
        cfg.dataSize = 1;
        cfg.data = bank_array;

        I2C_MasterSendBlocking(I2C0_MM_ID, &cfg);
    }
}

void ICM20948_write_register8(ICM20948 *icm, uint8_t bank, uint8_t reg, uint8_t val) {
    ICM20948_switch_bank(icm, bank);

    I2C_Transfer_Cfg cfg;

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = reg;
    cfg.subAddrSize = 1;
    cfg.dataSize = 1;
    cfg.data = &val;

    BL_Err_Type res = I2C_MasterSendBlocking(I2C0_MM_ID, &cfg);
    printf("I2C address %d response: %d\n",reg, res);

}


void ICM20948_write_register16(ICM20948 *icm, uint8_t bank, uint8_t reg, int16_t val) {
    ICM20948_switch_bank(icm, bank);

    I2C_Transfer_Cfg cfg;
    uint8_t value[2] = {((val >> 8) & 0xFF), val & 0xFF};

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = reg;
    cfg.subAddrSize = 1;
    cfg.dataSize = 2;
    cfg.data = value;

    I2C_MasterSendBlocking(I2C0_MM_ID, &cfg);

}

uint8_t ICM20948_read_register8(ICM20948 *icm, uint8_t bank, uint8_t reg) {
    ICM20948_switch_bank(icm, bank);

    I2C_Transfer_Cfg cfg;
    uint8_t value;

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = reg;
    cfg.subAddrSize = 1;
    cfg.dataSize = 1;
    cfg.data = &value;
    BL_Err_Type res = I2C_MasterReceiveBlocking(I2C0_MM_ID, &cfg);
    printf("I2C address %d response: %d\n",reg, res);

    return value;
}

int16_t ICM20948_read_register16(ICM20948 *icm, uint8_t bank, uint8_t reg) {
    ICM20948_switch_bank(icm, bank);

    I2C_Transfer_Cfg cfg;
    uint8_t value[2];

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = reg;
    cfg.subAddrSize = 1;
    cfg.dataSize = 2;
    cfg.data = value;
    I2C_MasterReceiveBlocking(I2C0_MM_ID, &cfg);

    return (value[0] << 8) + value[1];
}

void ICM20948_read_all_data(ICM20948 *icm, uint8_t *data) {
    ICM20948_switch_bank(icm, 0);

    I2C_Transfer_Cfg cfg;

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = ICM20948_ACCEL_X_OUT;
    cfg.subAddrSize = 1;
    cfg.dataSize = 20;
    cfg.data = data;
    I2C_MasterReceiveBlocking(I2C0_MM_ID, &cfg);
}

void ICM20948_read_XYZ_val_from_fifo(ICM20948 *icm,XYZ_float* xyz_result) {
    ICM20948_switch_bank(icm, 0);
    uint8_t fifo_triple[6];

    xyz_result->x = 0.0;
    xyz_result->y = 0.0;
    xyz_result->z = 0.0;

    I2C_Transfer_Cfg cfg;

    cfg.slaveAddr10Bit = false;
    cfg.stopEveryByte = false;
    cfg.slaveAddr = icm->i2c_address;


    cfg.subAddr = ICM20948_FIFO_R_W;
    cfg.subAddrSize = 1;
    cfg.dataSize = 6;
    cfg.data = fifo_triple;
    I2C_MasterReceiveBlocking(I2C0_MM_ID, &cfg);

    xyz_result->x = ((fifo_triple[0] << 8) + fifo_triple[1]) * 1.0;
    xyz_result->y = ((fifo_triple[2] << 8) + fifo_triple[3]) * 1.0;
    xyz_result->z = ((fifo_triple[4] << 8) + fifo_triple[5]) * 1.0;
}

void ICM20948_write_AK09916_register8(ICM20948 *icm, uint8_t reg, uint8_t val) {
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS); // write AK09916
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be written to
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_DO, val);
}

uint8_t ICM20948_read_AK09916_register8(ICM20948 *icm, uint8_t reg) {
    ICM20948_enable_mag_data_read(icm, reg, 0x01);
    ICM20948_enable_mag_data_read(icm, AK09916_HXL, 0x08);
    uint8_t reg_val = ICM20948_read_register8(icm, 0, ICM20948_EXT_SLV_SENS_DATA_00);
    return reg_val;
}

int16_t ICM20948_read_AK09916_register16(ICM20948 *icm, uint8_t reg) {
    int16_t reg_value = 0;
    ICM20948_enable_mag_data_read(icm, reg, 0x02);
    reg_value = ICM20948_read_register16(icm, 0, ICM20948_EXT_SLV_SENS_DATA_00);
    ICM20948_enable_mag_data_read(icm, AK09916_HXL, 0x08);
    return reg_value;
}

void ICM20948_reset(ICM20948 *icm) {
    ICM20948_write_register8(icm, 0, ICM20948_PWR_MGMT_1, ICM20948_RESET);
    vTaskDelay(10);  // wait for registers to reset
}

void ICM20948_enable_i2c_master(ICM20948 *icm) {
    ICM20948_write_register8(icm, 0, ICM20948_USER_CTRL, ICM20948_I2C_MST_EN); //enable I2C master
    ICM20948_write_register8(icm, 3, ICM20948_I2C_MST_CTRL, 0x07); // set I2C clock to 345.60 kHz
    vTaskDelay(10);
}

void ICM20948_enable_mag_data_read(ICM20948 *icm, uint8_t reg, uint8_t bytes) {
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS | AK09916_READ); // read AK09916
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be read
    ICM20948_write_register8(icm, 3, ICM20948_I2C_SLV0_CTRL, 0x80 | bytes); //enable read | number of byte
    vTaskDelay(10);
}

float sq(float x){
    return x*x;
}

void ICM20948_accel_calibration(ICM20948 *icm)
{
    uint8_t samples = 100;
    XYZ_float temp;
    uint8_t temp2[2];
    uint8_t temp3[2];
    uint8_t temp4[2];

    int32_t accel_bias[3] = {0};
    int32_t accel_bias_reg[3] = {0};
    uint8_t accel_offset[6] = {0};

    for(int i = 0; i < samples; i++)
    {
        ICM20948_get_acc_raw_values(icm,&temp);
        accel_bias[0] += temp.x;
        accel_bias[1] += temp.y;
        accel_bias[2] += temp.z;
    }

    accel_bias[0] /= samples;
    accel_bias[1] /= samples;
    accel_bias[2] /= samples;

    uint8_t mask_bit[3] = {0, 0, 0};

    temp2[0] = ICM20948_read_register8(icm,1, ICM20948_XA_OFFS_H);
    temp2[1] = ICM20948_read_register8(icm,1, ICM20948_XA_OFFS_L);
    accel_bias_reg[0] = (int32_t)(temp2[0] << 8 | temp2[1]);
    mask_bit[0] = temp2[1] & 0x01;

    temp3[0] = ICM20948_read_register8(icm,1, ICM20948_YA_OFFS_H);
    temp3[1] = ICM20948_read_register8(icm,1, ICM20948_YA_OFFS_L);
    accel_bias_reg[1] = (int32_t)(temp3[0] << 8 | temp3[1]);
    mask_bit[1] = temp3[1] & 0x01;

    temp4[0] = ICM20948_read_register8(icm,1, ICM20948_ZA_OFFS_H);
    temp4[1] = ICM20948_read_register8(icm,1, ICM20948_ZA_OFFS_L);
    accel_bias_reg[2] = (int32_t)(temp4[0] << 8 | temp4[1]);
    mask_bit[2] = temp4[1] & 0x01;

    accel_bias_reg[0] -= (accel_bias[0] / 8);
    accel_bias_reg[1] -= (accel_bias[1] / 8);
    accel_bias_reg[2] -= (accel_bias[2] / 8);

    accel_offset[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    accel_offset[1] = (accel_bias_reg[0])      & 0xFE;
    accel_offset[1] = accel_offset[1] | mask_bit[0];

    accel_offset[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    accel_offset[3] = (accel_bias_reg[1])      & 0xFE;
    accel_offset[3] = accel_offset[3] | mask_bit[1];

    accel_offset[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    accel_offset[5] = (accel_bias_reg[2])      & 0xFE;
    accel_offset[5] = accel_offset[5] | mask_bit[2];

    ICM20948_write_register16(icm,1, ICM20948_XA_OFFS_H, &accel_offset[0]);
    ICM20948_write_register16(icm,1, ICM20948_YA_OFFS_H, &accel_offset[2]);
    ICM20948_write_register16(icm,1, ICM20948_ZA_OFFS_H, &accel_offset[4]);
}