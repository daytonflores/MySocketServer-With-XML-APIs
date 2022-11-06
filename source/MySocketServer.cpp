#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/pugiconfig.hpp"
#include "../include/pugixml.hpp"
#include "../include/MySocketClient.h"
#include "../include/MySocketServer.h"

#define TCP_PROTOCOL	(0)

std::string MySocketServer::get_address() {
	return address;
}

int MySocketServer::get_port() {
	return port;
}

int MySocketServer::set_address(std::string _address) {
	address = _address;

	return EXIT_SUCCESS;
}

int MySocketServer::set_port(int _port) {
	port = _port;

	return EXIT_SUCCESS;
}

int MySocketServer::get_file_descriptor() {
	return file_descriptor;
}

char* MySocketServer::get_buffer() {
	return buf;
}

int MySocketServer::get_bytes_received() {
	return bytes_received;
}

int MySocketServer::get_bytes_sent() {
	return bytes_sent;
}

/**
 * Create socket server
 *		- Domain of AF_INET for IPv4 Internet protocols
 *		- Type of SOCK_STREAM for TCP (sequenced, reliable, two-way,
 *		  connection-based byte streams)
 *		- Protocol of 0 because only 1 protocol exists to support SOCK_STREAM
 *		- If socket is successful, socket file descriptor will be returned
 */
int MySocketServer::create_tcp_ipv4() {
	int return_value;

	return_value = socket(AF_INET, SOCK_STREAM, TCP_PROTOCOL);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		file_descriptor = return_value;
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

/**
 * Bind the socket to IP and port
 *		- Family of AF_INET for IPv4 Internet protocols
 *		- Port given by command line argument (if none is given then
 *		  default of DEFAULT_SOCK_SERVER_PORT)
 *			- htons used for endian conversion
 *		- Address given by command line argument (if none is given then
 *		  default of DEFAULT_SOCK_SERVER_ADDR)
 *			- inet_pton used to load string IPv4 address to unsigned long
 */
int MySocketServer::bind_tcp_ipv4() {
	int return_value;

	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &socket_address.sin_addr);
	return_value = bind(file_descriptor, (sockaddr*)(&socket_address), sizeof(socket_address));

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

/**
 * Set the socket to listen
 *		- SOMAXCONN defines the maximum length to which the queue of pending
 *		  connections for sockfd may grow. It can be viewed on Linux machines with:
 *			cat /proc/sys/net/core/somaxconn
 */
int MySocketServer::mark_passive() {
	int return_value;

	return_value = listen(file_descriptor, SOMAXCONN);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int MySocketServer::accept_client(MySocketClient *source){
	int return_value;

	return_value = accept(file_descriptor, (sockaddr*)(&source->socket_address), &source->socket_address_length);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		source->file_descriptor = return_value;
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int MySocketServer::close_file_descriptor() {
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

void MySocketServer::receive_request_from_client(MySocketClient* source) {

	pugi::xml_node node_request;
	pugi::xml_node node_command;
	pugi::xml_node node_data;


	memset(buf, 0, BUF_SIZE);

	bytes_received = recv(source->file_descriptor, buf, BUF_SIZE, 0);

	if (bytes_received > 0) {
		xml = request.load_buffer(buf, bytes_received);

		node_request = request.child("request");
		node_command = request.child("request").child("command");
		node_data = request.child("request").child("data");

		std::cout << "Received XML Request: " << xml.description() << std::endl;
		//std::cout << "Request = " << node_request.child_value() << std::endl;
		std::cout << "\tCommand = " << node_command.child_value() << std::endl;
		//std::cout << "Data = " << node_data.child_value() << std::endl;
	}
}

int MySocketServer::process_request() {
	int return_value;

	return_value = EXIT_SUCCESS;

	if (return_value != EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

void MySocketServer::send_response_to_client(MySocketClient* source) {
	bytes_sent = send(source->file_descriptor, buf, bytes_received + 1, 0);
}
