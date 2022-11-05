#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/pugiconfig.hpp"
#include "../include/pugixml.hpp"
#include "../include/MySocketClient.h"
#include "../include/MySocketServer.h"

#define DEFAULT_SOCKET_SERVER_ADDR	("127.0.0.1")
#define DEFAULT_SOCKET_SERVER_PORT	(5000)
#define MAX_NUM_OF_ARGS				(2 + 1)

using namespace std;

int main(int argc, char* argv[]){

	int rc_int;
	MySocketServer destination;
	MySocketClient source;

	/**
	 * Set Socket Server port and address based on command line arguments
	 *		- Port must be integer so it can be passed to htons
	 *		- Address must be string so it can be passed to inet_pton
	 */
	if (argc > MAX_NUM_OF_ARGS) {
		cerr << "FAILURE: Invalid number of arguments..." << endl;
		return EXIT_FAILURE;
	}
	else if (argc == MAX_NUM_OF_ARGS) {
		destination.set_address(argv[1]);
		destination.set_port(stoi(argv[2]));
	}
	else if (argc > 1) {
		destination.set_address(argv[1]);
		destination.set_port(DEFAULT_SOCKET_SERVER_PORT);
	}
	else {
		destination.set_address(DEFAULT_SOCKET_SERVER_ADDR);
		destination.set_port(DEFAULT_SOCKET_SERVER_PORT);
	}

	/**
	 * Create Socket Server
	 */
	cout << "Creating TCP Socket Server in IPv4 domain..." << endl;
	rc_int = destination.create_tcp_ipv4();
	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Could not create TCP Socket Server in IPv4 domain" << endl << "Aborting..." << endl;
		return EXIT_FAILURE;
	}

	/**
	 * Bind the Socket Server to IP and port
	 */
	cout << "Binding Socket Server to " << destination.get_address() << ":" << destination.get_port() << endl;
	rc_int = destination.bind_tcp_ipv4();
	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Could not bind Socket Server to " << destination.get_address() << ":" << destination.get_port() << endl << "Aborting..." << endl;
		return EXIT_FAILURE;
	}

	/**
	 * Mark the Socket Server as passive to listen for Socket Clients
	 */
	cout << "Marking Socket Server as passive (to listen for Socket Clients) with backlog of " << SOMAXCONN << "..." << endl;
	rc_int = destination.mark_passive();
	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Could not mark Socket Server as passive with backlog of " << SOMAXCONN << endl << "Aborting..." << endl;
		return EXIT_FAILURE;
	}
	cout << "Listening..." << endl;

	/**
	 * Once a Socket Client attempts to connect, accept the call
	 */
	rc_int = destination.accept_client(&source);

	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Found a Socket Client but could not accept connection" << endl << "Aborting..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "Socket Server has accepted a Socket Client connection!" << endl;
	}

	rc_int = source.set_name_info();

	if (rc_int == EXIT_SUCCESS) {
		cout << "Established TCP connection to Socket Client " << source.get_host_name() << ":" << source.get_service() << endl;
	}
	else {
		rc_int = source.set_ipv4_info();

		if(rc_int != EXIT_SUCCESS) {
			cerr << "FAILURE: Could not establish TCP connection to Socket Client" << endl << "Aborting..." << endl;
			return EXIT_FAILURE;
		}
		else {
			cout << "Established TCP connection to Socket Client " << source.get_host_name() << ":" << source.get_sin_port() << endl;
		}
	}

	/**
	 * Close the socket server file descriptor now that socket connection has been established
	 */
	cout << "Closing Socket Server file descriptor..." << endl;
	rc_int = destination.close_file_descriptor();

	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Could not close the socket server file descriptor..." << endl;
		return EXIT_FAILURE;
	}

	/**
	 * While receiving, echo message
	 */
	cout << "Ready to accept data from Socket Client..." << endl;
	while (1) {

		/**
		 * Clear the buffer and then wait for socket client to send data
		 */
		destination.receive_request_from_client(&source);

		if (destination.get_bytes_received() < EXIT_SUCCESS) {
			cerr << "FAILURE: Error receiving bytes from Socket Client" << endl << "Aborting..." << endl;
			return EXIT_FAILURE;
		}
		else if (destination.get_bytes_received() == EXIT_SUCCESS) {
			cout << "Disconnecting from Socket Client" << endl;
			break;
		}
		else {
			rc_int = destination.process_request();

			if (rc_int != EXIT_SUCCESS) {
				cerr << "FAILURE: Could not process request from client" << endl << "Aborting..." << endl;
				return EXIT_FAILURE;
			}

			destination.send_response_to_client(&source);

			if (destination.get_bytes_received() < EXIT_SUCCESS) {
				cerr << "FAILURE: Error sending bytes to Socket Client" << endl << "Aborting..." << endl;
				return EXIT_FAILURE;
			}
		}
	}

	/**
	 * Close the socket client file descriptor
	 */
	cout << "Closing Socket Client file descriptor..." << endl;
	rc_int = source.close_file_descriptor();

	if (rc_int != EXIT_SUCCESS) {
		cerr << "FAILURE: Could not close the Socket Client file descriptor" << endl << "Aborting..." << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}