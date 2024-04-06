//
// Created by Arthur Nindaba on 05/04/2024.
//

#ifndef PEN_MOTION_UDP_SERVER_H
#define PEN_MOTION_UDP_SERVER_H

typedef void (*data_provider)(char *data);

void star_server(data_provider provider);

#endif //PEN_MOTION_UDP_SERVER_H
