#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/SocketClient.h"

/**
 * \def		NI_NONE
 * \brief	The value to send to getnameinfo for zero flags
 */
#define NI_NONE	(0)

SocketClient::SocketClient() {
	socket_address_length = sizeof(socket_address);
	memset(host_name, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);
}

char* SocketClient::get_host_name() {
	return host_name;
}

char* SocketClient::get_service() {
	return service;
}

unsigned short SocketClient::get_sin_port() {
	return ntohs(socket_address.sin_port);
}

int SocketClient::set_name_info() {
	int return_value;

	return_value = getnameinfo((sockaddr*)&socket_address, socket_address_length, host_name, NI_MAXHOST, service, NI_MAXSERV, NI_NONE);

	if (return_value != EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int SocketClient::set_ipv4_info() {
	char* temp;
	int return_value;

	temp = (char*)inet_ntop(AF_INET, &socket_address.sin_addr, host_name, NI_MAXHOST);

	if (temp == NULL) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int SocketClient::close_file_descriptor() {
	int return_value;

	return_value = close(file_descriptor);

	if (return_value != EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}
