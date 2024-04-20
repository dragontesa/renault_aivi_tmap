
/*
 * socket_client.h
 *
 *  Created on: Feb 24, 2016
 *      Author: MAN1SZH
 */

#ifndef SOCKET_CLIENT_H_
#define SOCKET_CLIENT_H_

#include <stddef.h>

typedef void (*gnss_socket_client_callback) (unsigned int timeStamp, const char* buffer, size_t bufferLen, void *userdata);

void gnss_socket_client_init();
void gnss_socket_client_start(gnss_socket_client_callback callback, void *userdata);
void gnss_socket_client_stop();

void sensor_socket_client_init();

#endif /* SENSOR_INC_TEST_H_ */

