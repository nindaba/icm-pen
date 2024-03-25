//
// Created by Arthur Nindaba on 23/03/2024.
//

#ifndef PEN_MOTION_PEN_CONSOLE_H
#define PEN_MOTION_PEN_CONSOLE_H

#define CONSOLE_PROMPT "pen $"
#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

typedef void (*register_fn)(void);

void init_console(register_fn reg_fn);

#endif //PEN_MOTION_PEN_CONSOLE_H
