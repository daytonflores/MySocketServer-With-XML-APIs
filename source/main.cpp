#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE					(1024)
#define DEFAULT_SOCKET_SERVER_ADDR	("127.0.0.1")
#define DEFAULT_SOCKET_SERVER_PORT	(5000)
//#define EXIT_FAILURE				(1)
#define EXIT_FAILURE_N				(-1)
//#define EXIT_SUCCESS				(0)
#define MAX_NUM_OF_ARGS				(2 + 1)
#define NI_NONE						(0)

using namespace std;

int main(int argc, char* argv[]){

	char buf[BUF_SIZE];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	char* rc_charptr;
	int bytes_received;
	int rc_int;
	int socket_client_fd;
	int socket_server_fd;
	int socket_server_port;
	sockaddr_in socket_client;
	sockaddr_in socket_server;
	socklen_t socket_client_size;
	string socket_server_addr;

	/**
	 * Set socket server port and address based on command line arguments
	 *		- Port must be integer so it can be passed to htons
	 *		- Address must be string so it can be passed to inet_pton
	 */
	if (argc > MAX_NUM_OF_ARGS) {
		cerr << "FAILURE: Invalid number of arguments..." << endl;
		return EXIT_FAILURE;
	}
	else if (argc == MAX_NUM_OF_ARGS) {
		socket_server_addr = argv[1];
		socket_server_port = stoi(argv[2]);
	}
	else if (argc > 1) {
		socket_server_addr = argv[1];
		socket_server_port = DEFAULT_SOCKET_SERVER_PORT;
	}
	else {
		socket_server_addr = DEFAULT_SOCKET_SERVER_ADDR;
		socket_server_port = DEFAULT_SOCKET_SERVER_PORT;
	}

	cout << "Using socket server address " << socket_server_addr << " with port " << socket_server_port << "!" << endl;

	/**
	 * Create socket server
	 *		- Domain of AF_INET for IPv4 Internet protocols
	 *		- Type of SOCK_STREAM for TCP (sequenced, reliable, two-way,
	 *		  connection-based byte streams)
	 *		- Protocol of 0 because only 1 protocol exists to support SOCK_STREAM
	 *		- If socket is successful, socket file descriptor will be returned
	 */
	rc_int = socket(AF_INET, SOCK_STREAM, 0);

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not create socket server..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Created socket server!" << endl;
		socket_server_fd = rc_int;
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
	socket_server.sin_family = AF_INET;
	socket_server.sin_port = htons(socket_server_port);
	inet_pton(AF_INET, socket_server_addr.c_str(), &socket_server.sin_addr);
	rc_int = bind(socket_server_fd, (sockaddr*)(&socket_server), sizeof(socket_server));

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not bind socket server to " << socket_server_addr << ":" << socket_server_port << "..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Binded socket server to " << socket_server_addr << ":" << socket_server_port << "!" << endl;
	}

	/**
	 * Set the socket to listen
	 *		- SOMAXCONN defines the maximum length to which the queue of pending
	 *		  connections for sockfd may grow. It can be viewed on Linux machines with:
	 *			cat /proc/sys/net/core/somaxconn
	 */
	rc_int = listen(socket_server_fd, SOMAXCONN);

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not set socket server to listen using backlog of " << SOMAXCONN << "..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Set socket server to listen using backlog of " << SOMAXCONN << "!" << endl;
	}

	cout << "Listening..." << endl;

	/**
	 * Accept the call
	 */
	socket_client_size = sizeof(socket_client);
	rc_int = accept(socket_server_fd, (sockaddr*)&socket_client, &socket_client_size);

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not set socket server to accept socket client connection..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Socket server has accepted socket client connection!" << endl;
		socket_client_fd = rc_int;
	}

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	rc_int = getnameinfo((sockaddr*)&socket_client, socket_client_size, host, NI_MAXHOST, service, NI_MAXSERV, NI_NONE);

	if (rc_int == EXIT_SUCCESS) {
		cout << "SUCCESS: Host " << host << " connected on port service " << service << "!" << endl;
	}
	else {
		rc_charptr = (char*)inet_ntop(AF_INET, &socket_client.sin_addr, host, NI_MAXHOST);

		if (rc_charptr == NULL) {
			cerr << "FAILURE: Host " << host << " could not connect on port service " << ntohs(socket_client.sin_port) << "..." << endl;
			return EXIT_FAILURE;
		}
		else {
			cout << "SUCCESS: Host " << host << " connected on port service " << ntohs(socket_client.sin_port) << "!" << endl;
		}
	}

	/**
	 * Close the socket server file descriptor now that socket connection has been established
	 */
	rc_int = close(socket_server_fd);

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not close the socket server file descriptor..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Closed the socket server file descriptor!" << endl;
	}

	/**
	 * While receiving, echo message
	 */
	while (1) {

		/**
		 * Clear the buffer and then wait for socket client to send data
		 */
		memset(buf, 0, BUF_SIZE);

		bytes_received = recv(socket_client_fd, buf, BUF_SIZE, 0);

		if (bytes_received == EXIT_FAILURE_N) {
			cerr << "FAILURE: Error receiving bytes from socket client. Exiting..." << endl;
			return EXIT_FAILURE;
		}
		else if (bytes_received == 0) {
			cout << "SUCCESS: Disconnecting socket client!" << endl;
		}
		else {
			cout << string(buf, 0, bytes_received) << endl;
			send(socket_client_fd, buf, bytes_received + 1, 0);
		}
	}

	/**
	 * Close the socket client file descriptor
	 */
	rc_int = close(socket_client_fd);

	if (rc_int == EXIT_FAILURE_N) {
		cerr << "FAILURE: Could not close the socket client file descriptor..." << endl;
		return EXIT_FAILURE;
	}
	else {
		cout << "SUCCESS: Closed the socket client file descriptor!" << endl;
	}


	return EXIT_SUCCESS;
}