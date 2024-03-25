#ifndef ICM20948_1_H_
#define ICM20948_1_H_

#include "xyzfloat.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <bl808_i2c.h>

/* Registers ICM20948 USER BANK 0*/
#define ICM20948_WHO_AM_I               (0x00)
#define ICM20948_USER_CTRL              (0x03)
#define ICM20948_LP_CONFIG              (0x05)
#define ICM20948_PWR_MGMT_1             (0x06)
#define ICM20948_PWR_MGMT_2             (0x07)
#define ICM20948_INT_PIN_CFG            (0x0F)
#define ICM20948_INT_ENABLE             (0x10)
#define ICM20948_INT_ENABLE_1           (0x11)
#define ICM20948_INT_ENABLE_2           (0x12)
#define ICM20948_INT_ENABLE_3           (0x13)
#define ICM20948_I2C_MST_STATUS         (0x17)
#define ICM20948_INT_STATUS             (0x19)
#define ICM20948_INT_STATUS_1           (0x1A)
#define ICM20948_INT_STATUS_2           (0x1B)
#define ICM20948_INT_STATUS_3           (0x1C)
#define ICM20948_DELAY_TIME_H           (0x28)
#define ICM20948_DELAY_TIME_L           (0x29)
#define ICM20948_ACCEL_X_OUT              (0x2D) // accel data registers begin
#define ICM20948_ACCEL_Y_OUT              (0x2F) // accel data registers begin
#define ICM20948_ACCEL_Z_OUT              (0x31) // accel data registers begin
#define ICM20948_GYRO_OUT               (0x33) // gyro data registers begin
#define ICM20948_TEMP_OUT               (0x39)
#define ICM20948_EXT_SLV_SENS_DATA_00   (0x3B)
#define ICM20948_EXT_SLV_SENS_DATA_01   (0x3C)
#define ICM20948_FIFO_EN_1              (0x66)
#define ICM20948_FIFO_EN_2              (0x67)
#define ICM20948_FIFO_RST               (0x68)
#define ICM20948_FIFO_MODE              (0x69)
#define ICM20948_FIFO_COUNT             (0x70)
#define ICM20948_FIFO_R_W               (0x72)
#define ICM20948_DATA_RDY_STATUS        (0x74)
#define ICM20948_FIFO_CFG               (0x76)

/* Registers ICM20948 USER BANK 1*/
#define ICM20948_SELF_TEST_X_GYRO       (0x02)
#define ICM20948_SELF_TEST_Y_GYRO       (0x03)
#define ICM20948_SELF_TEST_Z_GYRO       (0x04)
#define ICM20948_SELF_TEST_X_ACCEL      (0x0E)
#define ICM20948_SELF_TEST_Y_ACCEL      (0x0F)
#define ICM20948_SELF_TEST_Z_ACCEL      (0x10)
#define ICM20948_XA_OFFS_H              (0x14)
#define ICM20948_XA_OFFS_L              (0x15)
#define ICM20948_YA_OFFS_H              (0x17)
#define ICM20948_YA_OFFS_L              (0x18)
#define ICM20948_ZA_OFFS_H              (0x1A)
#define ICM20948_ZA_OFFS_L              (0x1B)
#define ICM20948_TIMEBASE_CORR_PLL      (0x28)

/* Registers ICM20948 USER BANK 2*/
#define ICM20948_GYRO_SMPLRT_DIV        (0x00)
#define ICM20948_GYRO_CONFIG_1          (0x01)
#define ICM20948_GYRO_CONFIG_2          (0x02)
#define ICM20948_XG_OFFS_USRH           (0x03)
#define ICM20948_XG_OFFS_USRL           (0x04)
#define ICM20948_YG_OFFS_USRH           (0x05)
#define ICM20948_YG_OFFS_USRL           (0x06)
#define ICM20948_ZG_OFFS_USRH           (0x07)
#define ICM20948_ZG_OFFS_USRL           (0x08)
#define ICM20948_ODR_ALIGN_EN           (0x09)
#define ICM20948_ACCEL_SMPLRT_DIV_1     (0x10)
#define ICM20948_ACCEL_SMPLRT_DIV_2     (0x11)
#define ICM20948_ACCEL_INTEL_CTRL       (0x12)
#define ICM20948_ACCEL_WOM_THR          (0x13)
#define ICM20948_ACCEL_CONFIG           (0x14)
#define ICM20948_ACCEL_CONFIG_2         (0x15)
#define ICM20948_FSYNC_CONFIG           (0x52)
#define ICM20948_TEMP_CONFIG            (0x53)
#define ICM20948_MOD_CTRL_USR           (0x54)

/* Registers ICM20948 USER BANK 3*/
#define ICM20948_I2C_MST_ODR_CFG        (0x00)
#define ICM20948_I2C_MST_CTRL           (0x01)
#define ICM20948_I2C_MST_DELAY_CTRL     (0x02)
#define ICM20948_I2C_SLV0_ADDR          (0x03)
#define ICM20948_I2C_SLV0_REG           (0x04)
#define ICM20948_I2C_SLV0_CTRL          (0x05)
#define ICM20948_I2C_SLV0_DO            (0x06)

/* Registers ICM20948 ALL BANKS */
#define ICM20948_REG_BANK_SEL           (0x7F)

/* Registers AK09916 */
#define AK09916_ADDRESS                 (0x0C)
#define AK09916_WIA_1                   (0x00) // Who I am, Company ID
#define AK09916_WIA_2                   (0x01) // Who I am, Device ID
#define AK09916_STATUS_1                (0x10)
#define AK09916_HXL                     (0x11)
#define AK09916_HXH                     (0x12)
#define AK09916_HYL                     (0x13)
#define AK09916_HYH                     (0x14)
#define AK09916_HZL                     (0x15)
#define AK09916_HZH                     (0x16)
#define AK09916_STATUS_2                (0x18)
#define AK09916_CNTL_2                  (0x31)
#define AK09916_CNTL_3                  (0x32)

/* Register Bits */
#define ICM20948_RESET                  (0x80)
#define ICM20948_I2C_MST_EN             (0x20)
#define ICM20948_I2C_ADDRESS            (0x68)
#define ICM20948_SLEEP                  (0x40)
#define ICM20948_LP_EN                  (0x20)
#define ICM20948_BYPASS_EN              (0x02)
#define ICM20948_GYR_EN                 (0x07)
#define ICM20948_ACC_EN                 (0x38)
#define ICM20948_FIFO_EN                (0x40)
#define ICM20948_INT1_ACTL              (0x80)
#define ICM20948_INT_1_LATCH_EN         (0x20)
#define ICM20948_ACTL_FSYNC             (0x08)
#define ICM20948_INT_ANYRD_2CLEAR       (0x10)
#define ICM20948_FSYNC_INT_MODE_EN      (0x06)
#define AK09916_16_BIT                  (0x10)
#define AK09916_OVF                     (0x08)
#define AK09916_READ                    (0x80)
/* Others */
#define AK09916_WHO_AM_I_1              (0x4809)
#define AK09916_WHO_AM_I_2              (0x0948)
#define ICM20948_WHO_AM_I_CONTENT       (0xEA)
#define ICM20948_ROOM_TEMP_OFFSET       (0.0)
#define ICM20948_T_SENSITIVITY          (333.87)
#define AK09916_MAG_LSB                 (0.1495)

/* Enums */

typedef enum ICM20948_CYCLE {
    ICM20948_NO_CYCLE = 0x00,
    ICM20948_GYR_CYCLE = 0x10,
    ICM20948_ACC_CYCLE = 0x20,
    ICM20948_ACC_GYR_CYCLE = 0x30,
    ICM20948_ACC_GYR_I2C_MST_CYCLE = 0x70
} ICM20948_cycle;

typedef enum ICM20948_INT_PIN_POL {
    ICM20948_ACT_HIGH, ICM20948_ACT_LOW
} ICM20948_int_pin_pol;

typedef enum ICM20948_INT_TYPE {
    ICM20948_FSYNC_INT = 0x01,
    ICM20948_WOM_INT = 0x02,
    ICM20948_DMP_INT = 0x04,
    ICM20948_DATA_READY_INT = 0x08,
    ICM20948_FIFO_OVF_INT = 0x10,
    ICM20948_FIFO_WM_INT = 0x20
} ICM20948_int_type;

typedef enum ICM20948_FIFO_TYPE {
    ICM20948_FIFO_ACC = 0x10,
    ICM20948_FIFO_GYR = 0x0E,
    ICM20948_FIFO_ACC_GYR = 0x1E
} ICM20948_fifo_type;

typedef enum ICM20948_FIFO_MODE_CHOICE {
    ICM20948_CONTINUOUS, ICM20948_STOP_WHEN_FULL
} ICM20948_fifo_mode;

typedef enum ICM20948_GYRO_RANGE {
    ICM20948_GYRO_RANGE_250, ICM20948_GYRO_RANGE_500, ICM20948_GYRO_RANGE_1000, ICM20948_GYRO_RANGE_2000
} ICM20948_gyro_range;

typedef enum ICM20948_DLPF {
    ICM20948_DLPF_0 = 0, ICM20948_DLPF_1, ICM20948_DLPF_2, ICM20948_DLPF_3, ICM20948_DLPF_4, ICM20948_DLPF_5,
    ICM20948_DLPF_6, ICM20948_DLPF_7, ICM20948_DLPF_OFF
} ICM20948_dlpf;

typedef enum ICM20948_GYRO_AVG_LOW_PWR {
    ICM20948_GYR_AVG_1, ICM20948_GYR_AVG_2, ICM20948_GYR_AVG_4, ICM20948_GYR_AVG_8, ICM20948_GYR_AVG_16,
    ICM20948_GYR_AVG_32, ICM20948_GYR_AVG_64, ICM20948_GYR_AVG_128
} ICM20948_gyro_avg_low_power;

typedef enum ICM20948_ACC_RANGE {
    ICM20948_ACC_RANGE_2G, ICM20948_ACC_RANGE_4G, ICM20948_ACC_RANGE_8G, ICM20948_ACC_RANGE_16G
} ICM20948_acc_range;

typedef enum ICM20948_ACC_AVG_LOW_PWR {
    ICM20948_ACC_AVG_4, ICM20948_ACC_AVG_8, ICM20948_ACC_AVG_16, ICM20948_ACC_AVG_32
} ICM20948_acc_avg_low_power;

typedef enum ICM20948_WOM_COMP {
    ICM20948_WOM_COMP_DISABLE, ICM20948_WOM_COMP_ENABLE
} ICM20948_wom_comp_en;

typedef enum AK09916_OP_MODE {
    AK09916_PWR_DOWN = 0x00,
    AK09916_TRIGGER_MODE = 0x01,
    AK09916_CONT_MODE_10HZ = 0x02,
    AK09916_CONT_MODE_20HZ = 0x04,
    AK09916_CONT_MODE_50HZ = 0x06,
    AK09916_CONT_MODE_100HZ = 0x08
} AK09916_op_mode;

typedef enum ICM20948_ORIENTATION {
    ICM20948_FLAT, ICM20948_FLAT_1, ICM20948_XY, ICM20948_XY_1, ICM20948_YX, ICM20948_YX_1
} ICM20948_orientation;

typedef struct ICM20948 {
    uint8_t i2c_address;
    uint8_t current_bank;
    uint8_t buffer[20];
    XYZ_float acc_offset_val;
    XYZ_float acc_corr_factor;
    XYZ_float gyr_offset_val;
    uint8_t acc_range_factor;
    uint8_t gyr_range_factor;
    uint8_t reg_val;   // intermediate storage of register values
    ICM20948_fifo_type fifo_type;
} ICM20948;

/* Basic settings */

bool ICM20948_init(ICM20948 *icm);

void ICM20948_auto_offsets(ICM20948 *icm);

void ICM20948_set_acc_offsets(ICM20948 *icm, float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);

void ICM20948_set_gyr_offsets(ICM20948 *icm, float x_offset, float y_offset, float z_offset);

uint8_t ICM20948_who_am_i(ICM20948 *icm);

void ICM20948_enable_acc(ICM20948 *icm, bool is_enabled);

void ICM20948_set_acc_range(ICM20948 *icm, ICM20948_acc_range range);

void ICM20948_set_acc_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf);

void ICM20948_set_acc_sample_rate_divider(ICM20948 *icm, uint16_t divider);

void ICM20948_enable_gyr(ICM20948 *icm, bool enGyr);

void ICM20948_set_gyr_range(ICM20948 *icm, ICM20948_gyro_range range);

void ICM20948_set_gyr_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf);

void ICM20948_set_gyr_sample_rate_divider(ICM20948 *icm, uint8_t divider);

void ICM20948_set_temp_dlpf(ICM20948 *icm, ICM20948_dlpf dlpf);

void ICM20948_set_i2c_mst_sample_rate(ICM20948 *icm, uint8_t rateExp);


/* x,y,z results */

void ICM20948_read_sensor(ICM20948 *icm);

void ICM20948_get_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val);

void ICM20948_get_corrected_acc_raw_values(ICM20948 *icm, XYZ_float *acc_raw_val);

void ICM20948_get_g_values(ICM20948 *icm,XYZ_float *g_values);

void ICM20948_get_acc_raw_values_from_fifo(ICM20948 *icm,XYZ_float* values);

void ICM20948_get_corrected_acc_raw_values_from_fifo(ICM20948 *icm,XYZ_float *acc_raw_val) ;

void ICM20948_get_g_values_from_fifo(ICM20948 *icm,XYZ_float* values);

float ICM20948_get_resultant_g(XYZ_float *val);

float ICM20948_get_temperature(ICM20948 *icm);

void ICM20948_get_gyr_raw_values(ICM20948 *icm, XYZ_float* values);

void ICM20948_get_corrected_gyr_raw_values(ICM20948 *icm, XYZ_float* values);

void ICM20948_get_gyr_values(ICM20948 *icm, XYZ_float* values);

void ICM20948_get_gyr_values_from_fifo(ICM20948 *icm, XYZ_float* values);

void ICM20948_get_mag_values(ICM20948 *icm, XYZ_float* values);


/* Angles and Orientation */

void ICM20948_get_angles(ICM20948 *icm, XYZ_float* values);

ICM20948_orientation ICM20948_get_orientation(ICM20948 *icm);

char* ICM20948_get_orientation_as_string(ICM20948 *icm);

float ICM20948_get_pitch(ICM20948 *icm);

float ICM20948_get_roll(ICM20948 *icm);


/* Power, Sleep, Standby */

void ICM20948_enable_cycle(ICM20948 *icm, ICM20948_cycle cycle);

void ICM20948_enable_low_power(ICM20948 *icm, bool enLP);

void ICM20948_set_gyr_average_in_cycle_mode(ICM20948 *icm, ICM20948_gyro_avg_low_power avg);

void ICM20948_set_acc_average_in_cycle_mode(ICM20948 *icm, ICM20948_acc_avg_low_power avg);

void ICM20948_sleep(ICM20948 *icm, bool sleep);


/* Interrupts */

void ICM20948_set_int_pin_polarity(ICM20948 *icm, ICM20948_int_pin_pol pol);

void ICM20948_enable_int_latch(ICM20948 *icm, bool latch);

void ICM20948_enable_clear_int_by_any_read(ICM20948 *icm, bool clear_any_band);

void ICM20948_set_fsync_int_polarity(ICM20948 *icm, ICM20948_int_pin_pol pol);

void ICM20948_enable_interrupt(ICM20948 *icm, ICM20948_int_type type);

void ICM20948_disable_interrupt(ICM20948 *icm, ICM20948_int_type type);

uint8_t ICM20948_read_and_clear_interrupts(ICM20948 *icm);

bool ICM20948_check_interrupt(uint8_t source, ICM20948_int_type type);

void ICM20948_set_wake_on_motion_threshold(ICM20948 *icm, uint8_t womThresh, ICM20948_wom_comp_en wom_comp_en);


/* FIFO */

void ICM20948_enable_fifo(ICM20948 *icm, bool fifo);

void ICM20948_set_fifo_mode(ICM20948 *icm, ICM20948_fifo_mode mode);

void ICM20948_start_fifo(ICM20948 *icm, ICM20948_fifo_type fifo);

void ICM20948_stop_fifo(ICM20948 *icm);

void ICM20948_reset_fifo(ICM20948 *icm);

int16_t ICM20948_get_fifo_count(ICM20948 *icm);

int16_t ICM20948_get_number_of_fifo_data_sets(ICM20948 *icm);

void ICM20948_find_fifo_begin(ICM20948 *icm);


/* Magnetometer */

bool ICM20948_init_magnetometer(ICM20948 *icm);

uint16_t ICM20948_who_am_i_mag(ICM20948 *icm);

void ICM20948_set_mag_op_mode(ICM20948 *icm, AK09916_op_mode op_mode);

void ICM20948_reset_mag(ICM20948 *icm);

void ICM20948_accel_calibration(ICM20948 *icm);

#endif
