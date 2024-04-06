//
// Created by Arthur Nindaba on 23/03/2024.
//

#ifndef PEN_MOTION_ICM_PROCESSOR_H
#define PEN_MOTION_ICM_PROCESSOR_H

#define CSV_LINE_LENGTH             117
#define DEFAULT_NUM_OF_READINGS      500

void read_all_sensor_values(char *values);

void register_icm_cmds(void);

void icm_init(void);

#endif //PEN_MOTION_ICM_PROCESSOR_H
