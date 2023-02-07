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
#include "../include/SocketClient.h"
#include "../include/SocketServer.h"

/**
 * \def		DEFAULT_SOCKET_SERVER_ADDR
 * \brief	To be used as the IP address of the socket server if no command-line
 *		argument is given
 */
#define DEFAULT_SOCKET_SERVER_ADDR	("127.0.0.1")

/**
 * \def		DEFAULT_SOCKET_SERVER_PORT
 * \brief	To be used as the port of the socket server if no command-line
 *		argument is given
 */
#define DEFAULT_SOCKET_SERVER_PORT	(5000)

/**
 * \def		MAX_NUM_OF_ARGS
 * \brief	The max number of command-line arguments to expect
 */
#define MAX_NUM_OF_ARGS			(2 + 1)

/**
 * \fn		void main
 * \param	argc	The amount of command-line arguments given during execution
 * \param	argv	Each element of this array points to the string of each
 *			command-line argument
 * \return	Returns EXIT_FAILURE upon any failures encountered,
 *		and EXIT_SUCCESS otherwise
 * \brief	Creates a socket server, connects with client, and processes XML
 *		requests from client and sends XML responses to client
 */
int main(int argc, char* argv[]){

	/**
	 * \var		return_code
	 * \brief	Holds return value of any invocations returned status codes
	 */
	int return_code;

	/**
	 * \var		destination
	 * \brief	Server object to process commands from client
	 */
	SocketServer destination;

	/**
	 * \var		source
	 * \brief	Client object sending commands to server via socket
	 */
	SocketClient source;

	/**
	 * Set Socket Server port and address based on command line arguments
	 *	- Port must be integer so it can be passed to htons
	 *	- Address must be string so it can be passed to inet_pton
	 */
	if (argc > MAX_NUM_OF_ARGS) {
		std::cerr << "FAILURE: Invalid number of arguments..." << std::endl;
		return EXIT_FAILURE;
	}
	else if (argc == MAX_NUM_OF_ARGS) {
		destination.set_address(argv[1]);
		destination.set_port(std::stoi(argv[2]));
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
	 * Create server object
	 */
	std::cout << "Creating TCP Socket Server in IPv4 domain..." << std::endl;
	return_code = destination.create_tcp_ipv4();
	if (return_code != EXIT_SUCCESS) {
		std::cerr << "FAILURE: Could not create TCP Socket Server in IPv4 domain" << std::endl << "Aborting..." << std::endl;
		return EXIT_FAILURE;
	}

	/**
	 * Bind the server object to IP and port
	 */
	std::cout << "Binding server to " << destination.get_address() << ":" << destination.get_port() << std::endl;
	return_code = destination.bind_tcp_ipv4();
	if (return_code != EXIT_SUCCESS) {
		std::cerr << "FAILURE: Could not bind server to " << destination.get_address() << ":" << destination.get_port() << std::endl << "Aborting..." << std::endl;
		return EXIT_FAILURE;
	}

	/**
	 * Mark the server as passive to listen for clients attempting to
	 * initiate a connection
	 */
	std::cout << "Marking server as passive (to listen for clients) with backlog of " << SOMAXCONN << "..." << std::endl;
	return_code = destination.mark_passive();
	if (return_code != EXIT_SUCCESS) {
		std::cerr << "FAILURE: Could not mark server as passive with backlog of " << SOMAXCONN << std::endl << "Aborting..." << std::endl;
		return EXIT_FAILURE;
	}

	/**
	 * Once a client attempts to connect, accept the call
	 */
	std::cout << "Listening for clients..." << std::endl;
	return_code = destination.accept_client(&source);
	if (return_code != EXIT_SUCCESS) {
		std::cerr << "FAILURE: Found a client but could not accept connection" << std::endl << "Aborting..." << std::endl;
		return EXIT_FAILURE;
	}
	else {
		std::cout << "Server has accepted a client connection!" << std::endl;
	}

	/**
	 * Grab the hostname (or IP address) + port number the client is
	 * connecting from
	 */
	return_code = source.set_name_info();
	if (return_code == EXIT_SUCCESS) {
		std::cout << "Established TCP socket connection to client " << source.get_host_name() << ":" << source.get_service() << std::endl;
	}
	else {
		return_code = source.set_ipv4_info();

		if(return_code != EXIT_SUCCESS) {
			std::cerr << "FAILURE: Could not establish TCP socket connection to client" << std::endl << "Aborting..." << std::endl;
			return EXIT_FAILURE;
		}
		else {
			std::cout << "Established TCP socket connection to client " << source.get_host_name() << ":" << source.get_sin_port() << std::endl;
		}
	}

	/**
	 * Close the server file descriptor now that socket connection has been established
	 */
	std::cout << "Closing server file descriptor..." << std::endl;
	return_code = destination.close_file_descriptor();
	if (return_code != EXIT_SUCCESS) {
		std::cerr << "FAILURE: Could not close the server file descriptor..." << std::endl << "Aborting..." << std::endl;
		return EXIT_FAILURE;
	}

	/**
	 * Server will continue to process requests from client until
	 * client hangs up
	 */
	while (1) {

		/**
		 * Clear the buffer and then wait for client to send data. If client has
		 * hung up then close the client file descriptor and break from loop.
		 * Otherwise, server will validate and process the request and then
		 * send the response to the client. Once response is sent, server
		 * will again clear the buffer and wait for client to send data
		 */
		std::cout << "Waiting on client to send request..." << std::endl << std::endl;
		destination.receive_request_from_client(&source);
		if (destination.get_bytes_received() < EXIT_SUCCESS) {
			std::cerr << "FAILURE: Error receiving request from client" << std::endl << "Aborting..." << std::endl;
			return EXIT_FAILURE;
		}
		else if (destination.get_bytes_received() == EXIT_SUCCESS) {
			std::cout << "Connection to client lost! Closing client file descriptor..." << std::endl;
			return_code = source.close_file_descriptor();
			if (return_code != EXIT_SUCCESS) {
				std::cerr << "FAILURE: Could not close the client file descriptor" << std::endl << "Aborting..." << std::endl;
				return EXIT_FAILURE;
			}

			break;
		}
		else {
			std::cout << "Validating request from client..." << std::endl;
			destination.validate_request();

			std::cout << "Processing request from client..." << std::endl;
			destination.process_request();

			std::cout << "Sending response to client..." << std::endl;
			destination.send_response_to_client(&source);
			if (destination.get_bytes_sent() < EXIT_SUCCESS) {
				std::cerr << "FAILURE: Error sending response to client" << std::endl << "Aborting..." << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	std::cout << "Clean-up complete! Exiting gracefully..." << std::endl;

	return EXIT_SUCCESS;
}
